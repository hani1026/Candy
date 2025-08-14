// DetectorConstruction.cc (Geant4 10.7.4)

#include "DetectorConstruction.hh"
#include "Materials.hh"
#include "SipmSD.hh"

#include <G4SDManager.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4SubtractionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4RotationMatrix.hh>
#include <G4ThreeVector.hh>
#include <G4SystemOfUnits.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <string>

namespace {
    constexpr G4double world_size = 3. * m;

    constexpr G4double Acrylic_box_size = 500. * mm;
    constexpr G4double Acrylic_box_thick = 15. * mm;
    constexpr G4double Acrylic_plate_thick = 5. * mm;

    constexpr G4double LS_size = 500 * mm;


}
G4VPhysicalVolume* DetectorConstruction::Construct()
{
    auto mat = Materials::Instance();
    matLS      = mat->GetLS();
    matAcrylic = mat->GetAcrylic();
    matEpoxy   = mat->GetEpoxy();
    //matGlass   = mat->GetGlass();

    BuildWorld();
    BuildAcrylicBox();
    BuildReflectors();
    BuildPlatesAndSiPMs();
    //BuildPMTs();

    return physical_World;
}

void DetectorConstruction::BuildWorld()
{
    auto nist = G4NistManager::Instance();
    auto air  = nist->FindOrBuildMaterial("G4_AIR");

    auto solid_World = new G4Box("solid_world",
                                 0.5 * world_size,
                                 0.5 * world_size,
                                 0.5 * world_size);
    
    logical_World = new G4LogicalVolume(solid_World,
                                        air,
                                        "logical_world");
    
    physical_World = new G4PVPlacement(nullptr, G4ThreeVector(), logical_World, "physical_world", nullptr, false, 0, true);


    auto solid_LS = new G4Box("solid_LS", 0.5 * LS_size, 0.5 * LS_size, 0.5 * LS_size);
    logical_LS = new G4LogicalVolume(solid_LS, matLS, "logical_LS");
    physical_LS = new G4PVPlacement(nullptr, G4ThreeVector(), logical_LS, "physical_ls", logical_World, false, 0, true);
    
}

void DetectorConstruction::BuildAcrylicBox()
{
    auto solid_Acrylic_outer = new G4Box("solid_Acrylic_outer", 0.5 * Acrylic_box_size + Acrylic_box_thick, 0.5 * Acrylic_box_size + Acrylic_box_thick, 0.5 * Acrylic_box_size + Acrylic_box_thick);
    auto solid_Acrylic_inner = new G4Box("solid_Acrylic_inner", 0.5 * Acrylic_box_size, 0.5 * Acrylic_box_size, 0.5 * Acrylic_box_size);
    auto solid_Acrylic_shell = new G4SubtractionSolid("solid_Acrylic_Shell", solid_Acrylic_outer, solid_Acrylic_inner);

    logical_Acrylic = new G4LogicalVolume(solid_Acrylic_shell, matAcrylic, "logical_Acrylic");

    new G4PVPlacement(nullptr, G4ThreeVector(), logical_Acrylic, "physical_Acrylic", logical_World, false, 0, true);

    logical_Acrylic->SetVisAttributes(G4VisAttributes(G4Colour(0.1, 0.7, 1.0, 0.15)));
}

void DetectorConstruction::BuildReflectors()
{
    auto ESR   = Materials::Instance()->GetESRSurface();
    const G4double FILM_T = 0.1*mm;              // 전체 두께
    const G4double hFilm  = 0.5 * FILM_T;        // half‑length
    const G4double hLS    = 0.5 * Acrylic_box_size;          // 250 mm

    /* ── Film solids ── */
    auto sFrontBack = new G4Box("sESR_FrontBack", hFilm, hLS, hLS);
    auto sLeftRight = new G4Box("sESR_LeftRight", hLS, hFilm, hLS);
    auto sTopBot    = new G4Box("sESR_TopBot",    hLS, hLS, hFilm);

    auto lvFront = new G4LogicalVolume(sFrontBack, matAcrylic, "lvESR_Front");
    auto lvSide  = new G4LogicalVolume(sLeftRight, matAcrylic, "lvESR_Side");
    auto lvTop   = new G4LogicalVolume(sTopBot,    matAcrylic, "lvESR_Top");

    /* ── Placements inside LS ── */
    const G4double d = hLS - hFilm;              // 250 − 0.05 = 249.95 mm

    new G4PVPlacement(nullptr, {+d,0,0}, lvFront,"pvESR_Front", logical_LS,false,0,true);
    new G4PVPlacement(nullptr, {-d,0,0}, lvFront,"pvESR_Back",  logical_LS,false,1,true);
    new G4PVPlacement(nullptr, {0,-d,0}, lvSide ,"pvESR_Left",  logical_LS,false,2,true);
    new G4PVPlacement(nullptr, {0,+d,0}, lvSide ,"pvESR_Right", logical_LS,false,3,true);
    new G4PVPlacement(nullptr, {0,0,+d}, lvTop  ,"pvESR_Top",   logical_LS,false,4,true);
    new G4PVPlacement(nullptr, {0,0,-d}, lvTop  ,"pvESR_Bot",   logical_LS,false,5,true);

    /* ── Optical surface (LS ↔ Film) ── */
    new G4LogicalSkinSurface("ESR_Front_skin", lvFront, ESR);   // 앞·뒤
    new G4LogicalSkinSurface("ESR_Side_skin",  lvSide,  ESR);   // 좌·우
    new G4LogicalSkinSurface("ESR_Top_skin",   lvTop,   ESR);   // 상·하
}


void DetectorConstruction::BuildPlatesAndSiPMs()
{
    /* ──────── 기본 치수 ──────── */
    constexpr G4double plateXY          = LS_size;          // 500 mm
    constexpr G4double plateT           = Acrylic_plate_thick;      // 5 mm
    constexpr G4double socketXY_full    = 7.35 * mm;        // 1 SiPM 구멍의 폭
    constexpr G4double socketXY         = 0.5 * socketXY_full;
    constexpr G4double socketDepth_full = 1.45 * mm;        // 구멍 깊이
    constexpr G4double socketDepth      = 0.5 * socketDepth_full;
    constexpr G4double boardT_full      = 1.20 * mm;
    constexpr G4double boardT           = 0.5 * boardT_full;
    constexpr G4double windowT_full     = 0.25 * mm;
    constexpr G4double windowT          = 0.5 * windowT_full;

    /* ──────── Plate → 25 개 소켓 구멍을 한 덩어리 solid 로 ──────── */
    auto solid_Plate = new G4Box("solid_plate",
                                 0.5 * plateXY,
                                 0.5 * plateXY,
                                 0.5 * plateT);

    // 단일 소켓 모양
    auto solid_Socket = new G4Box("solid_socket",
                                  socketXY, socketXY, socketDepth);

    // 25 개 구멍을 차례로 빼서 plateWithSockets 완성
    G4VSolid* plateWithSockets = solid_Plate;
    for (int ix = 0; ix < 5; ++ix) {
        for (int iy = 0; iy < 5; ++iy) {

            const G4double xPos = -0.5 * plateXY + 5.0 * cm + ix * 10.0 * cm;
            const G4double yPos = -0.5 * plateXY + 5.0 * cm + iy * 10.0 * cm;

            const G4ThreeVector trans(xPos, yPos,
                                      /* z: 소켓 바닥이 plate 상면과 맞닿도록 */
                                      0.5 * plateT - socketDepth);

            plateWithSockets =
                new G4SubtractionSolid("solid_plateSockets",
                                       plateWithSockets,
                                       solid_Socket,
                                       nullptr,               // no rotation
                                       trans);
        }
    }

    /* ──────── Plate LV / PV ──────── */
    auto logical_Plate = new G4LogicalVolume(plateWithSockets,
                                             matAcrylic,
                                             "logical_Plate");
    logical_Plate->SetVisAttributes(G4VisAttributes(G4Colour(0.8,0.8,0.8,0.3)));

    /* ──────── Board & Window LV ──────── */
    auto solid_Board  = new G4Box("solid_board",
                                  socketXY, socketXY, boardT);
    auto logical_Board = new G4LogicalVolume(solid_Board, matEpoxy,
                                             "logical_Board");
    logical_Board->SetVisAttributes(G4VisAttributes(G4Colour::Green()));

    auto solid_Window = new G4Box("solid_window",
                                  0.5 * 6 * mm, 0.5 * 6 * mm, windowT);
    logical_Window = new G4LogicalVolume(solid_Window, matEpoxy,
                                              "logical_Window");
    logical_Window->SetVisAttributes(G4VisAttributes(G4Colour(1.0,0.0,1.0,0.6)));

    /* ──────── 5 개의 Plate · Board · Window 배치 ──────── */
    int copyID = 0;
    for (int ip = 0; ip < 5; ++ip) {

        const G4double zCenter = -200.0 * mm + ip * 100.0 * mm; // plate 간격 100 mm

        // Plate
        new G4PVPlacement(nullptr,
                          G4ThreeVector(0, 0, zCenter),
                          logical_Plate,
                          "physical_Plate",
                          logical_LS,
                          false, ip, true);

        // Board & Window – 각 소켓마다
        for (int ix = 0; ix < 5; ++ix) {
            for (int iy = 0; iy < 5; ++iy) {

                const G4double xPos = -0.5 * plateXY + 5.0 * cm + ix * 10.0 * cm;
                const G4double yPos = -0.5 * plateXY + 5.0 * cm + iy * 10.0 * cm;

                const G4double zBase   = zCenter + 0.5 * plateT - socketDepth_full;
                const G4double zBoard  = zBase + boardT;
                const G4double zWindow = zBoard + boardT + windowT;

                new G4PVPlacement(nullptr,
                                  G4ThreeVector(xPos, yPos, zBoard),
                                  logical_Board,
                                  "physical_Board",
                                  logical_LS,
                                  false, copyID, true);

                new G4PVPlacement(nullptr,
                                  G4ThreeVector(xPos, yPos, zWindow),
                                  logical_Window,
                                  "physical_Window",
                                  logical_LS,
                                  false, copyID, true);

                ++copyID;
            }
        }
    }
}

void DetectorConstruction::ConstructSDandField()
{
    auto sdMan = G4SDManager::GetSDMpointer();

    auto sipmSD = new SipmSD("SiPM_SD");          // SD 이름
    sdMan->AddNewDetector(sipmSD);

    logical_Window->SetSensitiveDetector(sipmSD);
}