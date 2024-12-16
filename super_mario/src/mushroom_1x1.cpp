#include "mushroom_1x1.h"
#include "image.h"
#include "define.h"
#include "spritetype.h"
#include "direction.h"
#include "vector.h"
#include "overlap.h"

struct sprite* createMushroom_1x1()
{
	mushroom_1x1* pMushroom_1x1 = (struct mushroom_1x1*)malloc(sizeof(mushroom_1x1));
	mushroom_1x1Init(pMushroom_1x1);
	return (struct sprite*)pMushroom_1x1;
}

void mushroom_1x1Draw(struct mushroom_1x1* m)
{
	switch (m->status)
	{
		case mushroom_1x1_status_hidden:
		case mushroom_1x1_status_waiting:
			break;
		case mushroom_1x1_status_moving_up:
		case mushroom_1x1_status_running:
		{
			putTransparentImage(m->super.x, m->super.y, m->imgMushroom_1x1_mask, m->imgMushroom_1x1);
			break;
		}
	}
}

void mushroom_1x1Update(struct mushroom_1x1* m)
{	
	switch (m->status)
	{
		case mushroom_1x1_status_hidden:
			break;
		case mushroom_1x1_status_waiting:
		{
			if (m->waitingCounter >= 10)
				m->status = mushroom_1x1_status_moving_up;
			m->waitingCounter++;
			break;
		}
		case mushroom_1x1_status_moving_up:
		{
			if (m->originGlobalY - m->super.globalY >= GRID_HEIGHT)
			{
				m->super.vy = 0;
				m->status = mushroom_1x1_status_running;
				m->super.isCollisionable = true;
				break;
			}
			m->super.vy = -4;
			break;
		}
		case mushroom_1x1_status_running:
		{
			switch (m->runningDir)
			{
				case direction_left:
				{
					m->super.vx = -10;
					break;
				}
				case direction_right:
				{
					m->super.vx = 10;
					break;
				}
			}
			//  ga
			if (!(m->super.collisionDirs & direction_bottom))
				m->super.vy += GRAVITY_ACCELERATION;
			break;
		}
	}
}

void mushroom_1x1Trigger(struct mushroom_1x1* m, struct sprite* t, int triggerDir, struct mainScene *ms)
{
	if (t->zOrder == 0)
		return;

	//  triggered by mario
	if (t->spriteType == sprite_type_mario)
	{
		if (m->status == mushroom_1x1_status_hidden)
		{
			//  hit secretbox trigger mushroom
			m->originGlobalY = m->super.globalY;
			m->status = mushroom_1x1_status_waiting;
		}
		else if (m->status == mushroom_1x1_status_running)
		{
			//  mario eat mushroom
			m->super.status = sprite_status_destroy;
		}
	}
	else
	{
		//  triggered by others
		if (m->status == mushroom_1x1_status_running)
		{
			if (triggerDir == direction_right && m->runningDir == direction_right)
				m->runningDir = direction_left;
			else if (triggerDir == direction_left && m->runningDir == direction_left)
				m->runningDir = direction_right;
		}
	}
}

void mushroom_1x1Destroy(struct mushroom_1x1* m)
{
	delete m->imgMushroom_1x1;
	delete m->imgMushroom_1x1_mask;
}

void mushroom_1x1Init(struct mushroom_1x1 *m)
{
	spriteInit((sprite*)m);
	m->super.draw = (void (*)(struct sprite *))mushroom_1x1Draw;
	m->super.update = (void (*)(struct sprite*))mushroom_1x1Update;
	m->super.destroy = (void (*)(struct sprite *))mushroom_1x1Destroy;
	m->super.trigger = (void (*)(struct sprite*, struct sprite*, int collision, struct mainScene*))mushroom_1x1Trigger;
	m->super.spriteType = sprite_type_mushroom_1x1;
	m->super.width = GRID_WIDTH;
	m->super.height = GRID_HEIGHT;
	m->super.isMovable = true;

	m->status = mushroom_1x1_status_hidden;

	m->imgMushroom_1x1 = new IMAGE;
	loadimage(m->imgMushroom_1x1, "img/mushroom_1x1.png");
	m->imgMushroom_1x1_mask = new IMAGE;
	loadimage(m->imgMushroom_1x1_mask, "img/mushroom_1x1_mask.png");
	
	m->waitingCounter = 0;
	m->runningDir = direction_right;
}
