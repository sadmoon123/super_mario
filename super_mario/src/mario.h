#pragma once

#include <easyx.h>
#include "define.h"
#include "sprite.h"

#define MARIO_RUN_ACCELERATION 2
#define MARIO_FRICTION_ACCELERATION 1
#define MARIO_MAX_SPEED 20
#define MARIO_INIT_GLOBAL_X (2 * GRID_WIDTH)
#define MARIO_INIT_GLOBAL_Y (10 * GRID_HEIGHT)
#define MARIO_JUMP_SPEED (-50)
#define MARIO_SQUASHED_MONSTER_JUMP_SPEED (-30)
#define MARIO_DEAD_JUMP_SPEED (-45)

enum marioForm {
    mario_form_small,
    mario_form_super,
    mario_form_fire,
    num_of_mario_form
};

enum marioLevelAction {
    mario_level_action_up,
    mario_level_action_down
};

enum marioFormTransform {
    mario_form_transform_no_action,
    mario_form_transform_small_to_super,
    mario_form_transform_super_to_small,
    mario_form_transform_super_to_fire,
    mario_form_transform_fire_to_small
};

enum marioImageDirection {
    mario_image_direction_left,
    mario_image_direction_right,
    num_of_mario_image_direction
};

enum marioStatus {
    mario_status_brake,
    mario_status_jump,
    mario_status_run,
    mario_status_stand,
    mario_status_dead,
    mario_status_climb,
    mario_status_goto_castle,
    num_of_mario_status
};

enum marioAction {
    mario_action_brake,
    mario_action_jump,
    mario_action_run0,
    mario_action_run1,
    mario_action_run2,
    mario_action_stand,
    mario_action_growing,
    mario_action_hurt,
    mario_action_dead,
    mario_action_climb0,
    mario_action_climb1,
    num_of_mario_action
};

struct mario {
    struct sprite super;
    void (*control)(struct mario* m, ExMessage* msg);
    void (*levelChange)(struct mario* m, enum marioLevelAction lvAction);

    enum marioForm form;
    enum marioFormTransform formTransform;
    enum marioStatus status;
    enum marioAction action;
	
	IMAGE* imgArrMario[num_of_mario_image_direction][num_of_mario_form][num_of_mario_action];
    IMAGE* imgArrMarioMask[num_of_mario_image_direction][num_of_mario_form][num_of_mario_action];
    IMAGE* imgColorChanged;
    IMAGE* imgColorChangedMask;

	enum direction dir;

	bool isLeftKeyDown;
    bool isRightKeyDown;
    bool isSpaceKeyDown;
	
	int runAcc;
    int runFreqDevider;

    int index;
    int frames;

    enum marioForm curFrameForm;
    enum marioAction curFrameAction;

    int vxBeforeFormTransform;
    int vyBeforeFormTransform;

    int climbingFreqDevider;
};

void marioInit(struct mario*);
void marioDestroy(struct mario*);
struct sprite* marioGetMaxTopOverlapSprite(struct mario* pMario, struct vector *vecSprite);