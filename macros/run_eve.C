void run_eve()
{
    auto run = LKRun::GetRun();
    run -> SetDataPath("data");
    run -> SetInputFile("data/qmd_test.mc.root");
    run -> SetTag("read");
    run -> AddDetector(new LHTpc());
    run -> Add(new LKEveTask());
    run -> Init();
    run -> Print();
    run -> RunEvent(0);
}
