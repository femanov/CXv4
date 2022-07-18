#include <casdef.h>
#include <fdManager.h>

#include "cxsd_fe_epics.h"
#include "cxsd_fe_epics_meat.h"


//////////////////////////////////////////////////////////////////////

static casEventMask update_mask;

//////////////////////////////////////////////////////////////////////

static aitEnum dtype2aitEnum(cxdtype_t dtype)
{
    switch (dtype)
    {
        case CXDTYPE_INT32:  return aitEnumInt32;
        case CXDTYPE_UINT32: return aitEnumUint32;
        case CXDTYPE_INT16:  return aitEnumInt16;
        case CXDTYPE_UINT16: return aitEnumUint16;
        case CXDTYPE_DOUBLE: return aitEnumFloat64;
        case CXDTYPE_SINGLE: return aitEnumFloat32;
        //case CXDTYPE_INT64:  return aitEnumInt64;
        //case CXDTYPE_UINT64: return aitEnumUint64;
        case CXDTYPE_INT8:   return aitEnumInt8;
        case CXDTYPE_UINT8:  return aitEnumUint8;
        case CXDTYPE_TEXT:   return aitEnumString;
        default:             return aitEnumInvalid;
    }
}

static cxdtype_t aitEnum2dtype(aitEnum value_type)
{
    switch (value_type)
    {
        case aitEnumInt32:   return CXDTYPE_INT32;
        case aitEnumUint32:  return CXDTYPE_UINT32;
        case aitEnumInt16:   return CXDTYPE_INT16;
        case aitEnumUint16:  return CXDTYPE_UINT16;
        case aitEnumFloat64: return CXDTYPE_DOUBLE;
        case aitEnumFloat32: return CXDTYPE_SINGLE;
        case aitEnumInt8:    return CXDTYPE_INT8;
        case aitEnumUint8:   return CXDTYPE_UINT8;
        case aitEnumString:  return CXDTYPE_TEXT;
        default:             return CXDTYPE_UNKNOWN;
    }
}

static caStatus mondata2gdd(int mid, gdd *gp, int max_nelems)
{
  cxdtype_t       dtype;
  int             nelems;
  void           *data;
  rflags_t        rflags;
  cx_time_t       timestamp;

  aitEnum         value_type;

  struct timespec tspc;

    if (cxsd_fe_epics_get_data(mid, &dtype, &nelems, &data, &rflags, &timestamp) != 0)
        return S_casApp_noSupport;

    value_type = dtype2aitEnum(dtype);
    if      (value_type == aitEnumInvalid) return S_casApp_noSupport;
    // Strings are handled separately
    else if (value_type == aitEnumString)
    {
        return S_casApp_noSupport;
    }
    // Vector?
    else if (max_nelems != 1)
        gp->adjust(NULL, data, value_type);
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
};

class fe_epics_PV     : public casPV
{
 private:
    int        mid;
    cxdtype_t  chn_dtype;
    int        max_nelems;
    int        is_rw;

 public:
//    gddScalar        upd_gdd;
    fe_epics_gdd upd_gdd;

 public:
    fe_epics_PV(int my_mid, cxdtype_t my_dtype, int my_max_nelems, int my_is_rw);
    virtual void destroy ();
    virtual aitEnum bestExternalType () const;
    virtual unsigned maxDimension () const;
    virtual aitIndex maxBound ( unsigned dimension ) const;

//    virtual caStatus interestRegister () {fprintf(stderr, "%p %s\n", this, __FUNCTION__); return S_casApp_success;}

    void              update();

    virtual caStatus  read  (const casCtx &ctx, gdd &prototype);
    virtual caStatus  write (const casCtx &ctx, const gdd &value);

    virtual const char * getName () const;
};

class fe_epics_meat_PVrec_t {
 public:
    fe_epics_PV PV;

    fe_epics_meat_PVrec_t(int my_mid, cxdtype_t my_dtype, int my_max_nelems, int my_is_rw): PV(my_mid, my_dtype, my_max_nelems, my_is_rw) {}
};

fe_epics_Server::fe_epics_Server() : caServer()
{
    update_mask = valueEventMask();
fprintf(stderr, "%s(): update_mask=%d\n", __FUNCTION__, *((int*)(&update_mask)));
}

pvExistReturn        fe_epics_Server::pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName)
{
fprintf(stderr, "%s(\"%s\")\n", __FUNCTION__, pPVAliasName);

    return cxsd_fe_epics_pvExistTest(pPVAliasName)? pverExistsHere : pverDoesNotExistHere;
}

pvAttachReturn       fe_epics_Server::pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName)
{
  int                    r;
  int                    mid;
  fe_epics_meat_PVrec_t *PV_ptr;

  cxdtype_t              dtype;
  int                    max_nelems;
  int                    is_rw;

    r = cxsd_fe_epics_pvAttach(pPVAliasName, &mid);
    if      (r == CXSD_FE_EPICS_noMemory) return S_casApp_noMemory;
    else if (r < 0)                       return S_casApp_pvNotFound;
    else if (r == CXSD_FE_EPICS_ALREADY)
    {
        cxsd_fe_epics_get_PV_ptr(mid, (void **)(&PV_ptr)); // Note: for some reason the "static_cast<void **>(&PV_ptr)" generates an "error: invalid static_cast from type 'fe_epics_meat_PVrec_t**' to type 'void**'"
        goto RETURN_SUCCESS;
    }

    cxsd_fe_epics_get_info(mid, &dtype, &max_nelems, &is_rw);

    PV_ptr = new fe_epics_meat_PVrec_t(mid, dtype, max_nelems, is_rw);
    if (PV_ptr == NULL)
    {
        /*!!! cxsd_fe_epics_deletePV(mid) */
        return S_casApp_pvNotFound;
    }

    cxsd_fe_epics_set_PV_ptr(mid, PV_ptr);

 RETURN_SUCCESS:;
//    return S_casApp_pvNotFound;
fprintf(stderr, "%p %s\n", &(PV_ptr->PV), __PRETTY_FUNCTION__);
    return *(&(PV_ptr->PV));
}

fe_epics_PV::fe_epics_PV(int my_mid, cxdtype_t my_dtype, int my_max_nelems, int my_is_rw) : casPV()
{
fprintf(stderr, "%p %s\n", this, __FUNCTION__);
    mid = my_mid;
    chn_dtype  = my_dtype;
    max_nelems = my_max_nelems;
    is_rw      = my_is_rw;

//    upd_gdd.reference();
}

void fe_epics_PV::destroy () {/* Implicitly DO NOTHING */}

aitEnum fe_epics_PV::bestExternalType() const
{
fprintf(stderr, "%p %s dtype=%d aitEnum=%d\n", this, __FUNCTION__, chn_dtype, dtype2aitEnum(chn_dtype));
    return dtype2aitEnum(chn_dtype);
}

unsigned fe_epics_PV::maxDimension () const
{
    return max_nelems == 1? 0 : 1;
}

aitIndex fe_epics_PV::maxBound ( unsigned dimension ) const
{
    return max_nelems == 1? 0 : max_nelems; // CAS_Reference.pdf: "Note that for scalar PVs, maxBound() should return zero no matter what dimension its argument specifies."
}

void      fe_epics_PV::update(void)
{
#if 1
    if (mondata2gdd(mid, &(upd_gdd), max_nelems) != S_casApp_success) return;

    postEvent(update_mask, upd_gdd);
#else
  cxdtype_t       dtype;
  int             nelems;
  void           *data;
  rflags_t        rflags;
  cx_time_t       timestamp;

  aitEnum         value_type;

  gdd                   *gp     = &(upd_gdd);
  struct timespec        tspc;

    if (cxsd_fe_epics_get_data(mid, &dtype, &nelems, &data, &rflags, &timestamp) != 0) return;

#if 1
    value_type = dtype2aitEnum(dtype);
    if (value_type == aitEnumInvalid) return;
    gp->adjust(NULL, data, value_type);
#else
    switch (dtype)
    {
        case CXDTYPE_INT32:  *gp = *((  int32*)data);     break;
        case CXDTYPE_UINT32: *gp = *(( uint32*)data);     break;
        case CXDTYPE_INT16:  *gp = *((  int16*)data);     break;
        case CXDTYPE_UINT16: *gp = *(( uint16*)data);     break;
        case CXDTYPE_DOUBLE: *gp = *((float64*)data);     break;
        case CXDTYPE_SINGLE: *gp = *((float32*)data);     break;
        //case CXDTYPE_INT64:  *gp = *((  int64*)data);     break;
        //case CXDTYPE_UINT64: *gp = *(( uint64*)data);     break;
        case CXDTYPE_INT8:   *gp = *((  int8 *)data);     break;
        case CXDTYPE_UINT8:  *gp = *(( uint8 *)data);     break;
        default:             return;
    }
#endif

    tspc.tv_sec  = timestamp.sec;
    tspc.tv_nsec = timestamp.nsec;
    gp->setTimeStamp(&tspc);
    postEvent(update_mask, *gp);
#endif
}

caStatus  fe_epics_PV::read  (const casCtx &ctx, gdd &prototype)
{
fprintf(stderr, "%p %s\n", this, __FUNCTION__);

#if 1
    return mondata2gdd(mid, &prototype, max_nelems);
#elif 1
  cxdtype_t       dtype;
  int             nelems;
  void           *data;
  rflags_t        rflags;
  cx_time_t       timestamp;

  aitEnum         value_type;

  struct timespec tspc;

    if (cxsd_fe_epics_get_data(mid, &dtype, &nelems, &data, &rflags, &timestamp) != 0)
        return S_casApp_noSupport;

fprintf(stderr, "\tprimitiveType():%d\n", prototype.primitiveType());
#if 1
    value_type = dtype2aitEnum(dtype);
    if      (value_type == aitEnumInvalid) return S_casApp_noSupport;
    else if (value_type == aitEnumString)
    {
        return S_casApp_noSupport;
    }
    if (max_nelems != 1)
        prototype.adjust(NULL, data, value_type);
    else
    {
        switch (dtype)
        {
            case CXDTYPE_INT32:  prototype = *((  int32*)data);     break;
            case CXDTYPE_UINT32: prototype = *(( uint32*)data);     break;
            case CXDTYPE_INT16:  prototype = *((  int16*)data);     break;
            case CXDTYPE_UINT16: prototype = *(( uint16*)data);     break;
            case CXDTYPE_DOUBLE: prototype = *((float64*)data);     break;
            case CXDTYPE_SINGLE: prototype = *((float32*)data);     break;
            //case CXDTYPE_INT64:  prototype = *((  int64*)data);     break;
            //case CXDTYPE_UINT64: prototype = *(( uint64*)data);     break;
            case CXDTYPE_INT8:   prototype = *((  int8 *)data);     break;
            case CXDTYPE_UINT8:  prototype = *(( uint8 *)data);     break;
            default:             return S_casApp_noSupport;
        }
    }
fprintf(stderr, "\tdata=%p\n", data);
#else
    switch (dtype)
    {
        case CXDTYPE_INT32:  prototype = *((  int32*)data);     break;
        case CXDTYPE_UINT32: prototype = *(( uint32*)data);     break;
        case CXDTYPE_INT16:  prototype = *((  int16*)data);     break;
        case CXDTYPE_UINT16: prototype = *(( uint16*)data);     break;
        case CXDTYPE_DOUBLE: prototype = *((float64*)data);     break;
        case CXDTYPE_SINGLE: prototype = *((float32*)data);     break;
        //case CXDTYPE_INT64:  prototype = *((  int64*)data);     break;
        //case CXDTYPE_UINT64: prototype = *(( uint64*)data);     break;
        case CXDTYPE_INT8:   prototype = *((  int8 *)data);     break;
        case CXDTYPE_UINT8:  prototype = *(( uint8 *)data);     break;
        default:             return S_casApp_noSupport;
    }
#endif
fprintf(stderr, "\tprimitiveType()=%d\n", prototype.primitiveType());

    tspc.tv_sec  = timestamp.sec;
    tspc.tv_nsec = timestamp.nsec;
    prototype.setTimeStamp(&tspc);

    return S_casApp_success;
#else
//prototype=123.456;
prototype=789;
fprintf(stderr, "\tprimitiveType()=%d\n", prototype.primitiveType());
    return S_casApp_success;
    return S_casApp_noSupport;
#endif
}

caStatus  fe_epics_PV::write (const casCtx &ctx, const gdd &value)
{
  aitEnum     value_type;
  cxdtype_t   dtype;
  int         nelems;
  CxAnyVal_t  val;
  void       *data;

  aitIndex    b_first, b_count;

fprintf(stderr, "%p %s\n", this, __FUNCTION__);
fprintf(stderr, "\tprimitiveType()=%d\n", value.primitiveType());

    if (!is_rw) return S_casApp_noSupport;

    value_type = value.primitiveType();
#if 1
    dtype = aitEnum2dtype(value_type);
    if (dtype == CXDTYPE_UNKNOWN) return S_casApp_noSupport;

    if (value.dimension() == 0)
        nelems = 1;
    else
    {
        value.getBound(0, b_first, b_count);
        nelems = b_count;
    }
    if      (nelems == 0)
        data = NULL;
#if 1
    else
        data = (void *)(value.dataVoid());
#else
    // !!! Maybe should better use "value.DataVoid()"?
    else if (nelems == 1)
        data = (void *)(value.dataAddress());
    else
        data = (void *)(value.dataPointer());
#endif

    return cxsd_fe_epics_do_write(mid, dtype, nelems, data) == 0? S_casApp_success : S_casApp_noSupport;
#else
    switch (value_type)
    {
        case aitEnumInt32:   val.i32 = value; data = &(val.i32);  dtype = CXDTYPE_INT32;  fprintf(stderr, "\t\t=@i:%d\n", val.i32); break;
        case aitEnumUint32:  val.u32 = value; data = &(val.u32);  dtype = CXDTYPE_UINT32; break;
        case aitEnumInt16:   val.i16 = value; data = &(val.i16);  dtype = CXDTYPE_INT16;  break;
        case aitEnumUint16:  val.u16 = value; data = &(val.u16);  dtype = CXDTYPE_UINT16; break;
        case aitEnumFloat64: val.f64 = value; data = &(val.f64);  dtype = CXDTYPE_DOUBLE; fprintf(stderr, "\t\t=@i:%f\n", val.f64); break;
        case aitEnumFloat32: val.f32 = value; data = &(val.f32);  dtype = CXDTYPE_SINGLE; break;
        case aitEnumInt8:    val.i8  = value; data = &(val.i8);   dtype = CXDTYPE_INT8;   break;
        case aitEnumUint8:   val.u8  = value; data = &(val.u8);   dtype = CXDTYPE_UINT8;  break;
        default:             return S_casApp_noSupport;
    }

    return cxsd_fe_epics_do_write(mid, dtype, 1, data) == 0? S_casApp_success : S_casApp_noSupport;
#endif
}

const char * fe_epics_PV::getName () const
{
    return cxsd_fe_epics_get_name(mid);
}

// -------------------------------------------------------------------
static fe_epics_Server *the_server = NULL;
static int  create_epics_Server(void)
{
fprintf(stderr, "%s entry\n", __FUNCTION__);
    if (the_server != NULL)
    {
        return -1;
    }
    the_server = new(fe_epics_Server);
fprintf(stderr, "%s the_server=%p\n", __FUNCTION__, the_server);
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
  fe_epics_meat_PVrec_t *PV_ptr = (fe_epics_meat_PVrec_t *)a_PV_ptr;

    PV_ptr->PV.update();
}

//////////////////////////////////////////////////////////////////////

extern "C"
{

int cxsd_fe_epics_meat_init_f (int server_instance, int cxsd_hw_numchans_val)
{
    return create_epics_Server();
}

void cxsd_fe_epics_meat_term_f (void)
{
    delete_epics_Server();
}

void cxsd_fe_epics_meat_do_poll(void)
{
    do_call_fdManager_process();
}

void cxsd_fe_epics_meat_update (int mid, void *PV_ptr)
{
    do_perform_update(mid, PV_ptr);
}


} /* extern "C" */
