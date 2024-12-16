#pragma once

struct mainScene;

enum spriteStatus {
	sprite_status_normal,
	sprite_status_destroy
};

struct sprite {
	void (*draw)(struct sprite*);
	void (*update)(struct sprite*);
	void (*destroy)(struct sprite*);
	bool (*collisionRule)(struct sprite*, struct sprite*);
	void (*trigger)(struct sprite*, struct sprite*, int,  struct mainScene*);

	int x;
	int y;

	int vx;
	int vy;

	int globalX;
	int globalY;
	int zOrder;

	int width;
	int height;

	bool isTriggerable;

	bool isCollisionable;
	int collisionDirs;

	int spriteType;
	enum spriteStatus status;

	bool isMovable;
};

void spriteInit(struct sprite* s);
