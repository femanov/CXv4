#include <stdio.h>
#include <strings.h>

#include <casdef.h>
#include <fdManager.h>
#include <gddApps.h> // For gddAppType_value

#include "misc_macros.h" // For safe_realloc()
#include "misc_iso8601.h"

#include "epics2smth.h"

#include "libcas2cx_conv.h"


//////////////////////////////////////////////////////////////////////

e2s_backend_t *the_backend;

//////////////////////////////////////////////////////////////////////

extern "C"
{

typedef int (*name_compare_t)(const char *s1, const char *s2);

static name_compare_t name_compare = strcasecmp;

} /* extern "C" */

//////////////////////////////////////////////////////////////////////

enum
{
    MON_TYPE_UNUSED        = 0,
    MON_TYPE_REGISTERING   = 1,
    MON_TYPE_DOES_EXIST    = 2,
    MON_TYPE_DOESN_T_EXIST = 3,
    MON_TYPE_DESTROYING    = 4,
    MON_TYPE_SEARCH        = 5,
    MON_TYPE_WORKS         = 6,
};

// Note: monitors implementation is based on cxscheduler.c's trec_t
typedef struct
{
    int         is_used; 
    int         next;    // Link to next
    int         prev;    // Link to previous

    const char *name;
    cxdtype_t   chn_dtype;
    int         max_nelems;
    int         is_rw;

    void       *PV_ptr;  // Pointer to casPV object
    casAsyncPVExistIO     *AS_ptr;   // Deletion rules: when !=NULL, it is epics2smth's responsibility to delete it; but after postIOCompletion() it is =NULL and object's deletion becomes libcas responsibility
    e2s_backend_obj_ref_t  obj_ref;  // Reference to backend's object
} moninfo_t;

enum {MONS_ALLOC_INC = 100};
static moninfo_t  *mons_list        = NULL;
static int         mons_list_allocd = 0;

static int         frs_mon = -1;
static int         lst_mon = -1;
static int         avl_mon = -1;

//////////////////////////////////////////////////////////////////////

static int  GetMonSlot(void)
{
  int        mid;
  moninfo_t *mp;
  moninfo_t *new_list;
  int        x;
  int        prev;
  int        next;

    if (avl_mon < 0)
    {
        /* Okay, let's grow the list... */
        new_list = (moninfo_t*)safe_realloc(mons_list,
                                            sizeof(moninfo_t)
                                            *
                                            (mons_list_allocd + MONS_ALLOC_INC));
        if (new_list == NULL) return -1;

        /* ...zero-fill newly allocated space... */
        bzero(new_list + mons_list_allocd,
              sizeof(moninfo_t) * MONS_ALLOC_INC);
        /* ...initialize it... */
        for (x = mons_list_allocd;  x < mons_list_allocd + MONS_ALLOC_INC - 1;  x++)
            new_list[x].next = x + 1;
        new_list[mons_list_allocd + MONS_ALLOC_INC - 1].next = -1;
        avl_mon = mons_list_allocd;   // Store first of newly allocated block as first-available...
        if (avl_mon == 0) avl_mon++;  // ...but skip 0th (for 0 to be "invalid")
        /* ...and record its presence */
        mons_list         = new_list;
        mons_list_allocd += MONS_ALLOC_INC;
    }
    
    mid = avl_mon;
    mp = mons_list + mid;
    // Remove from "available" list
    avl_mon = mp->next;
    // Place into "active" list (into its head)
    prev = -1;
    next = frs_mon;
    mp->prev = prev;
    frs_mon = mid;
    mp->next = next;
    if (next >= 0) mons_list[next].prev = mid; else lst_mon = mid;

    mp->is_used = MON_TYPE_REGISTERING;

    return mid;
}

static void      RlsMonSlot(int mid)
{
  moninfo_t *mp = mons_list + mid;
  int        prev;
  int        next;

    safe_free((void*)(mp->name));
    mp->name = NULL;

    if (mp->AS_ptr != NULL)
    {
        delete mp->AS_ptr;
        mp->AS_ptr = NULL;
    }

    // Manage the list connections
    prev = mp->prev;
    next = mp->next;
    // a. Remove from "active" list
    if (prev < 0) frs_mon = next; else mons_list[prev].next = next;
    if (next < 0) lst_mon = prev; else mons_list[next].prev = prev;
    // b. Place into "available" list (into its head)
    mp->next = avl_mon;
    avl_mon = mid;

    mp->is_used = MON_TYPE_UNUSED;
}

static moninfo_t*AccessMonSlot(int mid)
{
    return mons_list + mid;
}

static int       FindMonSlotWithName(const char *name)
{
  int        mid;
  moninfo_t *mp;

    for (mid = frs_mon;  mid >= 0; mid = mp->next)
    {
        mp = mons_list + mid;
//fprintf(stderr, "mid=%d mp->name=<%s> name=<%s>\n", mid, mp->name, name);
        if (name_compare(name, mp->name) == 0) return mid;
    }

//fprintf(stderr, "ret=%d\n", mid);
    return mid;
}

static int CheckMid_unlocked(int mid)
{
    if (mid > 0  &&  mid < mons_list_allocd  &&
        mons_list[mid].is_used != MON_TYPE_UNUSED)
        return 0;

    return -1;
}

//////////////////////////////////////////////////////////////////////
static pthread_t        e2s_threadid;
static pthread_mutex_t  e2s_mutex;

static void do_lock  (void)
{
    if (pthread_mutex_trylock(&e2s_mutex) == 0) return;
    pthread_mutex_lock  (&e2s_mutex);
}

static void do_unlock(void)
{
    pthread_mutex_unlock(&e2s_mutex);
}

static void e2s_lock  (void)
{
    if (!pthread_equal(pthread_self(), e2s_threadid)) do_lock();
}

static void e2s_unlock(void)
{
    if (!pthread_equal(pthread_self(), e2s_threadid)) do_unlock();
}
//////////////////////////////////////////////////////////////////////

static casEventMask update_mask;

//////////////////////////////////////////////////////////////////////

static caStatus data2gdd(gdd *gp, int max_nelems,
                         cxdtype_t dtype, int nelems, void *data,
                         int some_flags, cx_time_t timestamp)
{
  aitEnum         value_type;

  aitString      *aisp;  // AItString Pointer

  struct timespec tspc;

    value_type = dtype2aitEnum(dtype);
//fprintf(stderr, "dtype=%d value_type=%d gp=%p\n", dtype, value_type, gp);
    if      (value_type == aitEnumInvalid) return S_casApp_noSupport;
    // Strings are handled separately
    else if (value_type == aitEnumString)
    {
        /* Note: we MUST set the type first and only afterwards
                 we may change string's content.
                 That's because setPrimType() does some cleanup and
                 than calls init() on a newly "born" string */
        gp->setPrimType(aitEnumString);
        aisp = (aitString*)(gp->dataAddress());
//fprintf(stderr, "s=%p:%s nelems=%d max_nelems=%d gp=%p aisp=%p\n", data, data, nelems, max_nelems, gp, aisp);
//fprintf(stderr, "\t<%s>\n", data);
        aisp->installConstBuf((char*)data, nelems, max_nelems);
    }
    // Vector?
    else if (max_nelems != 1)
    {
        gp->adjust(NULL, data, value_type);
        gp->setBound(0, 0, nelems);
    }
    // Scalar
    else
    {
        switch (dtype)
        {
            case CXDTYPE_INT32:  *gp = *((  int32*)data);     break;
            case CXDTYPE_UINT32: *gp = *(( uint32*)data);     break;
            case CXDTYPE_INT16:  *gp = *((  int16*)data);     break;
            case CXDTYPE_UINT16: *gp = *(( uint16*)data);     break;
            case CXDTYPE_DOUBLE: *gp = *((float64*)data);     break;
            case CXDTYPE_SINGLE: *gp = *((float32*)data);     break;
            //case CXDTYPE_INT64:  prototype = *((  int64*)data);     break;
            //case CXDTYPE_UINT64: prototype = *(( uint64*)data);     break;
            case CXDTYPE_INT8:   *gp = *((  int8 *)data);     break;
            case CXDTYPE_UINT8:  *gp = *(( uint8 *)data);     break;
            default:             return S_casApp_noSupport;
        }
    }

    tspc.tv_sec  = timestamp.sec;
    tspc.tv_nsec = timestamp.nsec;
    gp->setTimeStamp(&tspc);

    return S_casApp_success;
}

//////////////////////////////////////////////////////////////////////

class e2s_Server : public caServer
{
 public:
    e2s_Server();

    pvExistReturn        pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName);
    pvAttachReturn       pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName);
};

class e2s_gdd : public gdd
{
 public:
    e2s_gdd(cxdtype_t my_dtype, aitUint32 my_max_nelems) :
        gdd(gddAppType_value, aitEnumInvalid/* Deliberately so to prevent unneeded memory allocation */,
            (my_max_nelems == 1  ||  my_dtype == CXDTYPE_TEXT)? 0    : 1,
            (my_max_nelems == 1  ||  my_dtype == CXDTYPE_TEXT)? NULL : &my_max_nelems)
        {};
};

class e2s_PV     : public casPV
{
 private:
    e2s_gdd      upd_gdd;
    int          mid;
    int          is_interesting;

 public:
    e2s_PV(int my_mid, cxdtype_t my_dtype, int my_max_nelems);
    virtual void destroy () {/* Implicitly DO NOTHING */}
    virtual aitEnum bestExternalType () const;
    virtual unsigned maxDimension () const;
    virtual aitIndex maxBound ( unsigned dimension ) const;

    virtual caStatus interestRegister () {/*fprintf(stderr, "%p %s\n", this, __FUNCTION__);*/ is_interesting = 1; return S_casApp_success;}
    virtual void     interestDelete   () {/*fprintf(stderr, "%p %s\n", this, __FUNCTION__);*/ is_interesting = 0;}

    void              setDimension(int d, int d0size);

    void              update(int max_nelems, // max_nelems=AccessMonSlot(mid)->max_nelems
                             cxdtype_t dtype, int nelems, void *value,
                             int some_flags, cx_time_t timestamp);

    virtual caStatus  read  (const casCtx &ctx, gdd &prototype);
    virtual caStatus  write (const casCtx &ctx, const gdd &value);

    virtual const char * getName () const;
};

//------------

e2s_Server::e2s_Server() : caServer()
{
    update_mask = valueEventMask();
//fprintf(stderr, "%s(): update_mask=%d\n", __FUNCTION__, *((int*)(&update_mask)));
}

pvExistReturn        e2s_Server::pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName)
{
  int                    mid;
  moninfo_t             *mp;
  e2s_backend_obj_ref_t  obj_ref;
  e2s_PV                *PV_p;

if (pPVAliasName[0]!='a' || pPVAliasName[1]!='s' || pPVAliasName[2]!='d' || pPVAliasName[3]!='f') return pverDoesNotExistHere;

fprintf(stderr, "%s %s(): %s\n", strcurtime_msc(), __FUNCTION__, pPVAliasName);

    // An optional check for name validity, should be stateless, so no locking required
    if (the_backend->check_name != NULL  &&
        the_backend->check_name(pPVAliasName) < 0)
        return pverDoesNotExistHere;

    e2s_lock();

    // Is this name already registered?
    mid = FindMonSlotWithName(pPVAliasName);
    if (mid >= 0)
    {
        mp = AccessMonSlot(mid);
        /*
            Note: casdef.h states
                // Return pverDoesNotExistHere if too many simultaneous
                // asynchronous IO operations are pending against the server. 
                // The client library will retry the request at some time
                // in the future.
            in a comment for caServer::pvExistTest() method declaration
            So, we return "DoesNot" for already existing but yet-not-found ones.
        */
fprintf(stderr, "mp->is_used=%d\n", mp->is_used);
        if (mp->is_used == MON_TYPE_SEARCH) goto RETURN_DOESNOT;
        goto RETURN_EXISTS;
    }

    // No -- OK, let's register
    mid = GetMonSlot();
    if (mid < 0) goto RETURN_DOESNOT;
    mp = AccessMonSlot(mid);
    // The name
    if ((mp->name = strdup(pPVAliasName)) == NULL)
    {
        RlsMonSlot(mid);
        goto RETURN_DOESNOT;
    }
fprintf(stderr, "MID=%d mp->name=<%s>\n", mid, mp->name);

    // ...and try to register
    /* Mark "not-fully-registered"... */
    mp->is_used = MON_TYPE_REGISTERING;
    e2s_unlock();
    obj_ref = the_backend->add_chan(pPVAliasName, mid);
    e2s_lock();
    mp = AccessMonSlot(mid); // Re-cache
    if (obj_ref == NULL)
    {
        RlsMonSlot(mid);
        goto RETURN_DOESNOT;
    }
    mp->obj_ref = obj_ref;

    // Had the state changed?
    if (mp->is_used != MON_TYPE_REGISTERING)
    {
        if (mp->is_used == MON_TYPE_DOES_EXIST)
        {
            mp->is_used = MON_TYPE_WORKS;

            PV_p = new e2s_PV(mid, mp->chn_dtype, mp->max_nelems);
            if (PV_p == NULL) 
            {
                mp->is_used = MON_TYPE_DOESN_T_EXIST;
                RlsMonSlot(mid);
                goto RETURN_DOESNOT;
            }
            mp->PV_ptr = PV_p;

            goto RETURN_EXISTS;
        }
        else
        {
            RlsMonSlot(mid);
            goto RETURN_DOESNOT;
        }
    }

    // No -- we are waiting for result, so allocate an casAsyncPVExistIO object
    // At this point the new object implicitly gets information about name-being-queried
    // (that is done in its I-proxy constructor, see casAsyncPVExistIOI::casAsyncPVExistIOI() in casAsyncPVExistIOI.cpp)
    mp->AS_ptr = new casAsyncPVExistIO(ctx);
    if (mp->AS_ptr == NULL)
    {
        if (the_backend->del_chan != NULL)
        {
            mp->is_used = MON_TYPE_DESTROYING;
            e2s_unlock();
            the_backend->del_chan(obj_ref);
            e2s_lock();
        }
        RlsMonSlot(mid);
        goto RETURN_DOESNOT;
    }

    /*...and here should mark "waiting for existence confirmation" */
    mp->is_used = MON_TYPE_SEARCH;
    return pverAsyncCompletion;

 RETURN_EXISTS:
fprintf(stderr, "EXISTS\n");
    e2s_unlock();
    return pverExistsHere;

 RETURN_DOESNOT:
fprintf(stderr, "DOESNOT\n");
    e2s_unlock();
    return pverDoesNotExistHere;
}

pvAttachReturn       e2s_Server::pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName)
{
  int                    mid;
  moninfo_t             *mp;
  e2s_PV                *PV_p;

    e2s_lock();

    // Is this name already registered?
    mid = FindMonSlotWithName(pPVAliasName);
    if (mid < 0) goto RETURN_NOTFOUND;

    mp = AccessMonSlot(mid);
    PV_p = (e2s_PV*)(mp->PV_ptr);
    if (PV_p != NULL) goto RETURN_PV;

fprintf(stderr, "NEW!!!\n");
    PV_p = new e2s_PV(mid, mp->chn_dtype, mp->max_nelems);
    if (PV_p == NULL) goto RETURN_NOTFOUND;
    mp->PV_ptr = PV_p;

 RETURN_PV:
    e2s_unlock();
    return *PV_p;

 RETURN_NOTFOUND:
    e2s_unlock();
    return S_casApp_pvNotFound;
}

e2s_PV::e2s_PV(int my_mid, cxdtype_t my_dtype, int my_max_nelems) :
 casPV(),
 upd_gdd(my_dtype, my_max_nelems)
{
    mid            = my_mid;
    is_interesting = 0;
}

aitEnum e2s_PV::bestExternalType() const
{
  moninfo_t             *mp;
  aitEnum                ret;

    e2s_lock();

    if (CheckMid_unlocked(mid) < 0) ret = aitEnumInvalid;
    else
    {
        mp  = AccessMonSlot(mid);
        ret = dtype2aitEnum(mp->chn_dtype);
    }

    e2s_unlock();
    return ret;
}

unsigned e2s_PV::maxDimension () const
{
  moninfo_t             *mp;
  unsigned               ret;

    e2s_lock();

    if (CheckMid_unlocked(mid) < 0) ret = NULL;
    else
    {
        mp  = AccessMonSlot(mid);
        ret = mp->max_nelems == 1  ||  mp->chn_dtype == CXDTYPE_TEXT? 0 : 1;
    }

    e2s_unlock();
    return ret;
}

aitIndex e2s_PV::maxBound ( unsigned dimension ) const
{
  moninfo_t             *mp;
  aitIndex               ret;

    e2s_lock();

    if (CheckMid_unlocked(mid) < 0) ret = NULL;
    else
    {
        mp  = AccessMonSlot(mid);
        ret = mp->max_nelems == 1  ||  mp->chn_dtype == CXDTYPE_TEXT? 0 : mp->max_nelems; // CAS_Reference.pdf: "Note that for scalar PVs, maxBound() should return zero no matter what dimension its argument specifies."
    }

    e2s_unlock();
    return ret;
}

void      e2s_PV::setDimension(int d, int d0size)
{
  gddBounds  d0bnds;

    d0bnds.set(0, d0size);
    upd_gdd.setDimension(d, &d0bnds);
}

void      e2s_PV::update(int max_nelems,
                         cxdtype_t dtype, int nelems, void *value,
                         int some_flags, cx_time_t timestamp)
{
    if (!is_interesting) return;

    if (data2gdd(&upd_gdd, max_nelems,
                 dtype, nelems, value, some_flags, timestamp) != S_casApp_success) return;

    postEvent(update_mask, upd_gdd);
}

caStatus  e2s_PV::read  (const casCtx &ctx, gdd &prototype)
{
    // A guard against DBR_STSACK_STRING
    if (prototype.isContainer()  ||  prototype.isAtomic()) return S_casApp_noSupport;

//    return data2gdd();
}

caStatus  e2s_PV::write (const casCtx &ctx, const gdd &value)
{
  moninfo_t  *mp;
  caStatus    ret;

  aitEnum     value_type;
  cxdtype_t   dtype;
  int         nelems;
  CxAnyVal_t  val;
  const void *data;

  aitIndex    b_first, b_count;

  const char *cp;

  const aitString      *ast_p;
  const aitFixedString *afs_p;

//fprintf(stderr, "%p %s\n", this, __FUNCTION__);
//fprintf(stderr, "\tprimitiveType()=%d\n", value.primitiveType());

    if (the_backend->snd_data == NULL) return S_casApp_noSupport;

    value_type = value.primitiveType();
    dtype      = aitEnum2dtype(value_type);
    if (dtype == CXDTYPE_UNKNOWN) return S_casApp_noSupport;

    if      (value_type == aitEnumString)
    {
        value.getRef(ast_p);
        data   = ast_p->string();
        nelems = ast_p->length();
    }
    else if (value_type == aitEnumFixedString)
    {
        value.getRef(afs_p);
        data   = afs_p->fixed_string;
        for (nelems = 0;
             nelems < sizeof(afs_p->fixed_string)  &&  afs_p->fixed_string[nelems] != '\0';
             nelems++);
    }
    else
    {
        if (value.dimension() == 0)
            nelems = 1;
        else
        {
            value.getBound(0, b_first, b_count);
            nelems = b_count;
        }
        if      (nelems == 0)
            data = NULL;
        else
            data = value.dataVoid();
    }

    e2s_lock();
    mp = AccessMonSlot(mid);
    ret = the_backend->snd_data(mp->obj_ref, dtype, nelems, data) == 0? S_casApp_success : S_casApp_noSupport;
    e2s_unlock();

    return ret;
}

const char * e2s_PV::getName () const
{
  moninfo_t             *mp;
  const char            *ret;

    e2s_lock();

    if (CheckMid_unlocked(mid) < 0) ret = NULL;
    else
    {
        mp  = AccessMonSlot(mid);
        ret = mp->name;
    }

    e2s_unlock();
    return ret;
}

//////////////////////////////////////////////////////////////////////

extern "C"
{

void e2s_set_found  (int mid, int is_found)
{
  moninfo_t             *mp;
  casAsyncPVExistIO     *asyn_p;
  e2s_PV                *PV_p;

    e2s_lock();

    mp = AccessMonSlot(mid);

    if (CheckMid_unlocked(mid) != 0) goto DO_RETURN;

    if      (mp->is_used == MON_TYPE_REGISTERING)
        mp->is_used = is_found? MON_TYPE_DOES_EXIST : MON_TYPE_DOESN_T_EXIST;
    else if (mp->is_used == MON_TYPE_SEARCH)
    {
        // Change the state
        mp->is_used = is_found? MON_TYPE_WORKS      : MON_TYPE_DOESN_T_EXIST; // The "!is_found" value is almost arbitrary, as this monitor will be destroyed right afterwards below
        //
        if (is_found)
        {
            // create, if fails -- simulate NOTFOUND by setting is_found = 0, mp->is_used = MON_TYPE_DOESN_T_EXIST
            PV_p = new e2s_PV(mid, mp->chn_dtype, mp->max_nelems);
            if (PV_p == NULL) 
            {
                is_found = 0;
                mp->is_used = MON_TYPE_DOESN_T_EXIST;
            }
            else
                mp->PV_ptr = PV_p;
        }

        asyn_p = mp->AS_ptr;
        mp->AS_ptr = NULL; // Get rid of deletion respoisibility, as it is passed to libcas upon call to postIOCompletion()

        e2s_unlock();
        if (is_found)
            asyn_p->postIOCompletion(pverExistsHere);
        else
            asyn_p->postIOCompletion(pverDoesNotExistHere);
        e2s_lock();
        mp = AccessMonSlot(mid); // Re-cache

        // Release the monitor only if NOT found
        if (!is_found)
            RlsMonSlot(mid);
    }

 DO_RETURN:;
    e2s_unlock();
}

void e2s_set_props  (int mid,
                     cxdtype_t dtype, int max_nelems, int is_rw)
{
  moninfo_t             *mp;
  e2s_PV                *PV_p;

    e2s_lock();

    mp = AccessMonSlot(mid);

    if (CheckMid_unlocked(mid) != 0)   goto DO_RETURN;
    if (mp->is_used != MON_TYPE_SEARCH       &&
        mp->is_used != MON_TYPE_REGISTERING  &&
        mp->is_used != MON_TYPE_WORKS) goto DO_RETURN;

    mp->chn_dtype  = dtype;
    mp->max_nelems = max_nelems;
    mp->is_rw      = is_rw;

    PV_p = (e2s_PV*)(mp->PV_ptr);
    if (PV_p != NULL)
        PV_p->setDimension((max_nelems == 1  ||  dtype == CXDTYPE_TEXT)? 0    : 1,
                           (max_nelems == 1  ||  dtype == CXDTYPE_TEXT)? NULL : max_nelems);

 DO_RETURN:;
    e2s_unlock();
}

void e2s_update_data(int mid,
                     cxdtype_t dtype, int nelems, void *value,
                     int some_flags, cx_time_t timestamp)
{
  moninfo_t             *mp;
  e2s_PV                *PV_p;

    e2s_lock();

    mp = AccessMonSlot(mid);

    if (CheckMid_unlocked(mid) != 0)   goto DO_RETURN;
    if (mp->is_used != MON_TYPE_WORKS) goto DO_RETURN;

    PV_p = (e2s_PV*)(mp->PV_ptr);
    PV_p->update(mp->max_nelems, dtype, nelems, value, some_flags, timestamp);

 DO_RETURN:;
    e2s_unlock();
}

void e2s_run(e2s_backend_t *backend)
{
  e2s_Server the_server;

  int        saved_errno;

    the_backend = backend;
    name_compare = the_backend->names_case_sensitive? strcmp : strcasecmp;

    e2s_threadid = pthread_self();
    if ((saved_errno = pthread_mutex_init(&e2s_mutex, NULL)) != 0)
    {
        errno = saved_errno;
        fprintf(stderr, "%s %s(): pthread_mutex_init() failure: %s\n", strcurtime(), __FUNCTION__, strerror(saved_errno));
        exit(1);
    }

    while (1) fileDescriptorManager.process(3600);
}

} /* extern "C" */
