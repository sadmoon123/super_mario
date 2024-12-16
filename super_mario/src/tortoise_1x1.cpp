#include "tortoise_1x1.h"
#include "image.h"
#include "define.h"
#include "spritetype.h"
#include "direction.h"
#include <stdio.h>

struct sprite* createTortoise_1x1()
{
	tortoise_1x1 * pTortoise_1x1 = (struct tortoise_1x1*)malloc(sizeof(tortoise_1x1));
	tortoise_1x1Init(pTortoise_1x1);
	return (struct sprite*)pTortoise_1x1;
}

void tortoise_1x1Draw(struct tortoise_1x1* t)
{
	switch (t->status)
	{
		case tortoise_1x1_status_runing:
		{
			IMAGE* pImage = NULL;
			IMAGE* pImageMask = NULL;
			enum tortoise_image_direction imageDir;
			if (t->runningDir & direction_left)
				imageDir = tortoise_image_direction_left;
			else if (t->runningDir & direction_right)
				imageDir = tortoise_image_direction_right;

			pImage = t->imgTortoise_1x1[imageDir][t->footStatus];
			pImageMask = t->imgTortoise_1x1_mask[imageDir][t->footStatus];

			if (pImage == NULL || pImageMask == NULL)
				return;
			putTransparentImage(t->super.x, t->super.y, pImageMask, pImage);
			break;
		}
		case tortoise_1x1_status_retracted:
		case tortoise_1x1_status_sliding:
		{
			putTransparentImage(t->super.x, t->super.y, t->imgTortoise_1x1_shell_mask, t->imgTortoise_1x1_shell);
			break;
		}

		case tortoise_1x1_status_exploring:
		{
			if (t->footStatus == tortoise_1x1_foot_status_open)
				putTransparentImage(t->super.x, t->super.y, t->imgTortoise_1x1_shell_mask, t->imgTortoise_1x1_shell);
			else if (t->footStatus == tortoise_1x1_foot_status_close)
				putTransparentImage(t->super.x, t->super.y, t->imgTortoise_1x1_exploring_mask, t->imgTortoise_1x1_exploring);
			break;
		}
	}
}

void tortoise_1x1Update(struct tortoise_1x1* t)
{
	switch (t->status)
	{
		case tortoise_1x1_status_runing:
		{
			t->super.height = TORTOISE_1x1_RUNNING_STATUS_HEIGHT;
			
			t->footStatusCounter++;
			if (t->footStatusCounter >= 3)
			{
				if (t->footStatus == tortoise_1x1_foot_status_open)
					t->footStatus = tortoise_1x1_foot_status_close;
				else if (t->footStatus == tortoise_1x1_foot_status_close)
					t->footStatus = tortoise_1x1_foot_status_open;
				t->footStatusCounter = 0;
			}
			
			switch (t->runningDir)
			{
				case direction_left:
				{
					t->super.vx = -TORTOISE_1X1_RUNNING_SPEED;
					break;
				}
				case direction_right:
				{
					t->super.vx = TORTOISE_1X1_RUNNING_SPEED;
					break;
				}
			}
			break;
		}

		case tortoise_1x1_status_sliding:
		{
			t->super.height = GRID_HEIGHT;

			switch (t->runningDir)
			{
				case direction_left:
				{
					t->super.vx = -TORTOISE_1X1_SLIDING_SPEED;
					break;
				}
				case direction_right:
				{
					t->super.vx = TORTOISE_1X1_SLIDING_SPEED;
					break;
				}
			}
			break;
		}

		case tortoise_1x1_status_retracted:
		{
			t->super.height = GRID_HEIGHT;
			
			t->retractedCounter++;
			if (t->retractedCounter >= FPS * 4)
			{
				t->status = tortoise_1x1_status_exploring;
				t->retractedCounter = 0;
			}
			break;
		}

		case tortoise_1x1_status_exploring:
		{
			t->super.height = GRID_HEIGHT;
			
			t->footStatusCounter++;
			if (t->footStatusCounter >= 3)
			{
				if (t->footStatus == tortoise_1x1_foot_status_open)
					t->footStatus = tortoise_1x1_foot_status_close;
				else if (t->footStatus == tortoise_1x1_foot_status_close)
					t->footStatus = tortoise_1x1_foot_status_open;
				t->footStatusCounter = 0;
			}
		
			t->exploringCounter++;
			if (t->exploringCounter >= FPS * 2)
			{ 
				t->status = tortoise_1x1_status_runing;
				t->super.globalY = t->super.globalY - (TORTOISE_1x1_RUNNING_STATUS_HEIGHT - GRID_HEIGHT);
				t->exploringCounter = 0;
				switch (t->runningDir)
				{
					case direction_left:
					{
						t->runningDir = direction_right;
						break;
					}
					case direction_right:
					{
						t->runningDir = direction_left;
						break;
					}
				}
			}
			break;
		}
	}
	//  ga
	if (!(t->super.collisionDirs & direction_bottom))
		t->super.vy += GRAVITY_ACCELERATION;
}

void tortoise_1x1Trigger(struct tortoise_1x1* t, struct sprite* other, int triggerDir, struct mainScene* ms)
{
	if (other->zOrder == 0)
		return;

	//  triggered by mario
	if (other->spriteType == sprite_type_mario)
	{
		if(triggerDir == direction_top && t->status == tortoise_1x1_status_runing)
		{
			t->super.vx = 0;
			t->status = tortoise_1x1_status_retracted;
		}
		else if (t->status == tortoise_1x1_status_retracted || t->status == tortoise_1x1_status_exploring)
		{
			t->status = tortoise_1x1_status_sliding;
			if (triggerDir == direction_left)
			{
				t->runningDir = direction_right;
			}
			else if (triggerDir == direction_right)
			{
				t->runningDir = direction_left;
			}
			else if (triggerDir == direction_top)
			{
				int tLeft = t->super.globalX;
				int tRight = t->super.globalX + t->super.width;
				int mLeft = other->globalX;
				int mRight = other->globalX + other->width;
				if (mLeft < tLeft)
					t->runningDir = direction_right;
				if (tRight < mRight)
					t->runningDir = direction_left;
			}
		}
	}
	else
	{
		//  triggered by others
		if (t->status == tortoise_1x1_status_runing || t->status == tortoise_1x1_status_sliding)
		{
			if (triggerDir == direction_right && (t->runningDir & direction_right))
				t->runningDir = direction_left;
			else if (triggerDir == direction_left && (t->runningDir & direction_left))
				t->runningDir = direction_right;
		}
	}
}

void tortoise_1x1Destroy(struct tortoise_1x1* t)
{
	for (int imageDir = tortoise_image_direction_left; imageDir < num_of_tortoise_image_direction; imageDir++)
	{
		for (int footStatus = tortoise_1x1_foot_status_open; footStatus < num_of_tortoise_image_direction; footStatus++)
		{
			delete t->imgTortoise_1x1[imageDir][footStatus];
			delete t->imgTortoise_1x1_mask[imageDir][footStatus];
		}
	}
	delete t->imgTortoise_1x1_shell;
	delete t->imgTortoise_1x1_shell_mask;
	delete t->imgTortoise_1x1_exploring;
	delete t->imgTortoise_1x1_exploring_mask;
}

void tortoise_1x1Init(struct tortoise_1x1* t)
{
	spriteInit((sprite*)t);
	t->super.draw = (void (*)(struct sprite*))tortoise_1x1Draw;
	t->super.update = (void (*)(struct sprite*))tortoise_1x1Update;
	t->super.destroy = (void (*)(struct sprite*))tortoise_1x1Destroy;
	t->super.trigger = (void (*)(struct sprite*, struct sprite*, int, struct mainScene*))tortoise_1x1Trigger;
	t->super.spriteType = sprite_type_tortoise_1x1;
	t->super.width = GRID_WIDTH;
	t->super.height = TORTOISE_1x1_RUNNING_STATUS_HEIGHT;
	t->super.isMovable = true;
	t->super.isCollisionable = true;

	for (int imageDir = tortoise_image_direction_left; imageDir < num_of_tortoise_image_direction; imageDir++)
	{
		for (int footStatus = tortoise_1x1_foot_status_open; footStatus < num_of_tortoise_1x1_foot_status; footStatus++)
		{
			char imgPath[50];

			sprintf(imgPath, "img/tortoise_1x1_%d.png", footStatus);
			t->imgTortoise_1x1[imageDir][footStatus] = new IMAGE;
			loadimage(t->imgTortoise_1x1[imageDir][footStatus], imgPath);

			sprintf(imgPath, "img/tortoise_1x1_%d_mask.png", footStatus);
			t->imgTortoise_1x1_mask[imageDir][footStatus] = new IMAGE;
			loadimage(t->imgTortoise_1x1_mask[imageDir][footStatus], imgPath);

			if (imageDir == tortoise_image_direction_right)
			{
				flipImage(t->imgTortoise_1x1[imageDir][footStatus]);
				flipImage(t->imgTortoise_1x1_mask[imageDir][footStatus]);
			}
		}
	}

	t->imgTortoise_1x1_shell = new IMAGE;
	loadimage(t->imgTortoise_1x1_shell, "img/tortoise_1x1_shell.png");
	t->imgTortoise_1x1_shell_mask = new IMAGE;
	loadimage(t->imgTortoise_1x1_shell_mask, "img/tortoise_1x1_shell_mask.png");

	t->imgTortoise_1x1_exploring = new IMAGE;
	loadimage(t->imgTortoise_1x1_exploring, "img/tortoise_1x1_exploring.png");
	t->imgTortoise_1x1_exploring_mask = new IMAGE;
	loadimage(t->imgTortoise_1x1_exploring_mask, "img/tortoise_1x1_exploring_mask.png");

	t->status = tortoise_1x1_status_runing;
	t->runningDir = direction_left;
	t->footStatus = tortoise_1x1_foot_status_open;
	
	t->retractedCounter = 0;
	t->exploringCounter = 0;
	t->footStatusCounter = 0;
}
