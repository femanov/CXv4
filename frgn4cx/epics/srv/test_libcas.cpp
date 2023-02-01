#include <string.h>

#include <casdef.h>
#include <fdManager.h>
#include <gddApps.h> // For gddAppType_value


#define THE_NAME "the_PV"


//////////////////////////////////////////////////////////////////////

class test_Server : public caServer
{
 public:
    test_Server() {}

    pvExistReturn  pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName);
    pvAttachReturn pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName);
};

class test_PV     : public casPV
{
 public:
    test_PV()                                              {}
    virtual void destroy ()                                {/* Implicitly DO NOTHING */}
    virtual aitEnum bestExternalType () const              {return aitEnumString;}
    virtual unsigned maxDimension () const                 {return 0;}
    virtual aitIndex maxBound ( unsigned dimension ) const {return 0;}

    virtual caStatus  read  (const casCtx &ctx, gdd &prototype);

    virtual const char * getName () const                  {return THE_NAME;}
};

//////////////////////////////////////////////////////////////////////

test_Server the_Server;
test_PV     the_PV;
const char  string_buf[100] = "A string value";
aitString   the_String = "A string value";

//////////////////////////////////////////////////////////////////////

pvExistReturn  test_Server::pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName)
{
    return (strcmp(pPVAliasName, THE_NAME) == 0)? pverExistsHere : pverDoesNotExistHere;
}

pvAttachReturn test_Server::pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName)
{
    return the_PV;
}

caStatus  test_PV::read  (const casCtx &ctx, gdd &prototype)
{
fprintf(stderr, "dump BEFORE\n");
prototype.dump();
#if 0
  prototype = string_buf;
#elif 0
  aitString      *aisp;  // AItString Pointer

    /* Note: we MUST set the type first and only afterwards
             we may change string's content.
             That's because setPrimType() does some cleanup and
             than calls init() on a newly "born" string */
    prototype.setPrimType(aitEnumString);
    aisp = (aitString*)(prototype.dataAddress());
    aisp->installConstBuf(string_buf, strlen(string_buf), sizeof(string_buf));
#else
    prototype = the_String;
#endif
fprintf(stderr, "dump AFTER\n");
    prototype.dump();
    return S_casApp_success;
}

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    while (1) fileDescriptorManager.process(1);

    return 0;
}
