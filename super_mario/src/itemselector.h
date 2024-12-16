#pragma once

#include "sprite.h"
#include <easyx.h>

struct itemSelector {
	struct sprite super;
};

void itemSelectorInit(struct itemSelector*);
struct sprite* createItemSelector();