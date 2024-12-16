#pragma once

#include "sprite.h"
#include "vector.h"

struct triggerSprite {
    struct sprite* s;
    int dir;
};

int getTriggerDirs(struct sprite* ont, struct vector* vecSprites);
int getTriggerSprites(struct sprite* one, struct vector* vecSprites, struct vector* vecTriggerSprites);
void destroyTriggerSprites(struct vector* vecTriggerSprites);