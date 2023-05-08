#include "LHTpc.hh"
#include "LHPadPlaneRCut.hh"
#include "LHPadPlaneYCut.hh"
#include "LHPadPlaneFan.hh"

ClassImp(LHTpc)

LHTpc::LHTpc()
:LKTpc("LAMPS-TPC","cylinderical time projection chamber")
{
}

bool LHTpc::Init()
{
  //fEFieldAxis = fPar -> GetParAxis("tpcEFieldAxis"); // @todo

  if (BuildGeometry() == false)
    return false;

  if (BuildDetectorPlane() == false)
    return false;

  return true;
}

LKVector3::Axis LHTpc::GetEFieldAxis()
{
  return fEFieldAxis;
}

TVector3 LHTpc::GetEField(TVector3)
{
  LKVector3 e(0,0,0);
  e.AddAt(1,fEFieldAxis);
  return e;
}

LKPadPlane *LHTpc::GetDriftPlane(TVector3 pos)
{
  return (LKPadPlane *) fDetectorPlaneArray -> At(0);
}

bool LHTpc::BuildGeometry()
{
  if (fGeoManager == nullptr) {
    new TGeoManager();
    fGeoManager = gGeoManager;
    fGeoManager -> SetVerboseLevel(0);
    fGeoManager -> SetNameTitle("LAMPS TPC", "LAMPS TPC Geometry");
  }

  auto rMinTPC = fPar -> GetParDouble("LHTpc/rMinTPC");
  auto rMaxTPC = fPar -> GetParDouble("LHTpc/rMaxTPC");
  auto tpcLength = fPar -> GetParDouble("LHTpc/tpcLength");
  auto zOffset = fPar -> GetParDouble("LHTpc/zOffset");

  TGeoMedium *medium = new TGeoMedium("medium", 1, new TGeoMaterial("medium"));

  auto top = CreateGeoTop();

  TGeoVolume *tpc = new TGeoVolumeAssembly("TPC");
  TGeoTranslation *offTPC = new TGeoTranslation("TPC offset",0,0,zOffset);

  TGeoVolume *gas = fGeoManager -> MakeTube("gas",medium,rMinTPC,rMaxTPC,tpcLength/2);
  gas -> SetVisibility(true);
  gas -> SetLineColor(kBlue-10);
  gas -> SetTransparency(90);

  top -> AddNode(tpc, top->GetNdaughters()+1, offTPC);
  tpc -> AddNode(gas, 1);

  FinishGeometry();

  return true;
}

bool LHTpc::BuildDetectorPlane()
{
  LKPadPlane *padplane = nullptr;

  if (fPar->CheckPar("PadPlaneType"))
  {
         if (fPar->GetParString("LHTpc/PadPlaneType") == "LHPadPlaneRCut") padplane = new LHPadPlaneRCut();
         if (fPar->GetParString("LHTpc/PadPlaneType") == "LHPadPlaneYCut") padplane = new LHPadPlaneYCut();
    else if (fPar->GetParString("LHTpc/PadPlaneType") == "LHPadPlaneFan")  padplane = new LHPadPlaneFan();
  }
  else
    padplane = new LHPadPlaneYCut();

  padplane -> AddParameterContainer(fPar);
  padplane -> SetPlaneID(0);
  padplane -> SetPlaneK(fPar -> GetParDouble("LHTpc/tpcPadPlaneK0"));
  padplane -> SetAxis(LKVector3::kX, LKVector3::kY);
  padplane -> Init();

  AddPlane(padplane);

  return true;
}
