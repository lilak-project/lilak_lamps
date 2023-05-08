#ifndef LHDRIFTElECTRONTASK_HH
#define LHDRIFTElECTRONTASK_HH

#include "LKTask.hpp"
#include "LHTpc.hh"

#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"

class LHDriftElectronTask : public LKTask
{ 
  public:
    LHDriftElectronTask();
    virtual ~LHDriftElectronTask() {}

    bool Init();
    void Exec(Option_t*);

    void SetPadPersistency(bool persistence);

  private:
    TClonesArray* fMCStepArray;
    TClonesArray* fPadArray;
    bool fPersistencyPad = true;

    LHTpc *fTpc = nullptr;
    LKPadPlane *fPadPlane = nullptr;
    Int_t fNPlanes = 0;

    Double_t fDriftVelocity = 0;
    Double_t fCoefLD = 0;
    Double_t fCoefTD = 0;
    Double_t fEIonize = 0;

    Int_t fNElInCluster = 0;

    TH2D *fDiffusionFunction = nullptr;
    TF1 *fGainFunction = nullptr;
    Double_t fGainZeroRatio = 0;

    Int_t fNTbs;
    Double_t fTbTime;

    /// [debugging tool] if fSelectAndDebugTrackID is set, only the selected track will be digitized through the task
    Double_t fSelectAndDebugTrackID = -1;

  ClassDef(LHDriftElectronTask, 1)
};

#endif
