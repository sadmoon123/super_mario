#include "secretbox_1x1.h"
#include "image.h"
#include "define.h"
#include "spritetype.h"
#include <stdio.h>
#include "direction.h"
#include "mainscene.h"

static int secretboxUnopenedImageSequence[] = { 0, 1, 2, 2, 2, 1 };

static int dampingSpeedSequence[] = { -11, -5, 0, +4, +6, +6, +4, +3, -1, -1, -3, -2, 0 };

struct sprite* createSecretbox_1x1()
{
    secretbox_1x1* pSecretbox_1x1 = (struct secretbox_1x1*)malloc(sizeof(secretbox_1x1));
    secretbox_1x1Init(pSecretbox_1x1, secretbox_1x1_appearance_status_unopened);
    return (struct sprite*)pSecretbox_1x1;
}

struct sprite* createBrickSecretbox_1x1()
{
    secretbox_1x1* pSecretbox_1x1 = (struct secretbox_1x1*)malloc(sizeof(secretbox_1x1));
    secretbox_1x1Init(pSecretbox_1x1, secretbox_1x1_appearance_status_mock_brick);
    return (struct sprite*)pSecretbox_1x1;
}

int secrectboxUnopendSeqId = 0;

void secrectboxUpdateUnopendSeqId()
{
    static int freqDivider = 0;
    if (freqDivider >= 3)
    {
        freqDivider = 0;
        secrectboxUnopendSeqId++;
        if (secrectboxUnopendSeqId >= sizeof(secretboxUnopenedImageSequence) / sizeof(secretboxUnopenedImageSequence[0]))
            secrectboxUnopendSeqId = 0;
    }
    freqDivider++;
}

void secretbox_1x1Draw(struct secretbox_1x1* s)
{
    switch (s->appearanceStatus)
    {
    case secretbox_1x1_appearance_status_unopened:
    {
        int curUnopenedImg = secretboxUnopenedImageSequence[secrectboxUnopendSeqId];
        putimage(s->super.x, s->super.y, s->imgArrSecretboxUnopened[curUnopenedImg]);
        break;
    }
    case secretbox_1x1_appearance_status_opened:
        putimage(s->super.x, s->super.y, s->imgSecretboxOpened);
        break;
    case secretbox_1x1_appearance_status_mock_brick:
        putimage(s->super.x, s->super.y, s->imgBrick);
        break;
    }
}

void secretbox_1x1Update(struct secretbox_1x1* s)
{
    switch (s->motionStatus)
    {
    case secretbox_1x1_motion_status_static:
        break;
    case secretbox_1x1_motion_status_damping:
    {
        s->super.vy = dampingSpeedSequence[s->t];
        s->t++;
        if (s->t >= sizeof(dampingSpeedSequence) / sizeof(dampingSpeedSequence[0]))
        {
            s->t = 0;
            s->motionStatus = secretbox_1x1_motion_status_static;
        }
        break;
    }
    }
}

void secretbox_1x1Destroy(struct secretbox_1x1* s)
{
    for (int i = 0; i < 3; i++)
    {
        delete s->imgArrSecretboxUnopened[i];
    }
    delete s->imgSecretboxOpened;
    delete s->imgBrick;

    for (int i = 0; i < s->vecSecretitems.size; i++)
    {
        sprite* item = (sprite*)s->vecSecretitems.get(&s->vecSecretitems, i);
        item->destroy(item);
        free(item);
    }
    vectorDestroy(&s->vecSecretitems);
}

void secretbox_1x1Trigger(struct secretbox_1x1* secretbox, struct sprite* other, int triggerDir, struct mainScene* ms)
{
    if (!(other->spriteType == sprite_type_mario))
        return;

    if (triggerDir != direction_bottom)
        return;

    if (secretbox->appearanceStatus == secretbox_1x1_appearance_status_opened)
        return;

    if (secretbox->motionStatus != secretbox_1x1_motion_status_static)
        return;

    secretbox->motionStatus = secretbox_1x1_motion_status_damping;

    if (secretbox->vecSecretitems.size == 0)
    {
        secretbox->appearanceStatus = secretbox_1x1_appearance_status_opened;
        return;
    }

    // add item into scene
    sprite* item = (sprite*)secretbox->vecSecretitems.get(&secretbox->vecSecretitems, secretbox->vecSecretitems.size - 1);
    ms->spriteAppend(ms, item);
    secretbox->vecSecretitems.remove(&secretbox->vecSecretitems, secretbox->vecSecretitems.size - 1);

    //  trigger item
    if (item->trigger != NULL)
        item->trigger(item, other, triggerDir, ms);

    if (secretbox->vecSecretitems.size == 0)
        secretbox->appearanceStatus = secretbox_1x1_appearance_status_opened;
}

void secretbox_1x1AppendSecretitem(struct secretbox_1x1* s, struct sprite* item)
{
    if (item == NULL)
        return;
    s->vecSecretitems.append(&s->vecSecretitems, item);
}

void secretbox_1x1Init(secretbox_1x1* s, enum secretbox_1x1_appearance_status appearance)
{
    spriteInit((sprite*)s);
    s->super.draw = (void (*)(struct sprite*))secretbox_1x1Draw;
    s->super.update = (void (*)(struct sprite*))secretbox_1x1Update;
    s->super.destroy = (void (*)(struct sprite*))secretbox_1x1Destroy;
    s->super.trigger = (void (*)(struct sprite*, struct sprite*, int, struct mainScene*))secretbox_1x1Trigger;
    s->super.width = GRID_WIDTH;
    s->super.height = GRID_HEIGHT;
    s->super.isCollisionable = true;
    s->super.spriteType = sprite_type_secretbox_1x1;

    s->appearanceStatus = appearance;
    s->motionStatus = secretbox_1x1_motion_status_static;
    s->t = 0;
    s->appendSecretitem = secretbox_1x1AppendSecretitem;
    vectorInit(&s->vecSecretitems);

    char imgPath[50];
    for (int i = 0; i < 3; i++)
    {
        s->imgArrSecretboxUnopened[i] = new IMAGE;
        sprintf(imgPath, "img/secretbox_1x1_unopened_%d.png", i);
        loadimage(s->imgArrSecretboxUnopened[i], imgPath);
    }

    s->imgSecretboxOpened = new IMAGE;
    loadimage(s->imgSecretboxOpened, "img/secretbox_1x1_opened.png");

    s->imgBrick = new IMAGE;
    loadimage(s->imgBrick, "img/brick_1x1.png");
}

