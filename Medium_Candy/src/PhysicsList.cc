#include "PhysicsList.hh"

/* ――― Geant4 standard / reference physics ――― */
#include <G4DecayPhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4EmExtraPhysics.hh>
#include <G4OpticalPhysics.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <G4HadronElasticPhysics.hh>
#include <G4HadronPhysicsFTFP_BERT.hh>
#include <G4StoppingPhysics.hh>
#include <G4IonElasticPhysics.hh>
#include <G4IonPhysics.hh>

#include <G4SystemOfUnits.hh>
#include <G4ProductionCutsTable.hh>

/* -------------------------------------------------------------------------- */
PhysicsList::PhysicsList()
{
    /* ―― Step 1. 기본 EM, 붕괴, 핵반응 ―― */
    //RegisterPhysics(new G4DecayPhysics);
    RegisterPhysics(new G4EmStandardPhysics);      // 전자·감마 EM
    //RegisterPhysics(new G4EmExtraPhysics);         // 뮤온·전하 분산 등

    //RegisterPhysics(new G4HadronElasticPhysics);
    //RegisterPhysics(new G4HadronPhysicsFTFP_BERT);
    //RegisterPhysics(new G4StoppingPhysics);        // μ–, π– stopping 등

    //RegisterPhysics(new G4IonElasticPhysics);
    //RegisterPhysics(new G4IonPhysics);

    //RegisterPhysics(new G4RadioactiveDecayPhysics);/* ★ 환경 방사능 */

    /* ―― Step 2. Optical ―― */
    auto optical = new G4OpticalPhysics;
    RegisterPhysics(optical);
    ConfigureOptical();
}

/* -------------------------------------------------------------------------- */
void PhysicsList::ConfigureOptical()
{
    const auto* optConst =
        dynamic_cast<const G4OpticalPhysics*>(GetPhysics("Optical"));

    if (!optConst) return;

    auto* optical = const_cast<G4OpticalPhysics*>(optConst);
    /* ── 신틸레이션 & 체렌코프 설정 ── */
    optical->SetScintillationByParticleType(false);     // ★ 알파/전자 등 구분
    optical->SetScintillationYieldFactor(1.0);         // 재료 MPT 값 그대로
    optical->SetMaxNumPhotonsPerStep(100);
    optical->SetTrackSecondariesFirst(kCerenkov,     false);
    optical->SetTrackSecondariesFirst(kScintillation,false);

    /* ── 광학적 흡수·산란 step 크기 제한 ── */
    optical->Configure(kAbsorption, true);
    optical->Configure(kRayleigh,   true);
}

/* -------------------------------------------------------------------------- */
void PhysicsList::SetCuts()
{
    /* 전역 컷 값 – LS 두께(500 mm)에 비해 충분히 작게 */
    SetDefaultCutValue(1 * mm);

    G4VUserPhysicsList::SetCuts();                   // 내부 SetCuts 호출

    /* 옵티컬 포톤은 컷 없음 (0 → always generate) */
    G4ProductionCutsTable::GetProductionCutsTable()
        ->SetEnergyRange(250 * eV, 100 * GeV);       // Optical ~ X‑ray까지
}
