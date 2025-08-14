#ifndef EVENT_ACTION_HH
#define EVENT_ACTION_HH

#include <G4UserEventAction.hh>

class EventAction : public G4UserEventAction
{
public:
    EventAction()  = default;
    ~EventAction() = default;

    void BeginOfEventAction(const G4Event*) override;   // NEW
    void EndOfEventAction  (const G4Event*) override;
};

#endif // EVENT_ACTION_HH
