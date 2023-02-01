#ifndef __LIBCAS2CX_CONV_H
#define __LIBCAS2CX_CONV_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


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


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __LIBCAS2CX_CONV_H */
