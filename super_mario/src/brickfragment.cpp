#include "brickfragment.h"
#include "define.h"
#include "spritetype.h"
#include "image.h"
#include "overlap.h"

struct brickFragment* createBrickFragment()
{
	brickFragment *b = (struct brickFragment*)malloc(sizeof(brickFragment));
	brickFragmentInit(b);
	return b;
}

void brickFragmentDraw(struct brickFragment* b)
{
	if (b->status == brick_fragment_status_left)
		putTransparentImage(b->super.x, b->super.y, b->imgBrickFragmentLeftMask, b->imgBrickFragmentLeft);
	else if (b->status == brick_fragment_status_right)
		putTransparentImage(b->super.x, b->super.y, b->imgBrickFragmentRightMask, b->imgBrickFragmentRight);
}

void brickFragmentUpdate(struct brickFragment* b)
{
	if (b->rotateCounter >= 5)
	{
		if (b->status == brick_fragment_status_left)
			b->status = brick_fragment_status_right;
		else if (b->status == brick_fragment_status_right)
			b->status = brick_fragment_status_left;
		b->rotateCounter = 0;
	}
	b->rotateCounter++;

	b->super.vy += GRAVITY_ACCELERATION;

	if (b->super.globalY > VIEW_HEIGHT + b->super.height)
		b->super.status = sprite_status_destroy;
}

void brickFragmentDestroy(struct brickFragment* b)
{
	delete b->imgBrickFragmentLeft;
	delete b->imgBrickFragmentRight;
	delete b->imgBrickFragmentLeftMask;
	delete b->imgBrickFragmentRightMask;
}

void brickFragmentInit(struct brickFragment* b)
{
	spriteInit((sprite*)b);
	b->super.draw = (void (*)(struct sprite*))brickFragmentDraw;
	b->super.update = (void (*)(struct sprite*))brickFragmentUpdate;
	b->super.destroy = (void (*)(struct sprite*))brickFragmentDestroy;
	b->super.spriteType = sprite_type_brick_fragment;
	b->super.width = GRID_WIDTH / 2;
	b->super.height = GRID_HEIGHT / 2;
	b->super.isMovable = true;
	b->super.zOrder = 3;

	b->status = brick_fragment_status_right;
	b->rotateCounter = 0;

	b->imgBrickFragmentLeft = new IMAGE;
	loadimage(b->imgBrickFragmentLeft, "img/brick_fragment_left.png");
	b->imgBrickFragmentRight = new IMAGE;
	loadimage(b->imgBrickFragmentRight, "img/brick_fragment_right.png");
	b->imgBrickFragmentLeftMask = new IMAGE;
	loadimage(b->imgBrickFragmentLeftMask, "img/brick_fragment_left_mask.png");
	b->imgBrickFragmentRightMask = new IMAGE;
	loadimage(b->imgBrickFragmentRightMask, "img/brick_fragment_right_mask.png");
}
