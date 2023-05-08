#ifndef LHVERTEXFINDINGTASK_HH
#define LHVERTEXFINDINGTASK_HH

#include "LKTask.hpp"
#include "LKVertex.hpp"
#include "LKVector3.hpp"

#include "TClonesArray.h"

class LHVertexFindingTask : public LKTask
{ 
  public:
    LHVertexFindingTask();
    virtual ~LHVertexFindingTask() {}

    bool Init();
    void Exec(Option_t*);

    Double_t TestVertexAtK(LKVertex *vertex, LKVector3 testPosition, bool last = false);

    void SetVertexPersistency(bool val);

    void NewTrackWithHitClsuters(LKHit *vertex);

  private:
    TClonesArray* fTrackArray = nullptr;
    TClonesArray* fVertexArray = nullptr;

    TClonesArray *fTrackArray2 = nullptr;
    TClonesArray *fClusterArray = nullptr;
    TClonesArray* fVertexArray2 = nullptr;

    bool fPersistency = true;

    LKVector3::Axis fReferenceAxis = LKVector3::kZ;

  ClassDef(LHVertexFindingTask, 1)
};

#endif
