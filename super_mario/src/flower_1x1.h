#pragma once

#include "sprite.h"
#include <easyx.h>

enum flower_1x1_status {
	flower_1x1_status_hidden,
	flower_1x1_status_waiting,
	flower_1x1_status_moving_up,
	flower_1x1_status_static
};

struct flower_1x1 {
	struct sprite super;
	IMAGE* imgArrFlower_1x1[4];
	IMAGE* imgFlower_1x1Mask;
	int originGlobalY;
	enum flower_1x1_status status;
	int waitingCounter;
};

void flower_1x1_Init(struct flower_1x1*);
struct sprite* createFlower_1x1();