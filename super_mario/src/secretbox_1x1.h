#pragma once

#include "sprite.h"
#include <easyx.h>
#include "vector.h"

enum secretbox_1x1_appearance_status {
	secretbox_1x1_appearance_status_unopened,
	secretbox_1x1_appearance_status_mock_brick,
	secretbox_1x1_appearance_status_opened,
};

enum secretbox_1x1_motion_status {
	secretbox_1x1_motion_status_static,
	secretbox_1x1_motion_status_damping
};

struct secretbox_1x1 {
    struct sprite super;
	void (*appendSecretitem)(struct secretbox_1x1*, struct sprite*);
    
	IMAGE* imgArrSecretboxUnopened[3];
	IMAGE* imgBrick;
	IMAGE* imgSecretboxOpened;
	
	enum secretbox_1x1_appearance_status appearanceStatus;
	enum secretbox_1x1_motion_status motionStatus;
	
	int t;

	vector vecSecretitems;
};

void secretbox_1x1Init(struct secretbox_1x1* s, enum secretbox_1x1_appearance_status appearance);
struct sprite* createSecretbox_1x1();
struct sprite* createBrickSecretbox_1x1();
void secrectboxUpdateUnopendSeqId();
