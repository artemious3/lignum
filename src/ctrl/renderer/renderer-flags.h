#pragma once
#include <stdint.h>
using renderer_flags_t = uint16_t;

const renderer_flags_t RENDERER_IS_SECONDARY = 1 << 0;
const renderer_flags_t RENDERER_IS_ANCESTOR = 1 << 1;
const renderer_flags_t RENDERER_IS_DESCENDANT = 1 << 2;
const renderer_flags_t RENDERER_HAS_ANCESTORS = 1 << 3;
const renderer_flags_t RENDERER_HAS_DESCENDANTS = 1 << 4;
