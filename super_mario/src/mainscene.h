#pragma once

#include "scene.h"
#include "mario.h"
#include "vector.h"

#define VIEW_MARIO_MAX_MARGIN (6 * GRID_WIDTH)

struct mainScene {
    struct scene super;
    void (*spriteAppend)(struct mainScene *ms, struct sprite *);
    void (*gamePass)(struct mainScene* ms);
    struct vector *pMap;
    struct vector* pMonsterMap;
    struct vector *movables;
    int viewX;
    int width;
    int height;
    int layers;
    struct mario *pMario;
    int exitDelayCounter;
    bool isGamePass;
};

void mainSceneInit(struct mainScene* s);
void mainSceneDestroy(struct mainScene* s);
