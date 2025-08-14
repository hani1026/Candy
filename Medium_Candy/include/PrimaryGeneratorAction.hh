#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH

#include <G4VUserPrimaryGeneratorAction.hh>
#include <memory>

class G4GeneralParticleSource;
class G4Event;

/**
 *  간단한 “GPS 패스‑스루” Primary Generator
 *  · C++ 측에서는 G4GeneralParticleSource만 생성
 *  · 실제 입사 스펙트럼은 실행 매크로(/gps …)에서 지정
 */
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;

    /** GeneratePrimaries – GPS에게 위임 */
    void GeneratePrimaries(G4Event* anEvent) override;

private:
    G4GeneralParticleSource *fParticleGun;
};

#endif // PRIMARY_GENERATOR_ACTION_HH
