#include <sc2api/sc2_api.h>

#include <iostream>

using namespace sc2;

class Bot : public Agent
{
public:
    void OnGameStart() final
    {
        std::cout << "Hello, World!" << std::endl;
    }

    void OnStep() final
    {
        std::cout << Observation()->GetGameLoop() << std::endl;
    }
};

int main(int argc, char *argv[])
{
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    Bot bot;
    coordinator.SetParticipants({
        CreateParticipant(Terran, &bot),
        CreateComputer(Zerg)
    });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(kMapBelShirVestigeLE);

    while (coordinator.Update()) {}

    return 0;
}
