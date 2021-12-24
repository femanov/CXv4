#include <stdio.h>

#define LINUX // For CAENVMEtypes.h, which uses "#ifdef LINUX" instead of "#ifdef WIN32"
#include "CAENVMElib.h"


int main(int argc, char *argv[])
{
  int32_t               CAENVMElib_handle; // The same type as in CAENVMElib.h
  CVErrorCodes          err;
  CAEN_BYTE             lit_mask;
  int                   vect32;

  CVBoardTypes          BdType;
  short                 Link;
  short                 BdNum;

    BdType = cvV2718;
    Link   = 1;
    BdNum  = 0;

    err = CAENVME_Init(BdType, Link, BdNum, &(CAENVMElib_handle));
    printf("CAENVME_Init=%d CAENVMElib_handle=%d\n", err, CAENVMElib_handle);
    err = CAENVME_IRQCheck(CAENVMElib_handle, &lit_mask);
    printf("CAENVME_IRQCheck()=%d lit_mask=%d\n", err, lit_mask);
    err = CAENVME_IACKCycle(CAENVMElib_handle, cvIRQ7, &vect32, cvD8);
    printf("CAENVME_IACKCycle(cvIRQ7)=%d vect32=%d\n", err, vect32);

    return 0;
}
