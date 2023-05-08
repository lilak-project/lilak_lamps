#ifndef LHHELIXTRACKFINDINGTASK_HH
#define LHHELIXTRACKFINDINGTASK_HH

#include "TClonesArray.h"
#include "TGraphErrors.h"

#include "LKTask.hpp"
#include "LKHelixTrack.hpp"
#include "LKTpcHit.hpp"
#include "LKHitArray.hpp"

#include "LHTpc.hh"
#include "LKPadPlane.hpp"

#include <vector>
using namespace std;

class LHHelixTrackFindingTask : public LKTask
{
  public:
    LHHelixTrackFindingTask() : LKTask("LHHelixTrackFindingTask","LHHelixTrackFindingTask") {}
    virtual ~LHHelixTrackFindingTask() {}

    virtual bool Init();
    virtual void Exec(Option_t*);

    void SetTrackPersistency(bool val) { fPersistency = val; }

    enum StepNo : int {
      kStepInitArray,
      kStepNewTrack,
      kStepRemoveTrack,
      kStepInitTrack,
      kStepInitTrackAddHit,
      kStepContinuum,
      kStepContinuumAddHit,
      kStepExtrapolation,
      kStepExtrapolationAddHit,
      kStepConfirmation,
      kStepFinalizeTrack,
      kStepNextPhase,
      kStepEndEvent,
      kStepEndOfEvent,
    };

    bool ExecStep();

    bool ExecStepUptoTrackNum(Int_t numTracks);

    LKHelixTrack *GetCurrentTrack() const { return fCurrentTrack; }

    void SetHitBranchName(TString name) { fBranchNameHit = name; }
    void SetTrackletBranchName(TString name) { fBranchNameTracklet = name; }

  private:
    int StepInitArray();
    int StepNewTrack();
    int StepRemoveTrack();
    int StepInitTrack();
    int StepInitTrackAddHit();
    int StepContinuum();
    int StepContinuumAddHit();
    int StepExtrapolation();
    int StepExtrapolationAddHit();
    int StepConfirmation();
    int StepFinalizeTrack();
    int StepNextPhase();
    int StepEndEvent();

    void ReturnBadHitsToPadPlane();

    double CorrelateHitWithTrackCandidate(LKHelixTrack *track, LKTpcHit *hit);
    double CorrelateHitWithTrack(LKHelixTrack *track, LKTpcHit *hit, Double_t scale=1);

    int CheckParentTrackID(LKTpcHit *hit);
    bool CheckTrackQuality(LKHelixTrack *track);
    double CheckTrackContinuity(LKHelixTrack *track);
    bool CheckHitDistInAlphaIsLargerThanQuarterPi(LKHelixTrack *track, Double_t dLength);

    bool BuildAndConfirmTrack(LKHelixTrack *track, bool &tailToHead);
    bool AutoBuildByExtrapolation(LKHelixTrack *track, bool &buildHead, Double_t &extrapolationLength);
    bool AutoBuildAtPosition(LKHelixTrack *track, TVector3 p, bool &tailToHead, Double_t &extrapolationLength, Double_t scale=1);

  private:
    LHTpc *fTpc = nullptr;
    LKPadPlane *fPadPlane = nullptr;
    TClonesArray *fHitArray = nullptr;
    TClonesArray *fTrackArray = nullptr;

    TString fBranchNameHit = "Hit";
    TString fBranchNameTracklet = "Tracklet";

    bool fPersistency = true;

    LKHitArray *fTrackHits = nullptr;
    LKHitArray *fCandHits = nullptr;
    LKHitArray *fGoodHits = nullptr;
    LKHitArray *fBadHits  = nullptr;

    Double_t fDefaultScale;
    Double_t fTrackWCutLL;  ///< Track width cut low limit
    Double_t fTrackWCutHL;  ///< Track width cut high limit
    Double_t fTrackHCutLL;  ///< Track height cut low limit
    Double_t fTrackHCutHL;  ///< Track height cut high limit

    LKVector3::Axis fReferenceAxis;

    Int_t fPhaseIndex = 0;

    Int_t fMinHitsToFitInitTrack = 7; ///< try track fit if track has more than this number of hits in track
    Int_t fCutMinNumHitsInitTrack = 10; ///
    Int_t fCutMaxNumHitsInitTrack = 15; ///< return hits if track has more than this number of hits within initialization stage
    Int_t fCutMinNumHitsFinalTrack = 15; ///< remove track if track has smaller than this number of hits within initialization stage
    Double_t fCutMinHelixRadius = 30.; ///< helix radius cut for initialization stage
    Double_t fTrackLengthCutScale = 2.5; ///< track length cut for initialization stage is [this_var] * track -> GetRMSR()
    Double_t fCutdkInExpectedTrackPath = 4.; // the correlation distance cut through helix axis during the track path between two hits

    LKHelixTrack *fCurrentTrack = nullptr;

    Int_t fNextStep = StepNo::kStepInitArray;
    Int_t fNumCandHits;
    Int_t fNumGoodHits;
    Int_t fNumBadHits;

    TCanvas *fCvsCurrentTrack = nullptr;
    TGraphErrors *fGraphCurrentTrackPoint = nullptr;

  ClassDef(LHHelixTrackFindingTask, 1)
};

#endif
