#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include <G4VUserDetectorConstruction.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include "globals.hh"

class DetectorConstruction : public G4VUserDetectorConstruction { 
public:
    DetectorConstruction() = default;
    ~DetectorConstruction() override = default;

    // Construct the full geometry and return the physical world
    G4VPhysicalVolume* Construct() override;

    void ConstructSDandField() override; 

    G4LogicalVolume* logical_Window = nullptr;  

private:
    // Helper functions for each geometry component   
    void BuildWorld();
    void BuildAcrylicBox();
    void BuildReflectors();
    void BuildPlatesAndSiPMs();
    //void BuildPMTs();

    G4LogicalVolume* logical_World   = nullptr;
    G4LogicalVolume* logical_LS      = nullptr;
    G4LogicalVolume* logical_Acrylic = nullptr;

    G4VPhysicalVolume* physical_World   = nullptr;
    G4VPhysicalVolume* physical_LS      = nullptr;
    G4VPhysicalVolume* physical_Acrylic = nullptr;

    G4Material* matLS      = nullptr;
    G4Material* matAcrylic = nullptr;
    G4Material* matEpoxy   = nullptr;
    //G4Material* matGlass   = nullptr;
    
    

};

#endif // DETECTORCONSTRUCTION_HH