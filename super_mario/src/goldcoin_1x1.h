#pragma once

#include "sprite.h"
#include <easyx.h>

#define GOLDCOIN_JUMP_INIT_SPEED (-40)
#define NUM_OF_GOLDCOIN_IMAGE 4

enum goldcoin_1x1_status {
	goldcoin_1x1_status_hidden,
	goldcoin_1x1_status_show,
};

struct goldcoin_1x1 {
	struct sprite super;

	IMAGE* imgArrGoldcoin[NUM_OF_GOLDCOIN_IMAGE];
	IMAGE* imgArrGoldcoinMask[NUM_OF_GOLDCOIN_IMAGE];
	enum goldcoin_1x1_status status;
	int curImage;
	int triggerOriginY;
};

void goldcoin_1x1Init(struct goldcoin_1x1*);
struct sprite* createGoldcoin_1x1();