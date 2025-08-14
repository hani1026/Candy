#ifndef SIPM_SD_HH
#define SIPM_SD_HH

#include <G4VSensitiveDetector.hh>
#include "SipmHit.hh"

class SipmSD : public G4VSensitiveDetector
{
public:
    SipmSD(const G4String& name,
           const G4String& hitsCollectionName = "SiPMHits");
    ~SipmSD() override = default;

    void Initialize(G4HCofThisEvent* hce)           override;
    G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;
    void EndOfEvent(G4HCofThisEvent*)               override;

private:
    SipmHitsCollection* fHitsCollection{nullptr};
    int                 fHCID{-1};
};

#endif // SIPM_SD_HH
