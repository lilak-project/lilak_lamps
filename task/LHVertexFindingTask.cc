#include "LHVertexFindingTask.hh"
#include "LKHelixTrack.hpp"

#include "LKLogger.hpp"
#include "LKRun.hpp"
#include "LKHit.hpp"
#include "LKTpcHit.hpp"

#include <iostream>
using namespace std;

ClassImp(LHVertexFindingTask)

LHVertexFindingTask::LHVertexFindingTask()
:LKTask("LHVertexFindingTask","")
{
}

bool LHVertexFindingTask::Init()
{
  LKRun *run = LKRun::GetRun();

  fTrackArray = (TClonesArray *) run -> GetBranch("Tracklet");

  fVertexArray = new TClonesArray("LKVertex");
  run -> RegisterBranch("Vertex", fVertexArray, fPersistency);

  fClusterArray = new TClonesArray("LKTpcHit");
  run -> RegisterBranch("HitCluster", fClusterArray, fPersistency);

  fTrackArray2 = new TClonesArray("LKHelixTrack");
  run -> RegisterBranch("VertexTrack", fTrackArray2, fPersistency);

  TString  axis = run -> GetParameterContainer() -> GetParString("tpcBFieldAxis");
       if (axis == "x") fReferenceAxis = LKVector3::kX;
  else if (axis == "y") fReferenceAxis = LKVector3::kY;
  else if (axis == "z") fReferenceAxis = LKVector3::kZ;

  return true;
}

void LHVertexFindingTask::Exec(Option_t*)
{
  fVertexArray -> Clear("C");

  if (fTrackArray -> GetEntriesFast() < 2) {
    lk_warning << "Less than 2 tracks are given. Unable to find vertex." << endl;
    return;
  }

  LKVertex *vertex = (LKVertex *) fVertexArray -> ConstructedAt(0);
  vertex -> SetTrackID(10000000); //TODO

  Double_t sLeast = 1.e8;
  Double_t kAtSLeast = 0;
  Double_t sTest = 0;

  for (auto k = -10.; k < 10.; k+=0.1) {
    LKVector3 testPosition(fReferenceAxis, 0, 0, k);
    sTest = TestVertexAtK(vertex, testPosition);
    if (sTest < sLeast) {
      sLeast = sTest;
      kAtSLeast = k;
    }
  }

  LKVector3 testPosition(fReferenceAxis, 0, 0, kAtSLeast);
  sTest = TestVertexAtK(vertex, testPosition, true);

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
    LKHelixTrack *track = (LKHelixTrack *) fTrackArray -> At(iTrack);

    if (track -> GetParentID() == 0) {
      track -> Fit();
      track -> DetermineParticleCharge(vertex -> GetPosition());
    }
    else {
      auto trackID = track -> GetTrackID();
      track -> SetTrackID(-1);
      track -> FinalizeHits();
      track -> SetTrackID(trackID);
    }
  }

  auto pos = vertex -> GetPosition();

  //NewTrackWithHitClsuters(vertex);

  lk_info << "Found vertex at " << Form("(%.1f, %.1f, %.1f)",pos.X(),pos.Y(),pos.Z()) << " with " << vertex -> GetNumTracks() << " tracks" << endl;

  return;
}

Double_t LHVertexFindingTask::TestVertexAtK(LKVertex *vertex, LKVector3 testPosition, bool last)
{
  Double_t sTest = 0;
  Int_t numUsedTracks = 0;

  TVector3 averagePosition(0,0,0);

  Int_t numTracks = fTrackArray -> GetEntriesFast();
  for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
    LKHelixTrack *track = (LKHelixTrack *) fTrackArray -> At(iTrack);

    auto xyzOnHelix = track -> ExtrapolateTo(testPosition);
    auto dist = (testPosition.GetXYZ()-xyzOnHelix).Mag();

    if (last && dist > 10)
      continue;

    averagePosition += xyzOnHelix;

    if (numUsedTracks != 0)
      sTest = ((Double_t)numUsedTracks)/(numUsedTracks+1)*sTest + dist/numUsedTracks;

    ++numUsedTracks;

    if (last) {
      track -> SetParentID(0);
      vertex -> AddTrack(track);
    }
  }

  averagePosition = (1./(numUsedTracks))*averagePosition;

  if (last) {
    for (Int_t iTrack = 0; iTrack < numTracks; iTrack++) {
      LKHelixTrack *track = (LKHelixTrack *) fTrackArray -> At(iTrack);
      if (track -> GetParentID() == 0) {
        track -> AddHit(vertex);
      }
    }
  }

  if (last)
    vertex -> SetPosition(averagePosition);

  return sTest;
}

void LHVertexFindingTask::SetVertexPersistency(bool val) { fPersistency = val; }



void LHVertexFindingTask::NewTrackWithHitClsuters(LKHit *vertex) // TODO for curling tracks
{
  lk_debug << endl;
  Int_t numTracks = fTrackArray -> GetEntriesFast();
  lk_debug << endl;
  for (Int_t iTrack = 0; iTrack < numTracks; ++iTrack) {
    LKHelixTrack *track = (LKHelixTrack *) fTrackArray -> At(iTrack);
    LKHelixTrack *track2 = (LKHelixTrack *) fTrackArray2 -> ConstructedAt(iTrack);
    auto hitArray = track -> GetHitArray();
    hitArray -> SortByLayer(1);
    lk_debug << endl;

    LKTpcHit *currentCluster = nullptr;
    Int_t currentLayer = -1;
    Int_t countClusters = 0;
    lk_debug << endl;

    track2 -> AddHit(vertex);
    auto numHits = hitArray -> GetNumHits();
    for (auto iHit=0; iHit<=numHits; ++iHit)
    {
      auto hit = (LKTpcHit *) hitArray -> GetHit(iHit);
      auto layer = hit -> GetLayer();
      lk_debug << endl;

      if (iHit==0)
      {
        lk_debug << endl;
        currentLayer = hit -> GetLayer();
        lk_debug << endl;
        currentCluster = (LKTpcHit *) fClusterArray -> ConstructedAt(countClusters++);
        lk_debug << endl;
        currentCluster -> SetHitID(countClusters);
        lk_debug << endl;
        currentCluster -> SetLayer(layer);
        lk_debug << endl;
        currentCluster -> AddHit(hit);
        lk_debug << endl;
      }
      else if (layer != currentLayer) {
        lk_debug << endl;
        track2 -> AddHit(currentCluster);
        lk_debug << endl;
        currentLayer = layer;
        lk_debug << endl;
        currentCluster = (LKTpcHit *) fClusterArray -> ConstructedAt(countClusters++);
        lk_debug << endl;
        currentCluster -> SetHitID(countClusters);
        lk_debug << endl;
        currentCluster -> SetLayer(layer);
        lk_debug << endl;
        currentCluster -> AddHit(hit);
      }
      else {
        lk_debug << endl;
        currentCluster -> AddHit(hit);
      }
    }
    lk_debug << endl;
    track2 -> AddHit(currentCluster);
    lk_debug << endl;
    track2 -> Fit();
    lk_debug << endl;
    track2 -> FinalizeHits();
    lk_debug << endl;
  }

  return;
}
