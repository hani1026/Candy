#ifndef PHYSICS_LIST_HH
#define PHYSICS_LIST_HH

#include <G4VModularPhysicsList.hh>

class PhysicsList : public G4VModularPhysicsList
{
public:
    PhysicsList();
    ~PhysicsList() override = default;

    /** Optical / EM / Radioactive Decay 전부 포함. */
    void SetCuts() override;

private:
    /** OpticalPhysics 세부 파라미터를 한 곳에서 설정. */
    void ConfigureOptical();
};

#endif // PHYSICS_LIST_HH
