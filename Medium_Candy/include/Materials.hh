#ifndef MATERIALS_HH
#define MATERIALS_HH

#include <G4Material.hh>
#include <G4OpticalSurface.hh>
#include <map>
#include <string>
#include <vector>
#include "globals.hh"

class Materials {
public:
    static Materials* Instance();

    G4Material* GetLS()         const { return fLS; }
    G4Material* GetAcrylic()    const { return fAcrylic; }
    G4Material* GetEpoxy()      const { return fEpoxy; }
    //G4Material* GetGlass()      const { return fGlass; }
    //G4Material* GetGrease()     const { return fGrease; }

    /** ESR specular‑plus‑diffuse surface (inner / outer 동일). */
    G4OpticalSurface* GetESRSurface() const { return fESR; }

    static void LoadSpectrum(const std::string& path,
                             std::vector<G4double>& energies,
                             std::vector<G4double>& values,
                             G4bool expect_nm = true); // if true, first column in nm

private:
    Materials();                // ctor builds everything
    void BuildLS();             // LAB + PPO
    void BuildESRSurface();     // spec 0.98 + diff 0.02
    void BuildAcrylic();
    void BuildEpoxy();


    /* internal */
    G4Material *fLS{}, *fAcrylic{}, *fEpoxy{}; //*fGlass{}, *fGrease{};
    G4OpticalSurface *fESR{};

  static Materials* fInstance;
};

#endif // MATERIALS_HH