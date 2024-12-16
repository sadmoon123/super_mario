#pragma once

#include "sprite.h"
#include <easyx.h>

#define TORTOISE_1x1_RUNNING_STATUS_HEIGHT 80
#define TORTOISE_1X1_RUNNING_SPEED 5
#define TORTOISE_1X1_SLIDING_SPEED 20

enum tortoise_1x1_status {
	tortoise_1x1_status_runing,
	tortoise_1x1_status_retracted,
	tortoise_1x1_status_exploring,
	tortoise_1x1_status_sliding
};

enum tortoise_image_direction {
	tortoise_image_direction_left,
	tortoise_image_direction_right,
	num_of_tortoise_image_direction
};

enum tortoise_1x1_foot_status {
	tortoise_1x1_foot_status_open,
	tortoise_1x1_foot_status_close,
	num_of_tortoise_1x1_foot_status,
};

struct tortoise_1x1 {
	struct sprite super;

	IMAGE* imgTortoise_1x1[2][2];
	IMAGE* imgTortoise_1x1_mask[2][2];

	IMAGE* imgTortoise_1x1_shell;
	IMAGE* imgTortoise_1x1_shell_mask;

	IMAGE* imgTortoise_1x1_exploring;
	IMAGE* imgTortoise_1x1_exploring_mask;

	enum tortoise_1x1_status status;
	enum tortoise_1x1_foot_status footStatus;
	enum direction runningDir;
	
	int retractedCounter;
	int exploringCounter;
	int footStatusCounter;
};

void tortoise_1x1Init(struct tortoise_1x1*);
struct sprite* createTortoise_1x1();