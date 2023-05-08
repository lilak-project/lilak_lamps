#ifndef LAMPSTPC_HH
#define LAMPSTPC_HH

#include "LKTpc.hpp"

class LHTpc : public LKTpc
{
  public:
    LHTpc();
    virtual ~LHTpc() {};

    virtual bool Init();

    LKVector3::Axis GetEFieldAxis();

    virtual TVector3 GetEField(TVector3 pos);
    virtual LKPadPlane *GetDriftPlane(TVector3 pos);

  protected:
    virtual bool BuildGeometry();
    virtual bool BuildDetectorPlane();

    LKVector3::Axis fEFieldAxis = LKVector3::kY;

  ClassDef(LHTpc, 1)
};

#endif
