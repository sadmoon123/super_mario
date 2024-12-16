#pragma once

#include "sprite.h"
#include <easyx.h>

enum brickFragmentStatus {
	brick_fragment_status_left,
	brick_fragment_status_right
};

struct brickFragment {
	struct sprite super;
	IMAGE* imgBrickFragmentLeft;
	IMAGE* imgBrickFragmentRight;
	IMAGE* imgBrickFragmentLeftMask;
	IMAGE* imgBrickFragmentRightMask;
	enum brickFragmentStatus status;
	int rotateCounter;
};

void brickFragmentInit(struct brickFragment*);
struct brickFragment* createBrickFragment();