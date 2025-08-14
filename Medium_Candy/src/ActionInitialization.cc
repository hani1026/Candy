#include "ActionInitialization.hh"

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"

#include <G4Threading.hh>

/* -------------------------------------------------------------------------- */
void ActionInitialization::BuildForMaster() const
{
    /* 마스터는 데이터 파일 열고 닫기만 – RunAction만 필요 */
    SetUserAction(new RunAction);
}

/* -------------------------------------------------------------------------- */
void ActionInitialization::Build() const
{
    /* 워커 스레드: 전체 액션 */
    SetUserAction(new PrimaryGeneratorAction);
    auto runAct   = new RunAction;
    SetUserAction(runAct);
    SetUserAction(new EventAction);
}
