#ifndef __CDA_ADAPTER_H
#define __CDA_ADAPTER_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


void cda_adapter_init(const char *defpfx, int nbytes, const char *channames[]);
void cda_adapter_send_one(int n, int nbytes, void *buf);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __CDA_ADAPTER_H */
