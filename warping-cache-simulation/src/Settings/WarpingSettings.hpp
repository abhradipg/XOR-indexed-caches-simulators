#pragma once

#define DEFAULT_TRY_WARPING true
#define DEFAULT_USE_HEURISTICS false

struct WarpingSettings {
  bool tryWarping = DEFAULT_TRY_WARPING;
  bool useHeuristics = DEFAULT_USE_HEURISTICS;
};
