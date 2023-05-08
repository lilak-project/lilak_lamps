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
    LHElectronicsTask();
    virtual ~LHElectronicsTask() {}

    /// If pulse shaping should be done for each mc points use SetUsePointChargeMC(true)
    /// By default, this values is false, and pulse shaping is done in group of charge in one time bucket
    void SetUsePointChargeMC(bool usePointChargeMC) { fUsePointChargeMC = usePointChargeMC; }

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
