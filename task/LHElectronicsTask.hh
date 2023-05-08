#ifndef LHELECTRONICSTASK_HH
#define LHELECTRONICSTASK_HH

#include "LKTask.hpp"
#include "LHTpc.hh"
#include "LKPulseGenerator.hpp"

#include "TH2D.h"
#include "TF1.h"
#include "TClonesArray.h"

class LHElectronicsTask : public LKTask
{ 
  public:
    LHElectronicsTask(Bool_t usePointChargeMC = false);
    virtual ~LHElectronicsTask() {}

    bool Init();
    void Exec(Option_t*);

  private:
    TClonesArray* fPadArray;

    Int_t fNPlanes;
    Int_t fNTbs;
    Double_t feVToADC;
    Double_t fDynamicRange;

    TF1 *fPulseFunction;

    Bool_t fUsePointChargeMC = false;

  ClassDef(LHElectronicsTask, 1)
};

#endif
