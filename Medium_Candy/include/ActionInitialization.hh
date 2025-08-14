#ifndef ACTION_INITIALIZATION_HH
#define ACTION_INITIALIZATION_HH

#include <G4VUserActionInitialization.hh>

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization()  = default;
    ~ActionInitialization() override = default;

    /** 마스터 스레드(스코어 수집 전용) */
    void BuildForMaster() const override;

    /** 워커 스레드(이벤트 생성·추적) */
    void Build()          const override;
};

#endif // ACTION_INITIALIZATION_HH
