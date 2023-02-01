#include <stdio.h>
#include <strings.h>

#include <tango.h>
#include <casdef.h>
#include <fdManager.h>


static casEventMask update_mask;

//////////////////////////////////////////////////////////////////////

class e2t_Server : public caServer
{
 public:
    e2t_Server();

    pvExistReturn        pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName);
    pvAttachReturn       pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName);
};

class e2t_gdd : public gdd
{
 public:
    e2t_gdd() {};
};

class e2t_PV     : public casPV
{
 private:
    e2t_gdd      upd_gdd;
    int          is_interesting;

 public:
    e2t_PV();
    virtual void destroy () {/* Implicitly DO NOTHING */}
    virtual aitEnum bestExternalType () const;
    virtual unsigned maxDimension () const;
    virtual aitIndex maxBound ( unsigned dimension ) const;

    virtual caStatus interestRegister () {/*fprintf(stderr, "%p %s\n", this, __FUNCTION__);*/ is_interesting = 1; return S_casApp_success;}
    virtual void     interestDelete   () {/*fprintf(stderr, "%p %s\n", this, __FUNCTION__);*/ is_interesting = 0;}

    virtual const char * getName () const;
};

//------------

e2t_Server::e2t_Server() : caServer()
{
    update_mask = valueEventMask();
//fprintf(stderr, "%s(): update_mask=%d\n", __FUNCTION__, *((int*)(&update_mask)));
}

pvExistReturn        e2t_Server::pvExistTest(const casCtx &ctx, const caNetAddr & clientAddress, const char *pPVAliasName)
{

fprintf(stderr, "%s(): %s\n", __FUNCTION__, pPVAliasName);

    return pverDoesNotExistHere;
}

pvAttachReturn       e2t_Server::pvAttach   (const casCtx &ctx,                                  const char *pPVAliasName)
{
    return S_casApp_pvNotFound;
}

e2t_PV::e2t_PV() :
 casPV(),
 upd_gdd()
{
    is_interesting = 0;
}

aitEnum e2t_PV::bestExternalType() const
{
  aitEnum                ret;

    return ret;
}

unsigned e2t_PV::maxDimension () const
{
  unsigned               ret;

    return ret;
}

aitIndex e2t_PV::maxBound ( unsigned dimension ) const
{
  aitIndex               ret;

    return ret;
}

const char * e2t_PV::getName () const
{
  const char            *ret;

    return ret;
}

//////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

    return 0;
}
