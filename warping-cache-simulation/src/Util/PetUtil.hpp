#pragma once

#include <cassert>
#include <isl/cpp.h>
#include <list>
#include <pet.h>
#include <string>
#include <vector>

namespace PetUtil {

pet_options *GetPetOptions(const std::vector<std::string> &includes);

isl_args *GetPetOptionsIslArgs();

pet_scop *ExtractScop(isl::ctx context, const std::string &source);

void FreeScop(pet_scop *scop);

isl::union_map GetTaggedMayReads(pet_scop *scop);

isl::union_map GetTaggedMayWrites(pet_scop *scop);

isl::union_map GetTaggedMayReadsAndWrites(pet_scop *scop);

isl::union_set GetAccessDomain(pet_scop *scop);

isl::schedule GetSchedule(pet_scop *scop);

isl::union_map GetScheduleMap(pet_scop *scop);

pet_array **GetArrays(const pet_scop *scop);

size_t GetNumberOfArrays(const pet_scop *scop);

}; // namespace PetUtil
