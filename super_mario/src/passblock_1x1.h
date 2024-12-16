#pragma once

#include "sprite.h"
#include <easyx.h>

struct passblock_1x1 {
    struct sprite super;
};

void passblock_1x1Init(struct passblock_1x1*);
struct sprite* createPassblock_1x1();