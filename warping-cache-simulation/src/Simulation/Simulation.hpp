#pragma once

#include <iomanip>
#include <list>
#include <map>
#include <string>

#include "CacheState/CacheStateFactory.hpp"
#include "ProgramInfo/ProgramInfo.hpp"
#include "Settings/SimulationSettings.hpp"
#include "Simulation/SimulationResult.hpp"
#include "Simulation/SimulationTree.hpp"
#include "Util/Timer.hpp"

class Simulation {
public:
  explicit Simulation(isl::ctx context, SimulationSettings settings);

  void Run();

private:
  SimulationSettings settings;

  SimulationTree simulationTree;
};
