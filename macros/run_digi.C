#include "LKLogger.hpp"

void run_digi(TString name = "iqmd_test")
{
    lk_logger("data/log");

    auto run = LKRun::GetRun();
    run -> SetDataPath("data");
    run -> SetInputFile("data/qmd_test.mc.root");
    run -> SetTag("digi");
    run -> AddDetector(new LHTpc());

    run -> Add(new LHDriftElectronTask());
    //run -> Add(new LHElectronicsTask());
    run -> Init();
    run -> Print();
    run -> Run();
}
