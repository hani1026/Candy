#include "SipmSD.hh"
#include "SipmHit.hh"

#include <G4Step.hh>
#include <G4SDManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4OpticalPhoton.hh>

/* -------------------------------------------------------------------------- */
SipmSD::SipmSD(const G4String& name, const G4String& hcName)
  : G4VSensitiveDetector(name)
{
    collectionName.insert(hcName);          // 히트 컬렉션 이름 등록
}

/* -------------------------------------------------------------------------- */
void SipmSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection =
        new SipmHitsCollection(SensitiveDetectorName, collectionName[0]);

    if (fHCID < 0)
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);

    hce->AddHitsCollection(fHCID, fHitsCollection);
}

/* -------------------------------------------------------------------------- */
G4bool SipmSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::Definition())
        return false;

    int    copyNo = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber();
    double t_ns   = step->GetPreStepPoint()->GetGlobalTime() / ns;

    /* 기존 hit 탐색 */
    SipmHit* hit = nullptr;
    for (size_t i = 0; i < fHitsCollection->entries(); ++i)
        if ((*fHitsCollection)[i]->GetID() == copyNo) { hit = (*fHitsCollection)[i]; break; }

    /* 없으면 새로 생성 (포인터로 저장!) */
    if (!hit) {
        hit = new SipmHit(copyNo);
        fHitsCollection->insert(hit);
    }
    hit->Fill(t_ns);

    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return true;
}

/* -------------------------------------------------------------------------- */
void SipmSD::EndOfEvent(G4HCofThisEvent*)
{
    /* 현재는 출력·저장은 하지 않음. 필요 시 추가 */
}
