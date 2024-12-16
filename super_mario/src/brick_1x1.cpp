#include "brick_1x1.h"
#include "define.h"
#include "spritetype.h"
#include "direction.h"
#include "mario.h"
#include "mainscene.h"
#include "brickfragment.h"

static int dampingSpeedSequence[] = { -11, -5, 0, +4, +6, +6, +4, +3, -1, -1, -3, -2, 0 };

struct sprite* createBrick_1x1()
{
	brick_1x1* pBrick_1x1 = (struct brick_1x1*)malloc(sizeof(brick_1x1));
	brick_1x1Init(pBrick_1x1);
	return (struct sprite*)pBrick_1x1;
}

void brick_1x1Draw(struct brick_1x1* b)
{
	putimage(b->super.x, b->super.y, b->imgBrick_1x1);
}

void brick_1x1Update(struct brick_1x1* b)
{
	switch (b->status)
	{
		case brick_status_static:
			break;
		case brick_status_damping:
		{
			b->super.vy = dampingSpeedSequence[b->t];
			b->t++;
			if (b->t >= sizeof(dampingSpeedSequence) / sizeof(dampingSpeedSequence[0]))
			{
				b->t = 0;
				b->status = brick_status_static;
			}
			break;
		}
	}
}

void brick_1x1Destroy(struct brick_1x1* b)
{
	delete b->imgBrick_1x1;
}

void brick_1x1Trigger(struct brick_1x1* b, struct sprite* other, int triggerDir, struct mainScene *ms)
{

	if (!(other->spriteType == sprite_type_mario))
		return;

	if (triggerDir != direction_bottom)
		return;

	if (b->status != brick_status_static)
		return;

	struct mario* pM = (struct mario*)other;
	if (pM->form == mario_form_small)
	{
		b->status = brick_status_damping;
		return;
	}

	b->super.status = sprite_status_destroy;

	struct brickFragment *arrBF[4] = {};
	for (int i = 0; i < 4; i++)
	    arrBF[i] = createBrickFragment();

	//  [0][ ]
	//  [ ][ ]
	arrBF[0]->super.globalX = b->super.globalX;
	arrBF[0]->super.globalY = b->super.globalY;
	arrBF[0]->super.vx = -8;
	arrBF[0]->super.vy = -50;

	//  [ ][1]
	//  [ ][ ]
	arrBF[1]->super.globalX = b->super.globalX + GRID_WIDTH / 2 - 1;
	arrBF[1]->super.globalY = b->super.globalY;
	arrBF[1]->super.vx = 8;
	arrBF[1]->super.vy = -50;

	//  [ ][ ]
	//  [2][ ]
	arrBF[2]->super.globalX = b->super.globalX;
	arrBF[2]->super.globalY = b->super.globalY + GRID_HEIGHT / 2 - 1;
	arrBF[2]->super.vx = -8;
	arrBF[2]->super.vy = -30;

	//  [ ][ ]
	//  [ ][3]
	arrBF[3]->super.globalX = b->super.globalX + GRID_WIDTH / 2 - 1;
	arrBF[3]->super.globalY = b->super.globalY + GRID_HEIGHT / 2 - 1;
	arrBF[3]->super.vx = 8;
	arrBF[3]->super.vy = -30;

	for (int i = 0; i < 4; i++)
	{
		sprite* s = (sprite*)(arrBF[i]);
		ms->spriteAppend(ms, s);
	}
	
}

void brick_1x1Init(struct brick_1x1* b)
{
	spriteInit((sprite*)b);
	b->super.draw = (void (*)(struct sprite*))brick_1x1Draw;
	b->super.update = (void (*)(struct sprite*))brick_1x1Update;
	b->super.destroy = (void (*)(struct sprite*))brick_1x1Destroy;
	b->super.trigger = (void (*)(struct sprite*, struct sprite*, int, struct mainScene*))(brick_1x1Trigger);
	b->super.width = GRID_WIDTH;
	b->super.height = GRID_HEIGHT;
	b->super.isCollisionable = true;
	b->super.spriteType = sprite_type_brick_1x1;
	b->status = brick_status_static;
	b->t = 0;

	b->imgBrick_1x1 = new IMAGE;
	loadimage(b->imgBrick_1x1, "img/brick_1x1.png");
}
