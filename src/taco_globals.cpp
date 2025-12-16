// Single definition for global variables declared in headers.
#include "taco/tensor.h"

namespace taco {
// Default follow mode. Placed in a single translation unit to ensure
// there is exactly one instance across the program. This prevents the
// header-included `static` copy-per-TU issue where setting the variable
// in one TU doesn't affect others.
FollowMode followMode = FollowMode::PointerTrack;
} // namespace taco
