#include "LHDriftElectronTask.hh"

#include "LKRun.hpp"
#include "LKMCStep.hpp"

#include "TCanvas.h"
#include "TRandom.h"
#include "TMath.h"
#include "TObjString.h"

#include <iostream>
using namespace std;

ClassImp(LHDriftElectronTask)

LHDriftElectronTask::LHDriftElectronTask()
:LKTask("LADriftElectronTask","")
{
}

bool LHDriftElectronTask::Init()
{
    LKRun *run = LKRun::GetRun();

    fMCStepArray = (TClonesArray *) run -> GetBranch("MCStep0");
    fTpc = (LHTpc *) run -> GetDetectorSystem() -> GetTpc();

    fNPlanes = fTpc -> GetNumPlanes();

    LKParameterContainer *par = run -> GetParameterContainer();
    fDriftVelocity = par -> GetParDouble("LHDriftElectronTask/gasDriftVelocity");
    fCoefLD = par -> GetParDouble("LHDriftElectronTask/gasCoefLongDiff");
    fCoefTD = par -> GetParDouble("LHDriftElectronTask/gasCoefTranDiff");
    fEIonize = par -> GetParDouble("LHDriftElectronTask/gasEIonize");
    fNElInCluster = par -> GetParInt("LHDriftElectronTask/nElInCluster");

    TString gemDataFile;
    gemDataFile = par -> GetParString("LHDriftElectronTask/tpcGEMDataFile");
    TFile *gemFile = new TFile(gemDataFile, "read");
    fGainFunction = (TF1*) gemFile -> Get("gainFit");
    fGainZeroRatio = (((TObjString *) ((TList *) gemFile -> GetListOfKeys()) -> At(2)) -> GetString()).Atof();
    fDiffusionFunction = (TH2D*) ((TCanvas*) gemFile -> Get("diffusion")) -> FindObject("distHist");

    fNTbs = par -> GetParInt("LHDriftElectronTask/numTbs");
    fTbTime = par -> GetParDouble("LHDriftElectronTask/tbTime");

    if (par -> CheckPar("selectMCTrack"))
        fSelectAndDebugTrackID = par -> GetParInt("LHDriftElectronTask/selectMCTrack");

    if (par -> CheckPar("PadPersistency"))
        fPersistencyPad = par -> GetParBool("LHDriftElectronTask/PadPersistency");

    fPadArray = new TClonesArray("LKPad");
    run -> RegisterBranch("Pad", fPadArray, fPersistencyPad);

    return true;
}

void LHDriftElectronTask::Exec(Option_t*)
{
    fPadArray -> Clear("C");

    for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++) {
        fTpc -> GetPadPlane(iPlane) -> Clear();
    }

    Long64_t nMCSteps = fMCStepArray -> GetEntries();
    for (Long64_t iStep = 0; iStep < nMCSteps; ++iStep) {
        LKMCStep* step = (LKMCStep*) fMCStepArray -> At(iStep);
        if (fSelectAndDebugTrackID != -1 && fSelectAndDebugTrackID != step -> GetTrackID())
            continue;

        Int_t trackID = step -> GetTrackID();
        Double_t edep = step -> GetEdep();
        LKVector3 posMC(step -> GetX(),step -> GetY(),step -> GetZ());
        posMC.SetReferenceAxis(fTpc -> GetEFieldAxis());

        auto plane = fTpc -> GetDriftPlane(posMC.GetXYZ());
        if (plane == nullptr)
            continue;

        Int_t planeID = plane -> GetPlaneID();
        Double_t kPlane = plane -> GetPlaneK();

        Double_t lDrift = std::abs(kPlane - posMC.K());
        Double_t tDrift = lDrift/fDriftVelocity;
        Double_t sigmaLD = fCoefLD * sqrt(lDrift);
        Double_t sigmaTD = fCoefTD * sqrt(lDrift);

        Int_t nElectrons = Int_t(edep/fEIonize);

        for (Int_t iElectron = 0; iElectron < nElectrons; iElectron++) {
            Double_t dr    = gRandom -> Gaus(0, sigmaTD);
            Double_t angle = gRandom -> Uniform(2*TMath::Pi());

            Double_t di = dr*TMath::Cos(angle);
            Double_t dj = dr*TMath::Sin(angle);
            Double_t dt = gRandom -> Gaus(0,sigmaLD)/fDriftVelocity;

            Double_t tDriftTotal = tDrift + std::abs(dt);
            Int_t tb = (Int_t)(tDriftTotal/fTbTime);

            if (tb > fNTbs)
                continue;

            Int_t gain = fGainFunction -> GetRandom() * (1 - fGainZeroRatio);
            if (gain <= 0)
                continue;

            Int_t nElClusters = gain/fNElInCluster;
            Int_t gainRemainder = gain%fNElInCluster;

            Double_t iDiffGEM, jDiffGEM;
            for (Int_t iElCluster = 0; iElCluster < nElClusters; iElCluster++) {
                fDiffusionFunction -> GetRandom2(iDiffGEM, jDiffGEM);
                fTpc -> GetPadPlane(planeID) -> FillBufferIn(posMC.I()+di+iDiffGEM*10, posMC.J()+dj+jDiffGEM*10, tb, fNElInCluster, trackID);
            }
            fDiffusionFunction -> GetRandom2(iDiffGEM, jDiffGEM);
            fTpc -> GetPadPlane(planeID) -> FillBufferIn(posMC.I()+di+iDiffGEM*10, posMC.J()+dj+jDiffGEM*10, tb, gainRemainder, trackID);
        }
    }

    Int_t idx = 0;
    Int_t idxLast = 0;
    for (Int_t iPlane = 0; iPlane < fNPlanes; iPlane++) {
        LKPad *pad;
        TIter itChannel(fTpc -> GetPadPlane(iPlane) -> GetChannelArray());
        while ((pad = (LKPad *) itChannel.Next())) {
            if (pad -> IsActive() == false)
                continue;

            LKPad *padSave = (LKPad *) fPadArray -> ConstructedAt(idx);
            padSave -> SetPad(pad);
            padSave -> CopyPadData(pad);
            idx++;
        }

        if (fSelectAndDebugTrackID != -1)
            lk_info << "From selected MCTrack:" << fSelectAndDebugTrackID << ", Number of fired pads in plane-" << iPlane << ": " << idx - idxLast << endl;
        else
            lk_info << "Number of fired pads in plane-" << iPlane << ": " << idx - idxLast << endl;
        idxLast = idx;
    }

    return;
}

void LHDriftElectronTask::SetPadPersistency(bool persistence) { fPersistencyPad = persistence; }
