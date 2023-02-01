#include <stdio.h>
#include <strings.h>

#include <casdef.h>
#include <fdManager.h>

#include "misc_macros.h" // For safe_realloc()
#include "misc_iso8601.h"

#include "epics2smth.h"


enum
{
    MON_TYPE_UNUSED = 0,
    MON_TYPE_SEARCH = 1,
    MON_TYPE_WORKS  = 2,
};

// Note: monitors implementation is based on cxscheduler.c's trec_t
typedef struct
{
    int   is_used; 
    int   next;    // Link to next
    int   prev;    // Link to previous
    void *PV_ptr;  // Pointer to casPV object
    void *AS_ptr;  // Pointer to async object
    void *BE_ptr;  // Pointer to BackEnd's object
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
  moninfo_t *p;
  moninfo_t *new_list;
  int       x;

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
    p = mons_list + mid;
    avl_mon = p->next;
    p->is_used = MON_TYPE_SEARCH;

    return mid;
}

static void      RlsMonSlot(int mid)
{
  moninfo_t *p = mons_list + mid;

    p->is_used = MON_TYPE_UNUSED;
    p->next = avl_mon;
    avl_mon = mid;
}

//////////////////////////////////////////////////////////////////////

static casEventMask update_mask;

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
    e2s_gdd() {};
};

class e2s_PV     : public casPV
{
 private:
    e2s_gdd upd_gdd;
    int          is_interesting;

 public:
    e2s_PV();
    virtual void destroy () {/* Implicitly DO NOTHING */}
//    virtual aitEnum bestExternalType () const;
//    virtual unsigned maxDimension () const;
//    virtual aitIndex maxBound ( unsigned dimension ) const;

    virtual caStatus interestRegister () {/*fprintf(stderr, "%p %s\n", this, __FUNCTION__);*/ is_interesting = 1; return S_casApp_success;}
    virtual void     interestDelete   () {/*fprintf(stderr, "%p %s\n", this, __FUNCTION__);*/ is_interesting = 0;}

//    void              update();

//    virtual caStatus  read  (const casCtx &ctx, gdd &prototype);
//    virtual caStatus  write (const casCtx &ctx, const gdd &value);

//    virtual const char * getName () const;
};

class e2s_AsyncPVExistIO : public casAsyncPVExistIO
{
 public:
    e2s_AsyncPVExistIO(const casCtx & ctx) : casAsyncPVExistIO(ctx)  {}
    virtual void destroy () {/* Implicitly DO NOTHING */}
};

//------------

e2s_Server::e2s_Server() : caServer()
{
    update_mask = valueEventMask();
//fprintf(stderr, "%s(): update_mask=%d\n", __FUNCTION__, *((int*)(&update_mask)));
}

pvExistReturn        e2s_Server::pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName)
{
fprintf(stderr, "%s %s(): %s\n", strcurtime_msc(), __FUNCTION__, pPVAliasName);
    return pverDoesNotExistHere;
}

pvAttachReturn       e2s_Server::pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName)
{
    return S_casApp_pvNotFound;
}

e2s_PV::e2s_PV() :
 casPV(),
 upd_gdd()
{
    is_interesting = 0;
}

//////////////////////////////////////////////////////////////////////

extern "C"
{

void e2s_run(void)
{
  e2s_Server the_server;

    while (1) fileDescriptorManager.process(3600);
}

} /* extern "C" */
