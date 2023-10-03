#include "PetUtil.hpp"

pet_options *PetUtil::GetPetOptions(const std::vector<std::string> &includes) {
  std::vector<char *> args;
  char argProgram[] = "program";
  char argI[] = "-I";

  args.push_back(argProgram);
  for (const auto &include : includes) {
    args.push_back(argI);
    args.push_back(const_cast<char *>(include.c_str()));
  }

  struct pet_options *petOptions = pet_options_new_with_defaults();
  pet_options_parse(petOptions, args.size(), &args[0], ISL_ARG_ALL);
  return petOptions;
}

isl_args *PetUtil::GetPetOptionsIslArgs() { return &pet_options_args; }

pet_scop *PetUtil::ExtractScop(isl::ctx context, const std::string &source) {
  auto scop =
      pet_scop_extract_from_C_source(context.get(), source.c_str(), nullptr);
  assert(scop != nullptr);
  return scop;
}

void PetUtil::FreeScop(pet_scop *scop) { pet_scop_free(scop); }

isl::union_map PetUtil::GetTaggedMayReads(pet_scop *scop) {
  return isl::manage(pet_scop_get_tagged_may_reads(scop));
}

isl::union_map PetUtil::GetTaggedMayWrites(pet_scop *scop) {
  return isl::manage(pet_scop_get_tagged_may_writes(scop));
}

isl::union_map PetUtil::GetTaggedMayReadsAndWrites(pet_scop *scop) {
  const auto &taggedReads = PetUtil::GetTaggedMayReads(scop);
  const auto &taggedWrites = PetUtil::GetTaggedMayWrites(scop);
  return taggedReads.unite(taggedWrites).coalesce();
}

isl::union_set PetUtil::GetAccessDomain(pet_scop *scop) {
  const auto &reads = isl::manage(pet_scop_get_may_reads(scop));
  const auto &writes = isl::manage(pet_scop_get_may_writes(scop));
  return reads.domain().unite(writes.domain()).coalesce();
}

isl::schedule PetUtil::GetSchedule(pet_scop *scop) {
  return isl::manage(pet_scop_get_schedule(scop));
}

isl::union_map PetUtil::GetScheduleMap(pet_scop *scop) {
  return PetUtil::GetSchedule(scop).get_map();
}

size_t PetUtil::GetNumberOfArrays(const pet_scop *scop) {
  return scop->n_array;
}

pet_array **PetUtil::GetArrays(const pet_scop *scop) { return scop->arrays; }
