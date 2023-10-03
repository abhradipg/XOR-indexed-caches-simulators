#include "SimulationResult.hpp"

SimulationResult::SimulationResult(size_t nCacheLevels)
    : hitMissCounts(nCacheLevels) {
  this->simulatedAccessCounts.resize(nCacheLevels, 0);
  this->warpedAccessCounts.resize(nCacheLevels, 0);
}

const HitMissCountByCacheLevel &SimulationResult::GetHitMissCount() const {
  return this->hitMissCounts;
}

void SimulationResult::AddSimulatedHit(size_t cacheLevel) {
  this->hitMissCounts.AddHit(cacheLevel);
  this->simulatedAccessCounts[cacheLevel]++;
}

void SimulationResult::AddSimulatedMiss(size_t cacheLevel) {
  this->hitMissCounts.AddMiss(cacheLevel);
  this->simulatedAccessCounts[cacheLevel]++;
}

void SimulationResult::Warp(const WarpInfo &warpInfo) {
  const auto &warpedHitMissCount = warpInfo.GetHitMissCount();
  this->hitMissCounts += warpedHitMissCount;
  this->warpedAccessCounts +=
      warpedHitMissCount.GetHitCounts() + warpedHitMissCount.GetMissCounts();
}

void SimulationResult::UpdateTimerResult(const Timer::TimerResult &result) {
  this->timerResult = result;
}

void SimulationResult::PrintJSON(const SimulationSettings &simulationSettings,
                                 std::ostream &os) const {
  os << "{" << std::endl;
  this->PrintSimulationSettingsJSON(simulationSettings, os);
  this->PrintAccessCountsJSON(os);
  this->PrintHitMissCountsJSON(os);
  this->PrintWarpStatsJSON(os);
  this->PrintTimerResultsJSON(os);
  os << std::endl << "}" << std::endl;
}

double SimulationResult::GetWarpedAccessPercentage(size_t cacheLevel) const {

  return (double(this->warpedAccessCounts[cacheLevel]) /
          double(this->hitMissCounts.GetAccessCount(cacheLevel))) *
         100;
}

double SimulationResult::GetNotWarpedAccessPercentage(size_t cacheLevel) const {
  return (double(this->simulatedAccessCounts[cacheLevel]) /
          double(this->hitMissCounts.GetAccessCount(cacheLevel))) *
         100;
}
void SimulationResult::PrintSimulationSettingsJSON(
    const SimulationSettings &settings, std::ostream &os) const {
  os << "\"source\":"
     << "\"" << settings.source << "\"";
  this->PrintCacheSettingsJSON(settings.cacheSettings, os);
  this->PrintWarpingSettingsJSON(settings.warpingSettings, os);
}

void SimulationResult::PrintWarpingSettingsJSON(const WarpingSettings &settings,
                                                std::ostream &os) const {
  const auto &warpingStr = settings.tryWarping ? "true" : "false";
  const auto &heuristicsStr = settings.useHeuristics ? "true" : "false";
  os << "," << std::endl
     << "\"warping\":" << warpingStr << "," << std::endl
     << "\"heuristics\":" << heuristicsStr;
}

void SimulationResult::PrintCacheSettingsJSON(
    const std::vector<CacheSettings> &settings, std::ostream &os) const {
  for (const auto &cs : settings) {
    os << "," << std::endl
       << "\"cacheLevel" << cs.cacheLevel << "\": " << std::endl
       << "{" << std::endl

       << "\"cacheSize\":" << cs.cacheSize << "," << std::endl
       << "\"lineSize\":" << cs.lineSize << "," << std::endl
       << "\"associativity\":" << cs.associativity << "," << std::endl
       << "\"nLines\":" << cs.nCacheLines << "," << std::endl
       << "\"nSets\":" << cs.nCacheSets << "," << std::endl
       << "\"replacementPolicy\":" << cs.GetReplacementPolicyJSONStr() << ","
       << std::endl
       << "\"writePolicy\":" << cs.GetWritePolicyJSONStr() << std::endl
       << "}";
  }
}

void SimulationResult::PrintTimerResultsJSON(std::ostream &os) const {
  const auto &ts = this->timerResult;
  os << "," << std::endl
     << "\"timeMSec\":" << ts.inMilliseconds << "," << std::endl
     << "\"timeSec\":" << ts.inSeconds << "," << std::endl
     << "\"timeMin\":" << ts.inMinutes;
}

void SimulationResult::PrintAccessCountsJSON(std::ostream &os) const {
  os << "," << std::endl << "\"accessCounts\": [";
  for (size_t i = 0; i < this->hitMissCounts.GetNCacheLevels(); i++) {
    if (i != 0)
      os << ", ";
    os << this->hitMissCounts.GetAccessCount(i);
  }
  os << "]";
}

void SimulationResult::PrintHitMissCountsJSON(std::ostream &os) const {
  this->PrintHitCountsJSON(os);
  this->PrintMissCountsJSON(os);
}

void SimulationResult::PrintHitCountsJSON(std::ostream &os) const {
  os << "," << std::endl << "\"hitCounts\": [";
  for (size_t i = 0; i < this->hitMissCounts.GetNCacheLevels(); i++) {
    if (i != 0)
      os << ", ";
    os << this->hitMissCounts.GetHitCount(i);
  }
  os << "]";
}

void SimulationResult::PrintMissCountsJSON(std::ostream &os) const {
  os << "," << std::endl << "\"missCounts\": [";
  for (size_t i = 0; i < this->hitMissCounts.GetNCacheLevels(); i++) {
    if (i != 0)
      os << ", ";
    os << this->hitMissCounts.GetMissCount(i);
  }
  os << "]";
}

void SimulationResult::PrintWarpStatsJSON(std::ostream &os) const {
  this->PrintWarpedAccessPercentagesJSON(os);
  this->PrintNotWarpedAccessPercentagesJSON(os);
}

void SimulationResult::PrintWarpedAccessPercentagesJSON(
    std::ostream &os) const {
  os << "," << std::endl << "\"warpedAccessPercentages\": [";
  for (size_t i = 0; i < this->hitMissCounts.GetNCacheLevels(); i++) {
    if (i != 0)
      os << ", ";
    os << std::fixed << std::setprecision(2)
       << this->GetWarpedAccessPercentage(i);
  }
  os << "]";
}

void SimulationResult::PrintNotWarpedAccessPercentagesJSON(
    std::ostream &os) const {
  os << "," << std::endl << "\"notWarpedAccessPercentages\": [";
  for (size_t i = 0; i < this->hitMissCounts.GetNCacheLevels(); i++) {
    if (i != 0)
      os << ", ";
    os << std::fixed << std::setprecision(2)
       << this->GetNotWarpedAccessPercentage(i);
  }
  os << "]";
}
