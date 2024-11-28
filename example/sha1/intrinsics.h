#pragma once

#if defined(__arm64__)
#include <arm_neon.h>
#endif

#if defined(__wasm__)
#include "wasm_arm_neon.h"
#endif
