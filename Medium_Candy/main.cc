#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"

#include <G4RunManagerFactory.hh>
#include <G4UIExecutive.hh>
#include <G4VisExecutive.hh>
#include <G4UImanager.hh>

int main(int argc,char** argv)
{
    G4VisManager* visManager = nullptr;
    
    visManager = new G4VisExecutive;
    visManager->Initialize();

    auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

    runManager->SetUserInitialization(new DetectorConstruction);
    runManager->SetUserInitialization(new PhysicsList);
    runManager->SetUserInitialization(new ActionInitialization);

    G4UIExecutive* ui = nullptr;

    if (argc == 1) {                 // ── no macro ⇒ interactive
        ui = new G4UIExecutive(argc, argv, "Qt");

    }
    runManager->Initialize();

    if (argc > 1) {                  // ── batch mode
        G4String macro = argv[1];
        G4UImanager::GetUIpointer()->ApplyCommand("/control/execute " + macro);
    } else {                  
        G4UImanager::GetUIpointer()->ApplyCommand("/control/execute init_vis.mac");      // ── UI mode
        ui->SessionStart();
    }

    delete ui;
    delete visManager;
    delete runManager;
    return 0;
}
