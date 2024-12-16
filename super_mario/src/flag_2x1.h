#pragma once

#include "sprite.h"
#include <easyx.h>

#define FLAG_2x1_MOVING_SPEED 20

struct flag_2x1 {
	struct sprite super;
	IMAGE* imgFlag_2x1;
	IMAGE* imgFlag_2x1_mask;
	bool isFalling;
};

void flag_2x1Init(struct flag_2x1 *);
struct sprite* createFlag_2x1();