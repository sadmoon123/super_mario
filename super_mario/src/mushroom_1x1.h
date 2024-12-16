#pragma once

#include "sprite.h"
#include <easyx.h>

enum mushroom_1x1_status {
	mushroom_1x1_status_hidden,
	mushroom_1x1_status_waiting,
	mushroom_1x1_status_moving_up,
	mushroom_1x1_status_running
};

struct mushroom_1x1 {
	struct sprite super;
	IMAGE *imgMushroom_1x1;
	IMAGE *imgMushroom_1x1_mask;
	enum mushroom_1x1_status status;
	int waitingCounter;
	int originGlobalY;
	int runningDir;
};

void mushroom_1x1Init(struct mushroom_1x1 *);
struct sprite* createMushroom_1x1();