#include "goldcoin_1x1.h"
#include "image.h"
#include "define.h"
#include <stdio.h>
#include "spritetype.h"
#include "direction.h"

struct sprite* createGoldcoin_1x1()
{
	goldcoin_1x1* pGoldcoin_1x1 = (struct goldcoin_1x1*)malloc(sizeof(struct goldcoin_1x1));
	goldcoin_1x1Init(pGoldcoin_1x1);
	return (struct sprite*)pGoldcoin_1x1;
}

void goldcoin_1x1Draw(struct goldcoin_1x1* g)
{
	switch (g->status)
	{
		case goldcoin_1x1_status_hidden:
			break;
		case goldcoin_1x1_status_show:
		{
			g->curImage++;
			if (g->curImage >= NUM_OF_GOLDCOIN_IMAGE)
				g->curImage = 0;
			putTransparentImage(g->super.x, g->super.y, g->imgArrGoldcoinMask[g->curImage], g->imgArrGoldcoin[g->curImage]);
			break;
		}
	}
}

void goldcoin_1x1Update(struct goldcoin_1x1* g)
{
	switch (g->status)
	{
		case goldcoin_1x1_status_hidden:
			break;
		case goldcoin_1x1_status_show:
		{
			g->super.vy += GRAVITY_ACCELERATION;
			if (g->super.globalY >= g->triggerOriginY && g->super.vy > 0)
			{
				g->super.vy = 0;
				g->status = goldcoin_1x1_status_hidden;
			}
			break;
		}
	}
}

void goldcoin_1x1Trigger(struct goldcoin_1x1* goldcoin, struct sprite* other, int triggerDir, void* pData)
{
	if (other->spriteType != sprite_type_mario)
		return;

	if (triggerDir != direction_bottom)
		return;

	if (goldcoin->status != goldcoin_1x1_status_hidden)
		return;

	goldcoin->super.globalY -= GRID_HEIGHT;
	goldcoin->triggerOriginY = goldcoin->super.globalY;
	goldcoin->status = goldcoin_1x1_status_show;
	goldcoin->super.vy = GOLDCOIN_JUMP_INIT_SPEED;
}

void goldcoin_1x1Destroy(struct goldcoin_1x1* s)
{
	for (int i = 0; i < NUM_OF_GOLDCOIN_IMAGE; i++)
	{
		delete s->imgArrGoldcoin[i];
		delete s->imgArrGoldcoinMask[i];
	}
}

void goldcoin_1x1Init(struct goldcoin_1x1* g)
{
	spriteInit((sprite*)g);
	g->super.draw = (void (*)(struct sprite*))goldcoin_1x1Draw;
	g->super.update = (void (*)(struct sprite*))goldcoin_1x1Update;
	g->super.destroy = (void (*)(struct sprite*))goldcoin_1x1Destroy;
	g->super.trigger = (void (*)(struct sprite*, struct sprite*, int collision, struct mainScene *))goldcoin_1x1Trigger;
	g->super.spriteType = sprite_type_goldcoin_1x1;
	g->super.width = GRID_WIDTH;
	g->super.height = GRID_HEIGHT;

	g->status = goldcoin_1x1_status_hidden;
	g->curImage = 0;

	char imgPath[50];
	char imgMaskPath[50];

	for (int i = 0; i < NUM_OF_GOLDCOIN_IMAGE; i++)
	{
		g->imgArrGoldcoin[i] = new IMAGE;
		g->imgArrGoldcoinMask[i] = new IMAGE;
		sprintf(imgPath, "img/goldcoin_1x1_%d.png", i);
		sprintf(imgMaskPath, "img/goldcoin_1x1_%d_mask.png", i);
		loadimage(g->imgArrGoldcoin[i], imgPath);
		loadimage(g->imgArrGoldcoinMask[i], imgMaskPath);
	}
}
