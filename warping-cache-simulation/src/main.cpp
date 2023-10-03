#include <iostream>

#include "Settings/SimulationSettingsParser.hpp"
#include "Simulation/Simulation.hpp"

SimulationSettings parseSimulationSettings(int argc, char **argv) {
  SimulationSettingsParser parser(argc, argv);
  return parser.ParseSimulationSettings();
}

void runSimulation(const SimulationSettings &settings) {
  auto context = IslUtil::AllocContext(settings.includes);
  {
    Simulation simulation(context, settings);
    simulation.Run();
  }
  IslUtil::FreeContext(context);
}

int main(int argc, char **argv) {
  const auto simulationSettings = parseSimulationSettings(argc, argv);
  GlobalVars::InitGlobalVars(simulationSettings);
  runSimulation(simulationSettings);
  return 0;
}