#include <barvinok/options.h>
#include <isl/options.h>
#include <pet.h>

#include "model_options.h"

ISL_ARGS_START(struct model_options, model_options_args)
ISL_ARG_CHILD(struct model_options, isl, "isl", &isl_options_args, "isl options")
ISL_ARG_CHILD(struct model_options, pet, NULL, &pet_options_args, "pet options")
ISL_ARG_CHILD(struct model_options, barvinok, NULL, &barvinok_options_args, NULL)
ISL_ARGS_END

ISL_ARG_DEF(model_options, struct model_options, model_options_args)
