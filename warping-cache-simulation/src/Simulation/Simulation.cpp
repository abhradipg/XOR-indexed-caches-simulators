#include "Simulation.hpp"

Simulation::Simulation(isl::ctx context, SimulationSettings settings)
    : settings(std::move(settings)),
      simulationTree(ProgramInfo(context, this->settings.source), settings) {}

void Simulation::Run() {
  auto cacheState = CacheStateFactory::CreateCacheState(
      this->settings.cacheSettings,
      this->simulationTree.GetGlobalIteratorState());
  Timer timer(this->settings.warpingSettings.tryWarping ? "RunWithWarping"
                                                        : "RunWithoutWarping");
  auto result = this->simulationTree.Simulate(*cacheState);
  result.UpdateTimerResult(timer.End());
  result.PrintJSON(this->settings, std::cout);
}
