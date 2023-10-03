
#include "isl/union_map.h"     
#include "isl/polynomial.h"                                                  
#include "isl/options.h"  

#include <isl/op.h>
#include <map>

int main() {                                                                     

  isl_ctx *ctx = isl_ctx_alloc();                                               
  isl_options_set_on_error(ctx, ISL_ON_ERROR_ABORT);                             
  isl_union_map *us = isl_union_map_read_from_str(ctx,                           
   "{ S0[i, j, k] -> A[o0] : \
      (k = 0 and i <= 31 and j >= 4 and j <= -2 + i and 4o0 >= -3 + 33j and 4o0 <= 33j) or \
      (exists (e0 = floor((k)/4): 4e0 = k and i <= 31 and j <= -2 + i and k >= 4 and k <= -5 + j and 4o0 > 32j + k and 4o0 < 33j))  \
    }");                                                                         
   isl_union_map_coalesce(us);          
   printf("done\n");                                         
   return 0;                                                                     
}   

// int main() {                                                                     

//   isl_ctx *ctx = isl_ctx_alloc();                                               
//   isl_options_set_on_error(ctx, ISL_ON_ERROR_ABORT);                             
//   auto polynomail = isl::manage(isl_pw_qpolynomial_read_from_str(ctx,                           
//    "{ S_1[c0, c1, c2, c3, c4, i5] -> ((((((4542 + 46 * c1 - 8 * c1^2) - c3) + ((-2 + 16 * c1) - c3) * floor((8c2)/64)) + ((-16 - 16 * c1) + c3) * floor((56 + 8c2)/64)) + (1 + 2 * c1) * floor((8c3)/16)) + (-1 - floor((8c3)/16)) * floor((40 + 8c3)/64) + 2 * floor((40 + 8c3)/64)^2) : i5 = 2 and (c4) mod 8 = 0 and 0 < c1 < c0 and 16c0 < c2 <= 15 + 16c0 and 2 + 16c1 <= c3 <= 7 + 16c1 and c2 < c4 <= 1992 }"));                                                                         
   
//   std::map<long, long> Values;
//   auto enumeratePieces = [&](isl::set Set, isl::qpolynomial Polynomial) {
//     auto enumeratePoints = [&](isl::point Point) {
//       auto Value = Polynomial.eval(Point);
//       Values[Value.get_num_si()] += Value.get_num_si();
//       return isl::stat::ok;
//     };
//     Set.foreach_point(enumeratePoints);
//     return isl::stat::ok;
//   };
//   polynomail.foreach_piece(enumeratePieces);

//   for(auto Value : Values) {
//     printf("Value %ld : %ld\n", Value.first, Value.second);
//   }
   
//    printf("done\n");                                         
//    return 0;                                                                     
// }   
// enumerate and evaluate the polynomail?
//  -> { S_1[c0, c1, c2, c3, c4, i5] -> ((((((4542 + 46 * c1 - 8 * c1^2) - c3) + ((-2 + 16 * c1) - c3) * floor((8c2)/64)) + ((-16 - 16 * c1) + c3) * floor((56 + 8c2)/64)) + (1 + 2 * c1) * floor((8c3)/16)) + (-1 - floor((8c3)/16)) * floor((40 + 8c3)/64) + 2 * floor((40 + 8c3)/64)^2) : i5 = 2 and (c4) mod 8 = 0 and 0 < c1 < c0 and 16c0 < c2 <= 15 + 16c0 and 2 + 16c1 <= c3 <= 7 + 16c1 and c2 < c4 <= 1992 }