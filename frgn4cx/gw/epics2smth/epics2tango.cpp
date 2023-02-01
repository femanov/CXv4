#include <stdio.h>
#include <signal.h>

#include <tango.h>

#include "epics2smth.h"


static e2s_backend_t  epics2tango_backend =
{
    names_case_sensitive: 0,
    check_name:           NULL,
    add_chan:             NULL,
};

int main(int argc, char *argv[])
{
#if 1
    {
      Tango::DeviceAttribute abcd;
      int32    isrc32 = 1234;
      int32    idst32;
      float64  fsrc64 = 234.567;
      float64  fdst64;
      int      ri32, rf64;

        fprintf(stderr, "sizeof(abcd)=%zd\n", sizeof(abcd));
        abcd << isrc32;
        abcd << fsrc64;
        ri32 = (abcd >> idst32);
        rf64 = (abcd >> fdst64);
        fprintf(stderr, "idst32=%d fdst64=%f ri32=%d rf64=%d\n", idst32, fdst64, ri32, rf64);
        raise(11);
    }
#endif
    e2s_run(&epics2tango_backend);

    return 0;
}
