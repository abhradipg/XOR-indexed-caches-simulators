#include "ReplacementPolicy.hpp"

ReplacementPolicy::ReplacementPolicy(
    size_t nLines, const GlobalIteratorState &globalIteratorState)
    : nLines(nLines), globalIteratorState(globalIteratorState) {}

bool ReplacementPolicy::IsFull() const {
  return this->GetNumberOfNonEmptyLines() == this->nLines;
}