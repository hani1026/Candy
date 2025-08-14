#include "Materials.hh"

#include <G4NistManager.hh>
#include <G4SystemOfUnits.hh>
#include <fstream>
#include <sstream>
#include <stdexcept>

Materials* Materials::fInstance = nullptr;

Materials* Materials::Instance(){
    if (!fInstance)
    {
        fInstance = new Materials;
    }
return fInstance;
}

Materials::Materials(){
    BuildAcrylic();
    BuildESRSurface();
    BuildLS();
    BuildEpoxy();
}

void Materials::BuildAcrylic(){
    G4NistManager* nist = G4NistManager::Instance();
    fAcrylic = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
    
    const G4int n_Acrylic= 2;
    G4double PhotonE_Acrylic[n_Acrylic] = { 2.00*eV, 3.10*eV };   // 620 nm ↔ 400 nm
    G4double RefractiveIndex_Acrylic[n_Acrylic]  = { 1.49, 1.49 };         // PMMA n ≈ 1.49
    G4double AbsorptionLength_Acrylic[n_Acrylic]  = { 4.*m, 4.*m };         // 캐스팅급: 4 m

    auto mpt_Acrylic = new G4MaterialPropertiesTable();
    mpt_Acrylic->AddProperty("RINDEX",    PhotonE_Acrylic, RefractiveIndex_Acrylic, n_Acrylic);
    mpt_Acrylic->AddProperty("ABSLENGTH", PhotonE_Acrylic, AbsorptionLength_Acrylic, n_Acrylic);
    fAcrylic->SetMaterialPropertiesTable(mpt_Acrylic);
}

void Materials::BuildESRSurface(){
    fESR = new G4OpticalSurface("ESR_Surface");
    fESR->SetType(dielectric_metal);
    fESR->SetFinish(polished);
    fESR->SetModel(unified);

    const G4int n_ESR = 2;
    G4double photonE_ESR[n_ESR] = {2.0*eV, 3.5*eV};
    G4double ReflectiveIndex_ESR[n_ESR] = {0.98, 0.98};
    G4double Diffuse_ESR[n_ESR] = {0.02, 0.02};

    auto mpt_ESR = new G4MaterialPropertiesTable();
    mpt_ESR->AddProperty("REFLECTIVITY",photonE_ESR, ReflectiveIndex_ESR, n_ESR);
    mpt_ESR->AddProperty("DIFFUSEFRAC",photonE_ESR, Diffuse_ESR, n_ESR);
    fESR->SetMaterialPropertiesTable(mpt_ESR);
}

void Materials::BuildLS(){
    G4NistManager* nist = G4NistManager::Instance();
    fLS = new G4Material("LAB", 0.86*g/cm3, 2);
    fLS->AddElement(nist->FindOrBuildElement("C"), 18);
    fLS->AddElement(nist->FindOrBuildElement("H"), 30);

    std::vector<G4double> photonE_LS, emission_LS, RefractiveIndex_LS, AbsortionLength_LS;
    LoadSpectrum("data/lab_scint_spectrum.csv", photonE_LS, emission_LS, true);

    const size_t n = photonE_LS.size();
    RefractiveIndex_LS.assign(n, 1.50);
    AbsortionLength_LS.assign(n, 20.*m);

    auto mpt_LS = new G4MaterialPropertiesTable();
    mpt_LS->AddProperty("RINDEX",       photonE_LS.data(), RefractiveIndex_LS.data(), n);
    mpt_LS->AddProperty("ABSLENGTH",    photonE_LS.data(), AbsortionLength_LS.data(), n);
    mpt_LS->AddProperty("FASTCOMPONENT",photonE_LS.data(), emission_LS.data(),   n);



    mpt_LS->AddConstProperty("SCINTILLATIONYIELD", 10000./MeV);
    mpt_LS->AddConstProperty("FASTTIMECONSTANT",   3.5*ns);
    mpt_LS->AddConstProperty("RESOLUTIONSCALE",         1.0); 
    mpt_LS->AddConstProperty("SLOWTIMECONSTANT",   16.*ns);
    mpt_LS->AddConstProperty("YIELDRATIO",          1.0);
    fLS->SetMaterialPropertiesTable(mpt_LS);
}

void Materials::BuildEpoxy(){

    G4NistManager* nist = G4NistManager::Instance();
    fEpoxy = new G4Material("Epoxy", 1.20*g/cm3,2);
    fEpoxy->AddElement(nist->FindOrBuildElement("C"), 5);
    fEpoxy->AddElement(nist->FindOrBuildElement("H"), 8);
    const G4int n_epoxy = 2;
    G4double photonE_epoxy[n_epoxy] = {2.0*eV, 3.5*eV};
    G4double RefractiveIndex_epoxy[n_epoxy] = {1.55, 1.55};
    auto mpt_epoxy = new G4MaterialPropertiesTable();
    mpt_epoxy->AddProperty("RINDEX", photonE_epoxy, RefractiveIndex_epoxy, n_epoxy);
    fEpoxy->SetMaterialPropertiesTable(mpt_epoxy);
}

void Materials::LoadSpectrum(const std::string& path,
                             std::vector<G4double>& energies,
                             std::vector<G4double>& values,
                             G4bool expect_nm) {
  energies.clear(); 
  values.clear();
  std::ifstream fp(path);
  if (!fp) {
    G4cerr << "Materials::LoadSpectrum – file not found: " 
           << path << " (using fallback).\n";
    return;
  }
  std::string line;
  while (std::getline(fp, line)) {
    if (line.empty() || line[0]=='#') continue;
    std::stringstream ss(line);
    double x, y;
    char comma;
    if (ss >> x >> comma >> y) {
      double e = (expect_nm ? (1239.84197 / x) : x) * eV; // eV
      energies.push_back(e);
      values.push_back(y);
    }
  }
  if (energies.empty()) {
    G4cerr << "Materials::LoadSpectrum – empty file: " << path << G4endl;
  }
}