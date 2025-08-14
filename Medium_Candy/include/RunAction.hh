#ifndef RUN_ACTION_HH
#define RUN_ACTION_HH

#include <G4UserRunAction.hh>
#include <vector>
#include <mutex>
#include <TFile.h>      // NEW
#include <TTree.h>      // NEW

class RunAction : public G4UserRunAction
{
public:
    RunAction();
    ~RunAction() override;

    /* 이벤트 단위 버퍼 조작 -------------------------------- */
    static constexpr int kNSiPM   = 125;
    static constexpr int kMaxBins = 256;      // 4096 ns / 16 ns
    void ResetEvent();                        // 이벤트 시작 — 0으로
    void AddSipmHits(int sipmID, const std::vector<int>& hist);   // 건너온 bin 누적
    void FillEvent();                         // 이벤트 끝 — TTree Fill

    /* Geant4 hooks ---------------------------------------- */
    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction  (const G4Run*) override;

private:
    /* ROOT I/O */
    TFile* fFile  {nullptr};
    TTree* fTree  {nullptr};
    UInt_t fEvtBuf[kNSiPM][kMaxBins] {{0}};   // 이벤트-버퍼

    std::mutex fIOMutex;                      // 쓰레드 보호(Fill, Write)
};

#endif  // RUN_ACTION_HH
