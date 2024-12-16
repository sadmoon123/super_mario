#include "goombas_1x1.h"
#include "image.h"
#include "define.h"
#include "spritetype.h"
#include "direction.h"
#include <stdio.h>

struct sprite* createGoombas_1x1()
{
	goombas_1x1* pGoombas_1x1 = (struct goombas_1x1*)malloc(sizeof(goombas_1x1));
	goombas_1x1Init(pGoombas_1x1);
	return (struct sprite*)pGoombas_1x1;
}

void goombas_1x1Draw(struct goombas_1x1* g)
{
	switch (g->status)
	{
		case goombas_1x1_status_runing:
		{
			putTransparentImage(g->super.x, g->super.y, g->imgGoombas_1x1_mask[g->footDir], g->imgGoombas_1x1[g->footDir]);
			break;
		}
		case goombas_1x1_status_squashed:
		{
			putTransparentImage(g->super.x, g->super.y, g->imgGoombas_1x1_squashed_mask, g->imgGoombas_1x1_squashed);
			break;
		}
	}
}

void goombas_1x1Update(struct goombas_1x1* g)
{
	switch (g->status)
	{
		case goombas_1x1_status_runing:
		{
			if (g->footDirCounter > 4)
			{
				if (g->footDir == goombas_1x1_footDir_left)
					g->footDir = goombas_1x1_footDir_right;
				else if (g->footDir == goombas_1x1_footDir_right)
					g->footDir = goombas_1x1_footDir_left;
				g->footDirCounter = 0;
			}
			g->footDirCounter++;
			
			switch (g->runningDir)
			{
				case direction_left:
				{
					g->super.vx = -GOOMBAS_1X1_SPEED;
					break;
				}
				case direction_right:
				{
					g->super.vx = GOOMBAS_1X1_SPEED;
					break;
				}
			}
			//  ga
			if (!(g->super.collisionDirs & direction_bottom))
				g->super.vy += GRAVITY_ACCELERATION;
			break;
		}
		case goombas_1x1_status_squashed:
		{
			g->super.isCollisionable = false;
			g->squashedCounter++;
			if (g->squashedCounter >= 10)
			{
				g->super.status = sprite_status_destroy;
			}
			break;
		}
	}
}

void goombas_1x1Trigger(struct goombas_1x1* g, struct sprite* t, int triggerDir, struct mainScene *ms)
{
	if (t->zOrder == 0)
		return;

	//  triggered by mario
	if (t->spriteType == sprite_type_mario && triggerDir == direction_top && g->status == goombas_1x1_status_runing)
	{
		g->super.vx = 0;
        g->super.vy = 0;
		g->status = goombas_1x1_status_squashed;
	}
	else
	{
		//  triggered by others
		if (g->status == goombas_1x1_status_runing)
		{
			if ((triggerDir & direction_right) && (g->runningDir & direction_right))
				g->runningDir = direction_left;
			else if ((triggerDir & direction_left) && (g->runningDir & direction_left))
				g->runningDir = direction_right;
		}
	}
}

void goombas_1x1Destroy(struct goombas_1x1* g)
{
	for (int i = 0; i < 2; i++)
	{
		delete g->imgGoombas_1x1[i];
		delete g->imgGoombas_1x1_mask[i];
	}
	delete g->imgGoombas_1x1_squashed;
	delete g->imgGoombas_1x1_squashed_mask;
}

void goombas_1x1Init(struct goombas_1x1* g)
{
	spriteInit((sprite*)g);
	g->super.draw = (void (*)(struct sprite*))goombas_1x1Draw;
	g->super.update = (void (*)(struct sprite*))goombas_1x1Update;
	g->super.destroy = (void (*)(struct sprite*))goombas_1x1Destroy;
	g->super.trigger = (void (*)(struct sprite*, struct sprite*, int, struct mainScene*))goombas_1x1Trigger;
	g->super.spriteType = sprite_type_goombas_1x1;
	g->super.width = GRID_WIDTH;
	g->super.height = GRID_HEIGHT;
	g->super.isMovable = true;
	g->super.isCollisionable = true;

	for (int i = 0; i < 2; i++)
	{
		g->imgGoombas_1x1[i] = new IMAGE;
		char imgPath[50];
		sprintf(imgPath, "img/goombas_1x1_%d.png", i);
		loadimage(g->imgGoombas_1x1[i], imgPath);
	}

	for (int i = 0; i < 2; i++)
	{
		g->imgGoombas_1x1_mask[i] = new IMAGE;
		char imgPath[50];
		sprintf(imgPath, "img/goombas_1x1_%d_mask.png", i);
		loadimage(g->imgGoombas_1x1_mask[i], imgPath);
	}

	g->imgGoombas_1x1_squashed = new IMAGE;
	loadimage(g->imgGoombas_1x1_squashed, "img/goombas_1x1_squashed.png");
	
	g->imgGoombas_1x1_squashed_mask = new IMAGE;
	loadimage(g->imgGoombas_1x1_squashed_mask, "img/goombas_1x1_squashed_mask.png");

	g->status = goombas_1x1_status_runing;
	g->runningDir = direction_left;
	g->footDir = goombas_1x1_footDir_left;
	g->footDirCounter = 0;
	g->squashedCounter = 0;
}
