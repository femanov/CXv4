#include <casdef.h>
#include <fdManager.h>
#include <gddApps.h> // For gddAppType_value

#include "cxsd_access.h"

#include "cxsd_sfi.h"
#include "cxsd_fe_epics_cas.h"

#include "libcas2cx_conv.h"


//////////////////////////////////////////////////////////////////////

static casEventMask update_mask;

//////////////////////////////////////////////////////////////////////

static caStatus mondata2gdd(int mid, gdd *gp, int max_nelems)
{
  cxdtype_t       dtype;
  int             nelems;
  void           *data;
  rflags_t        rflags;
  cx_time_t       timestamp;

  aitEnum         value_type;

  aitString      *aisp;  // AItString Pointer

  struct timespec tspc;

    if (cxsd_sfi_get_data(mid, &dtype, &nelems, &data, &rflags, &timestamp) != 0)
        return S_casApp_noSupport;

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
fprintf(stderr, "s=%p:%s nelems=%d max_nelems=%d gp=%p aisp=%p\n", data, data, nelems, max_nelems, gp, aisp);
fprintf(stderr, "\t<%s>\n", data);
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

class fe_epics_Server : public caServer
{
 public:
    fe_epics_Server();

    pvExistReturn        pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName);
    pvAttachReturn       pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName);
};

class fe_epics_gdd : public gdd
{
 public:
    fe_epics_gdd(int app, aitEnum prim, cxdtype_t my_dtype, aitUint32 my_max_nelems) : 
             gdd(app, prim,
                 (my_max_nelems == 1  ||  my_dtype == CXDTYPE_TEXT)? 0    : 1,
                 (my_max_nelems == 1  ||  my_dtype == CXDTYPE_TEXT)? NULL : &my_max_nelems)
             {}
};

class fe_epics_PV     : public casPV
{
 private:
    int        mid;
    cxdtype_t  chn_dtype;
    int        max_nelems;
    int        is_rw;

    fe_epics_gdd upd_gdd;
    int          is_interesting;

 public:
    fe_epics_PV(int my_mid, cxdtype_t my_dtype, int my_max_nelems, int my_is_rw);
    virtual void destroy ();
    virtual aitEnum bestExternalType () const;
    virtual unsigned maxDimension () const;
    virtual aitIndex maxBound ( unsigned dimension ) const;

    virtual caStatus interestRegister () {/*fprintf(stderr, "%p %s\n", this, __FUNCTION__);*/ is_interesting = 1; return S_casApp_success;}
    virtual void     interestDelete   () {/*fprintf(stderr, "%p %s\n", this, __FUNCTION__);*/ is_interesting = 0;}

    void              update();

    virtual caStatus  read  (const casCtx &ctx, gdd &prototype);
    virtual caStatus  write (const casCtx &ctx, const gdd &value);

    virtual const char * getName () const;
};

class fe_epics_cas_PVrec_t {
 public:
    fe_epics_PV PV;

    fe_epics_cas_PVrec_t(int my_mid, cxdtype_t my_dtype, int my_max_nelems, int my_is_rw): PV(my_mid, my_dtype, my_max_nelems, my_is_rw) {}
};

fe_epics_Server::fe_epics_Server() : caServer()
{
    update_mask = valueEventMask();
//fprintf(stderr, "%s(): update_mask=%d\n", __FUNCTION__, *((int*)(&update_mask)));
}

pvExistReturn        fe_epics_Server::pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName)
{
  struct sockaddr_in  addr;
  uint32              ip_val;
  CxsdAccessPerms     perms;

//fprintf(stderr, "%s(\"%s\")\n", __FUNCTION__, pPVAliasName);

    if (clientAddress.isInet())
    {
        addr   = clientAddress.getSockIP();
        ip_val = ntohl(inet_addr(inet_ntoa(addr.sin_addr)));
    }
    else
        ip_val = 0; // The same policy as in cxsd_fe_cx.c::AcceptCXv4Connection()

    perms  = CxsdAccessCheck(NULL, ip_val);
    /* Is it allowed to connect? */
    if ((perms & CXSD_ACCESS_PERM_CONNECT) == 0)
        return pverDoesNotExistHere;

    return cxsd_sfi_pvExistTest(pPVAliasName)? pverExistsHere : pverDoesNotExistHere;
}

pvAttachReturn       fe_epics_Server::pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName)
{
  int                   r;
  int                   mid;
  fe_epics_cas_PVrec_t *PV_ptr;

  cxdtype_t             dtype;
  int                   max_nelems;
  int                   is_rw;

    r = cxsd_sfi_pvAttach(pPVAliasName, &mid);
    if      (r == CXSD_SFI_noMemory) return S_casApp_noMemory;
    else if (r < 0)                       return S_casApp_pvNotFound;
    else if (r == CXSD_SFI_ALREADY)
    {
        cxsd_sfi_get_PV_ptr(mid, (void **)(&PV_ptr)); // Note: for some reason the "static_cast<void **>(&PV_ptr)" generates an "error: invalid static_cast from type 'fe_epics_meat_PVrec_t**' to type 'void**'"
        goto RETURN_SUCCESS;
    }

    cxsd_sfi_get_info(mid, &dtype, &max_nelems, &is_rw);

    PV_ptr = new fe_epics_cas_PVrec_t(mid, dtype, max_nelems, is_rw);
    if (PV_ptr == NULL)
    {
        /*!!! cxsd_sfi_deletePV(mid) */
        return S_casApp_pvNotFound;
    }

    cxsd_sfi_set_PV_ptr(mid, PV_ptr);

 RETURN_SUCCESS:;
//fprintf(stderr, "%p %s\n", &(PV_ptr->PV), __PRETTY_FUNCTION__);
    return *(&(PV_ptr->PV));
}

fe_epics_PV::fe_epics_PV(int my_mid, cxdtype_t my_dtype, int my_max_nelems, int my_is_rw) :
 casPV(),
 upd_gdd(gddAppType_value, aitEnumInvalid/* Deliberately so to prevent unneeded memory allocation */, my_dtype, my_max_nelems)
{
//fprintf(stderr, "%p %s\n", this, __FUNCTION__);
    mid = my_mid;
    chn_dtype  = my_dtype;
    max_nelems = my_max_nelems;
    is_rw      = my_is_rw;

    is_interesting = 0;

//    upd_gdd.setApplType(gddAppType_value);
//    upd_gdd.reference();
}

void fe_epics_PV::destroy () {/* Implicitly DO NOTHING */}

aitEnum fe_epics_PV::bestExternalType() const
{
//fprintf(stderr, "%p %s dtype=%d aitEnum=%d\n", this, __FUNCTION__, chn_dtype, dtype2aitEnum(chn_dtype));
    return dtype2aitEnum(chn_dtype);
}

unsigned fe_epics_PV::maxDimension () const
{
    return max_nelems == 1  ||  chn_dtype == CXDTYPE_TEXT? 0 : 1;
}

aitIndex fe_epics_PV::maxBound ( unsigned dimension ) const
{
    return max_nelems == 1  ||  chn_dtype == CXDTYPE_TEXT? 0 : max_nelems; // CAS_Reference.pdf: "Note that for scalar PVs, maxBound() should return zero no matter what dimension its argument specifies."
}

void      fe_epics_PV::update(void)
{
    if (!is_interesting) return;

    if (mondata2gdd(mid, &(upd_gdd), max_nelems) != S_casApp_success) return;

    postEvent(update_mask, upd_gdd);
}

caStatus  fe_epics_PV::read  (const casCtx &ctx, gdd &prototype)
{
//fprintf(stderr, "%p %s\n", this, __FUNCTION__);

//prototype.dump();
    // A guard against DBR_STSACK_STRING
    if (prototype.isContainer()  ||  prototype.isAtomic()) return S_casApp_noSupport;

    return mondata2gdd(mid, &prototype, max_nelems);
}

caStatus  fe_epics_PV::write (const casCtx &ctx, const gdd &value)
{
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

    if (!is_rw) return S_casApp_noSupport;

    value_type = value.primitiveType();
    dtype      = aitEnum2dtype(value_type);
    if (dtype == CXDTYPE_UNKNOWN) return S_casApp_noSupport;

    if      (value_type == aitEnumString)
    {
        value.getRef(ast_p);
        data   = ast_p->string();
        nelems = ast_p->length();
        return cxsd_sfi_do_write(mid, dtype, nelems, data) == 0? S_casApp_success : S_casApp_noSupport;
    }
    else if (value_type == aitEnumFixedString)
    {
        value.getRef(afs_p);
        data   = afs_p->fixed_string;
        for (nelems = 0;
             nelems < sizeof(afs_p->fixed_string)  &&  afs_p->fixed_string[nelems] != '\0';
             nelems++);
        return cxsd_sfi_do_write(mid, dtype, nelems, data) == 0? S_casApp_success : S_casApp_noSupport;
    }

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

    return cxsd_sfi_do_write(mid, dtype, nelems, data) == 0? S_casApp_success : S_casApp_noSupport;
}

const char * fe_epics_PV::getName () const
{
    return cxsd_sfi_get_name(mid);
}

// -------------------------------------------------------------------
static fe_epics_Server *the_server = NULL;
static int  create_epics_Server(void)
{
//fprintf(stderr, "%s entry\n", __FUNCTION__);
    if (the_server != NULL)
    {
        return -1;
    }
    the_server = new(fe_epics_Server);
//fprintf(stderr, "%s the_server=%p\n", __FUNCTION__, the_server);
    return the_server == NULL? -1 : 0;
}
static void delete_epics_Server(void)
{
    if (the_server != NULL)
    {
        delete(the_server);
        the_server = NULL;
    }
}

static void do_call_fdManager_process()
{
    fileDescriptorManager.process(0);
}

static void do_perform_update(int mid, void *a_PV_ptr)
{
  fe_epics_cas_PVrec_t *PV_ptr = (fe_epics_cas_PVrec_t *)a_PV_ptr;

    PV_ptr->PV.update();
}

//////////////////////////////////////////////////////////////////////

extern "C"
{

int cxsd_fe_epics_cas_init_f (int server_instance, int cxsd_hw_numchans_val)
{
    return create_epics_Server();
}

void cxsd_fe_epics_cas_term_f (void)
{
    delete_epics_Server();
}

void cxsd_fe_epics_cas_do_poll(void)
{
    do_call_fdManager_process();
}

void cxsd_fe_epics_cas_update (int mid, void *PV_ptr)
{
    do_perform_update(mid, PV_ptr);
}


} /* extern "C" */
