#include "EventAction.hh"
#include "RunAction.hh"
#include "SipmHit.hh"

#include <G4Event.hh>
#include <G4SDManager.hh>
#include <G4HCofThisEvent.hh>
#include <G4RunManager.hh>

/* ---------- 이벤트 시작 ---------- */
void EventAction::BeginOfEventAction(const G4Event*)
{
    auto* runAct = const_cast<RunAction*>(
        static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));
    runAct->ResetEvent();          // 125×256 0 세팅
}

/* ---------- 이벤트 끝 ---------- */
void EventAction::EndOfEventAction(const G4Event* evt)
{
    auto hcEvt = evt->GetHCofThisEvent();
    if (!hcEvt) return;

    static int hcID = -1;
    if (hcID < 0)
        hcID = G4SDManager::GetSDMpointer()->GetCollectionID("SiPMHits");
    if (hcID < 0) return;

    auto* hits = static_cast<SipmHitsCollection*>(hcEvt->GetHC(hcID));
    if (!hits || hits->entries()==0) return;

    auto* runAct = const_cast<RunAction*>(
        static_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction()));

    /* ① SiPM 별 256-bin 누적 */
    for (size_t i=0; i<hits->entries(); ++i) {
        const SipmHit* h = (*hits)[i];
        runAct->AddSipmHits(h->GetID(), h->GetHist());
    }

    /* ② 한 ‘행’ 완료 → TTree Fill */
    runAct->FillEvent();
}
