#include "flower_1x1.h"
#include "image.h"
#include "define.h"
#include "spritetype.h"
#include "stdio.h"

struct sprite* createFlower_1x1()
{
	flower_1x1* pFlower_1x1 = (struct flower_1x1*)malloc(sizeof(flower_1x1));
	flower_1x1_Init(pFlower_1x1);
	return (struct sprite*)pFlower_1x1;
}

void flower_1x1Draw(struct flower_1x1* f)
{
	static int flashCounter = 0;
	switch (f->status)
	{
		case flower_1x1_status_hidden:
		case flower_1x1_status_waiting:
			break;
		case flower_1x1_status_moving_up:
		case flower_1x1_status_static:
		{
			putTransparentImage(f->super.x, f->super.y, f->imgFlower_1x1Mask, f->imgArrFlower_1x1[flashCounter]);
			flashCounter++;
			if (flashCounter >= 4)
				flashCounter = 0;
			break;
		}
	}
}

void flower_1x1Update(struct flower_1x1* f)
{
	switch (f->status)
	{
		case flower_1x1_status_hidden:
			break;
		case flower_1x1_status_waiting:
		{
			if (f->waitingCounter >= 10)
				f->status = flower_1x1_status_moving_up;
			f->waitingCounter++;
			break;
		}	
		case flower_1x1_status_moving_up:
		{
			if (f->originGlobalY - f->super.globalY >= GRID_HEIGHT)
			{
				f->super.vy = 0;
				f->status = flower_1x1_status_static;
				f->super.isCollisionable = true;
				break;
			}
			f->super.vy = -4;
			break;
		}
		case flower_1x1_status_static:
			break;
	}
}

void flower_1x1Trigger(struct flower_1x1* f, struct sprite* other, int triggerDir, void* pData)
{
	//  triggered by mario
	if (other->spriteType == sprite_type_mario)
	{
		if (f->status == flower_1x1_status_hidden)
		{
			//  hit secretbox trigger flower
			f->originGlobalY = f->super.globalY;
			f->status = flower_1x1_status_waiting;
		}
		else if (f->status == flower_1x1_status_static)
		{
			//  mario eat flower
			f->super.status = sprite_status_destroy;
		}
	}
}

void flower_1x1Destroy(struct flower_1x1* f)
{
	for (int i = 0; i < 4; i++)
	{
		delete f->imgArrFlower_1x1[i];
	}
	delete f->imgFlower_1x1Mask;
}

void flower_1x1_Init(struct flower_1x1* f)
{
	spriteInit((sprite*)f);
	f->super.draw = (void (*)(struct sprite*))flower_1x1Draw;
	f->super.update = (void (*)(struct sprite*))flower_1x1Update;
	f->super.destroy = (void (*)(struct sprite*))flower_1x1Destroy;
	f->super.trigger = (void (*)(struct sprite*, struct sprite*, int collision, struct mainScene*))flower_1x1Trigger;
	f->super.spriteType = sprite_type_flower_1x1;
	f->super.width = GRID_WIDTH;
	f->super.height = GRID_HEIGHT;
	f->super.isMovable = true;

	f->status = flower_1x1_status_hidden;
	f->waitingCounter = 0;

	for (int i = 0; i < 4; i++)
	{
		char imgPath[50];
		f->imgArrFlower_1x1[i] = new IMAGE;
		sprintf(imgPath, "img/flower_1x1_%d.png", i);
		loadimage(f->imgArrFlower_1x1[i], imgPath);
	}
	f->imgFlower_1x1Mask = new IMAGE;
	loadimage(f->imgFlower_1x1Mask, "img/flower_1x1_mask.png");
}
