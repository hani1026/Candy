#include "RunAction.hh"
#include <G4Run.hh>
#include <cstring>        // std::memset
#include <sstream>

/* -------------------------------------------------- */
RunAction::RunAction() = default;

/* -------------------------------------------------- */
RunAction::~RunAction()
{
    if (fFile) { fFile->Write(); delete fFile; }
}

/* =================  Geant4 hooks  ================= */
void RunAction::BeginOfRunAction(const G4Run*)
{
    /* ① 첫 Run 에서 ROOT 파일·TTree 초기화 */
    if (!fFile) {
        fFile = new TFile("sipm_hits.root", "RECREATE");
        fTree = new TTree("hits", "SiPM time-bin counts per event");

        /* ② 125 개의 브랜치 (각 256 배열) 예약 */
        for (int i = 0; i < kNSiPM; ++i) {
            std::string bn = "sipm_" + std::to_string(i);
            /* BN[256]/i  -- UInt_t 256-array */
            fTree->Branch(bn.c_str(), fEvtBuf[i], (bn + "[256]/i").c_str());
        }
    }
}

/* -------- Run 끝 → 파일 flush·닫기 ------------------ */
void RunAction::EndOfRunAction(const G4Run*)
{
    std::lock_guard<std::mutex> lock(fIOMutex);
    fFile->Write();      // 여러 Run 이면 누적
}

/* =================  이벤트 버퍼  =================== */
void RunAction::ResetEvent()
{
    std::memset(fEvtBuf, 0, sizeof(fEvtBuf));
}

void RunAction::AddSipmHits(int sipmID, const std::vector<int>& hist)
{
    /* hist.size()==256 보장(SipmHit) */
    for (int b = 0; b < kMaxBins; ++b)
        fEvtBuf[sipmID][b] += hist[b];
}

void RunAction::FillEvent()
{
    std::lock_guard<std::mutex> lock(fIOMutex);   // TTree Fill 보호
    fTree->Fill();
}
