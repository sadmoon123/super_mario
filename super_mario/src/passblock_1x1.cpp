#include "passblock_1x1.h"
#include "define.h"
#include "spritetype.h"
#include "mainscene.h"

struct sprite* createPassblock_1x1()
{
    passblock_1x1* pPassblock_1x1 = (struct passblock_1x1*)malloc(sizeof(passblock_1x1));
    passblock_1x1Init(pPassblock_1x1);
    return (struct sprite*)pPassblock_1x1;
}

void passblock_1x1Draw(struct passblock_1x1* p)
{

}

void passblock_1x1Update(struct passblock_1x1* p)
{

}

void passblock_1x1Destroy(struct passblock_1x1* p)
{

}

void passblock_1x1Trigger(struct passblock_1x1* p, struct sprite* other, int triggerDir, struct mainScene* ms)
{
    if (other->spriteType == sprite_type_mario)
    {
        ms->gamePass(ms);
    }
}

void passblock_1x1Init(struct passblock_1x1* p)
{
    spriteInit((sprite*)p);
    p->super.draw = (void (*)(struct sprite*))passblock_1x1Draw;
    p->super.update = (void (*)(struct sprite*))passblock_1x1Update;
    p->super.destroy = (void (*)(struct sprite*))passblock_1x1Destroy;
    p->super.trigger = (void (*)(struct sprite*, struct sprite*, int collision, struct mainScene*))passblock_1x1Trigger;
    p->super.width = GRID_WIDTH;
    p->super.height = GRID_HEIGHT;
    p->super.spriteType = sprite_type_passblock_1x1;
    p->super.isCollisionable = true;
}