#include <isl/arg.h>
#include <isl/ctx.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct model_options {
  struct isl_options *isl;
  struct pet_options *pet;
  struct barvinok_options *barvinok;
};

ISL_ARG_DECL(model_options, struct model_options, model_options_args)

#if defined(__cplusplus)
}
#endif
