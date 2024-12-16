#pragma once

#include "sprite.h"
#include <easyx.h>

#define GOOMBAS_1X1_SPEED 5

enum goombas_1x1_status {
	goombas_1x1_status_runing,
	goombas_1x1_status_squashed
};

enum goombas_1x1_footDir {
	goombas_1x1_footDir_left,
	goombas_1x1_footDir_right
};

struct goombas_1x1 {
	struct sprite super;
	IMAGE* imgGoombas_1x1[2];
	IMAGE* imgGoombas_1x1_mask[2];
	IMAGE* imgGoombas_1x1_squashed;
	IMAGE* imgGoombas_1x1_squashed_mask;
	enum goombas_1x1_status status;
	enum direction runningDir;
	enum goombas_1x1_footDir footDir;
	int footDirCounter;
	int squashedCounter;
};

void goombas_1x1Init(struct goombas_1x1*);
struct sprite* createGoombas_1x1();