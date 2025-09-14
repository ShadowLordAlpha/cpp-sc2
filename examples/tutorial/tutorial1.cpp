#include "astra.h"
#include "sc2/starcraft2.h"

#include <iostream>

using namespace astra;

class Bot : public Agent
{
public:
    void onGameStart() final
    {
        std::cout << "Hello, World!" << std::endl;
    }

    void onStep() final
    {
        //std::cout << Observation()->GetGameLoop() << std::endl;
    }
};

int main(int argc, char *argv[])
{
    Coordinator coordinator;
    coordinator.loadSettings(argc, argv);

    std::shared_ptr<Bot> bot = std::make_shared<Bot>();
    coordinator.setParticipants({
        {ParticipantType::Agent, ParticipantRace::Terran, "Bot Test 1", bot},
        {ParticipantType::Computer, ParticipantRace::Terran, "Computer 1", ComputerSetup()},
    });

    coordinator.start<sc2::Starcraft2>();

    return 0;
}
