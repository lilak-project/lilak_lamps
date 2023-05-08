#include "LKG4RunManager.hpp"
#include "LKParameterContainer.hpp"
#include "LKPrimaryGeneratorAction.hpp"
#include "LKEventAction.hpp"
#include "LKTrackingAction.hpp"
#include "LKSteppingAction.hpp"

#include "QGSP_BERT.hh"
#include "G4StepLimiterPhysics.hh"
#include "LHDetectorConstruction.hh"

int main(int argc, char** argv)
{
    auto runManager = new LKG4RunManager();

    G4VModularPhysicsList* physicsList = new QGSP_BERT;
    physicsList -> RegisterPhysics(new G4StepLimiterPhysics());
    runManager -> SetUserInitialization(physicsList);
    runManager -> AddParameterContainer(argv[1]);
    runManager -> GetPar() -> Print();
    runManager -> SetUserInitialization(new LHDetectorConstruction());
    runManager -> Initialize();
    runManager -> Run(argc, argv);

    delete runManager;

    return 0;
}
