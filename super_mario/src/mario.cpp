#include "mario.h"
#include "image.h"
#include <stdio.h>
#include "utility.h"
#include <stdlib.h>
#include "vector.h"
#include "direction.h"
#include "spritetype.h"
#include "collision.h"
#include "mushroom_1x1.h"
#include "flower_1x1.h"

const char* marioFormStrs[] = {
    "small",
    "super",
    "fire"
};

const char* marioActionStrs[] = {
    "brake",
    "jump",
    "run0",
    "run1",
    "run2",
    "stand",
    "growing",
    "hurt",
    "dead",
    "climb0",
    "climb1"
};

//  ����growing����
static enum marioForm marioGrowingFormArr[] = { mario_form_small, mario_form_small, mario_form_small, mario_form_small, mario_form_small, mario_form_small, mario_form_super, mario_form_small, mario_form_small, mario_form_super, mario_form_small, mario_form_super };
static enum marioAction marioGrowingActionArr[] = { mario_action_stand, mario_action_growing, mario_action_stand, mario_action_growing, mario_action_stand, mario_action_growing, mario_action_stand, mario_action_stand, mario_action_growing, mario_action_stand, mario_action_growing, mario_action_stand };

//  ��Сshrinking����
static enum marioForm marioShrinkingFormArr[] = { mario_form_super, mario_form_super, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small };
static enum marioAction marioShrinkingActionArr[] = { mario_action_jump, mario_action_jump, mario_action_jump, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_stand };

//  ��ɫcolor change����
static int marioColorChangeArr[] = { -100, -30, +20, +100, 0, -100, -30, +20, +100, 0, -100, -30, +20, +100 };

void marioDraw(struct mario* m)
{
    IMAGE* pImage = NULL;
    IMAGE* pImageMask = NULL;
    enum marioImageDirection imageDir;
    if (m->dir & direction_left)
        imageDir = mario_image_direction_left;
    else if (m->dir & direction_right)
        imageDir = mario_image_direction_right;

    switch (m->formTransform)
    {
        case mario_form_transform_no_action:
        {
            pImage = m->imgArrMario[imageDir][m->form][m->action];
            pImageMask = m->imgArrMarioMask[imageDir][m->form][m->action];
            break;
        }

        case mario_form_transform_small_to_super:
        case mario_form_transform_super_to_small:
        case mario_form_transform_fire_to_small:
        {
            pImage = m->imgArrMario[imageDir][m->curFrameForm][m->curFrameAction];
            pImageMask = m->imgArrMarioMask[imageDir][m->curFrameForm][m->curFrameAction];
            break;
        }
        case mario_form_transform_super_to_fire:
        {
            pImage = m->imgColorChanged;
            pImageMask = m->imgColorChangedMask;
            break;
        }
    }

    if (pImage == NULL || pImageMask == NULL)
        return;
    putTransparentImage(m->super.x, m->super.y, pImageMask, pImage);
}

void marioSpeedUpdate(struct mario* m)
{
    //  ����¼��ٶ�
    //  ˮƽ����
    //  ra, run acc�ܲ����ٶ�
    //  fa, friction accĦ�������ٶ�
    int runAcc = 0;
    int frictionAcc = 0;

    //  ��ֱ����
    //  ga, gravity acceleration�������ٶ�
    int ga = 0;

    //  run acc�ܲ����ٶ�
    //  left
    if (m->isLeftKeyDown == true && m->isRightKeyDown == false)
    {
        runAcc = -MARIO_RUN_ACCELERATION;
    }
    //  right
    else if (m->isLeftKeyDown == false && m->isRightKeyDown == true)
    {
        runAcc = MARIO_RUN_ACCELERATION;
    }

    //  friction accĦ�������ٶ�
    if (abs(m->super.vx) != 0)
    {
        if (abs(m->super.vx) < MARIO_MAX_SPEED)
            frictionAcc = MARIO_FRICTION_ACCELERATION;
        else
            frictionAcc = MARIO_RUN_ACCELERATION;
        //  Ħ����������ˮƽ�ٶȷ����෴
        frictionAcc *= -sign(m->super.vx);
    }

    //  gravity acc�������ٶ�
    //  �������ٶ���δ��������ʱ����
    if (!(m->super.collisionDirs & direction_bottom))
        ga = GRAVITY_ACCELERATION;

    //  ˮƽ����ϼ��ٶ�ax
    int ax = runAcc + frictionAcc;
    //  ��ֱ����ϼ��ٶ�
    int ay = ga;

    //  �����ٶ�
    //  vx
    m->super.vx += ax;
    //  vy
    m->super.vy += ay;

    //  ����runAcc
    m->runAcc = runAcc;
}

void marioToJumpStatus(struct mario* m, int jumpSpeed)
{
    m->status = mario_status_jump;
    m->super.vy = jumpSpeed;
    m->super.collisionDirs &= ~direction_bottom;
    m->isSpaceKeyDown = false;
}

void marioToDeadStatus(struct mario* m)
{
    m->status = mario_status_dead;
    m->super.vy = MARIO_DEAD_JUMP_SPEED;
    m->super.vx = 0;
    m->super.collisionDirs = direction_none;
    m->super.isCollisionable = false;

    m->isLeftKeyDown = false;
    m->isRightKeyDown = false;
}

void marioToClimbStatus(struct mario* m)
{
    m->status = mario_status_climb;
    m->super.vx = 0;
    m->super.vy = 0;
    m->isLeftKeyDown = false;
    m->isRightKeyDown = false;
    m->super.globalX = m->super.globalX + GRID_WIDTH / 2;
}

void growingOrShrinkingProcess(struct mario *m)
{
    enum marioForm* formArr = NULL;
    enum marioAction* actionArr = NULL;
    switch (m->formTransform)
    {
    case mario_form_transform_small_to_super:
    {
        formArr = marioGrowingFormArr;
        actionArr = marioGrowingActionArr;
        break;
    }
    case mario_form_transform_fire_to_small:
    case mario_form_transform_super_to_small:
    {
        formArr = marioShrinkingFormArr;
        actionArr = marioShrinkingActionArr;
        break;
    }
    }

    enum marioForm form = m->curFrameForm;
    enum marioAction action = m->curFrameAction;
    enum marioForm newForm = formArr[m->index];
    enum marioAction newAction = actionArr[m->index];
    m->curFrameForm = formArr[m->index];
    m->curFrameAction = actionArr[m->index];

    enum marioImageDirection imageDir;
    if (m->dir & direction_left)
    {
        imageDir = mario_image_direction_left;
    }
    else if (m->dir & direction_right)
    {
        imageDir = mario_image_direction_right;
    }

    IMAGE* pImage = m->imgArrMario[imageDir][form][action];
    int width = pImage->getwidth();
    int height = pImage->getheight();
    IMAGE* pNewImage = m->imgArrMario[imageDir][newForm][newAction];
    int newWidth = pNewImage->getwidth();
    int newHeight = pNewImage->getheight();

    int globalX, globalY;
    globalX = m->super.globalX;
    globalY = m->super.globalY;

    int newGlobalX, newGlobalY;
    newGlobalX = globalX - (newWidth - width) / 2;
    newGlobalY = globalY - (newHeight - height);

    m->super.globalX = newGlobalX;
    m->super.globalY = newGlobalY;

    m->super.width = newWidth;
    m->super.height = newHeight;
}

void colorChangeProcess(struct mario *m)
{
    enum marioImageDirection imageDir;
    if (m->dir & direction_left)
    {
        imageDir = mario_image_direction_left;
    }
    else if (m->dir & direction_right)
    {
        imageDir = mario_image_direction_right;
    }
    *m->imgColorChanged = *(m->imgArrMario[imageDir][m->form][m->action]);
    *m->imgColorChangedMask = *(m->imgArrMarioMask[imageDir][m->form][m->action]);
    int offsetH = marioColorChangeArr[m->index];
    adjustImageHSV(m->imgColorChanged, offsetH, 0, 0);
}

void marioFormTransformProcess(struct mario* m)
{
    if (m->formTransform == mario_form_transform_no_action)
        return;

	static int transFormFreqDevider = 0;
    if (transFormFreqDevider < 2)
    {
        transFormFreqDevider++;
        return;
    }
    transFormFreqDevider = 0;

    m->index++;
    if (m->index >= m->frames)
    {
        switch (m->formTransform)
        {
        case mario_form_transform_small_to_super:
            m->form = mario_form_super;
            break;
        case mario_form_transform_super_to_small:
            m->form = mario_form_small;
            break;
        case mario_form_transform_fire_to_small:
            m->form = mario_form_small;
            break;
        }
        m->formTransform = mario_form_transform_no_action;
        m->super.vx = m->vxBeforeFormTransform;
        m->super.vy = m->vyBeforeFormTransform;
        return;
    }

    if (m->formTransform == mario_form_transform_super_to_fire)
        colorChangeProcess(m);
    else
        growingOrShrinkingProcess(m);
}

void marioStatusUpdate(struct mario* m)
{
    bool isOnGround = false;
    if (m->super.collisionDirs & direction_bottom)
        isOnGround = true;
    switch (m->status)
    {
        //  stand
        case mario_status_stand:
        {
            m->action = mario_action_stand;
            //  to jump
            if (m->isSpaceKeyDown == true && isOnGround)
            {
                marioToJumpStatus(m, MARIO_JUMP_SPEED);
                break;
            }
            //  to run
            if (m->super.vx != 0)
                m->status = mario_status_run;
            break;
        }
        //  run
        case mario_status_run:
        case mario_status_goto_castle:
        {
            //  action
            if (m->action < mario_action_run0 || m->action > mario_action_run2)
            {
                //  ���ڷ�Χ��
                m->action = mario_action_run0;
            }
            else
            {
                //  �ڷ�Χ��
                if (m->runFreqDevider >= 2)
                {
                    m->action = (enum marioAction)(m->action + 1);
                    if (m->action > mario_action_run2)
                        m->action = mario_action_run0;
                    m->runFreqDevider = 0;
                }
                m->runFreqDevider++;
            }

            //  to stand
            if (m->super.vx == 0)
            {
                m->status = mario_status_stand;
                break;
            }
                
            //  to jump
            if (m->isSpaceKeyDown == true && isOnGround)
            {
                marioToJumpStatus(m, MARIO_JUMP_SPEED);
                break;
            }

            //  to brake
            int raDir, vDir;
            raDir = sign(m->runAcc);
            vDir = sign(m->super.vx);
            if (raDir + vDir == 0 && raDir != 0 && vDir != 0)
            {
                m->status = mario_status_brake;
                break;
            }
            break;
        }
        //  brake
        case mario_status_brake:
        {
            m->action = mario_action_brake;
            //  to jump
            if (m->isSpaceKeyDown == true && isOnGround)
            {
                marioToJumpStatus(m, MARIO_JUMP_SPEED);
                break;
            }

            //  to run
            int raDir, vDir;
            raDir = sign(m->runAcc);
            vDir = sign(m->super.vx);
            if (!(raDir + vDir == 0 && raDir != 0 && vDir != 0))
                m->status = mario_status_run;
            break;
        }
        //  jump
        case mario_status_jump:
        {
            m->action = mario_action_jump;
            if (isOnGround)
            {
                //  to stand
                if (m->super.vx == 0)
                {
                    m->status = mario_status_stand;
                    break;
                }
                //  to run
                else
                {
                    m->status = mario_status_run;
                    break;
                }
            }
            break;
        }
        case mario_status_dead:
        {
            //  ������¶����л�Ϊ��������
            m->action = mario_action_dead;
            break;
        }
        case mario_status_climb:
        {
            //  �������µĶ����Ƿ�Ϊ��������
            if (m->action < mario_action_climb0 || m->action > mario_action_climb1)
            {
                //  ���������������������л�Ϊ��������0
                m->action = mario_action_climb0;
            }
            else
            {
                //  ���ڵ���
                if (isOnGround == true)
                {
                    //  �����̶�����������0
                    m->action = mario_action_climb0;
                    break;
                }

                //  ���������л���Ƶ������
                m->climbingFreqDevider++;
                //  �����������л���Ƶ������2�����л���������
                if (m->climbingFreqDevider > 2)
                {
                    if (m->action == mario_action_climb0)
                        m->action = mario_action_climb1;
                    else
                        m->action = mario_action_climb0;
                    //  �����Ƶ��
                    m->climbingFreqDevider = 0;
                }
            }
			break;
        }
    }

    if (m->super.vx > 0)
        m->dir = direction_right;
    else if (m->super.vx < 0)
        m->dir = direction_left;
}

void marioUpdate(struct mario* m)
{
	//  ������ٶȸ���
    if (m->formTransform == mario_form_transform_no_action)
	    marioSpeedUpdate(m);
	//  �����״̬����
    marioStatusUpdate(m);
    //  �������̬�仯����
    marioFormTransformProcess(m);
}

void marioControl(struct mario* m, ExMessage* msg)
{
    if (msg->message == WM_KEYDOWN)
    {
        if (m->status == mario_status_dead || m->status == mario_status_climb || m->status == mario_status_goto_castle)
            return;

        if (msg->vkcode == VK_LEFT)
        {
            m->isLeftKeyDown = true;
        }
        else if (msg->vkcode == VK_RIGHT)
        {
            m->isRightKeyDown = true;
        }
        else if (msg->vkcode == VK_SPACE)
        {
            if (m->status != mario_status_jump)
                m->isSpaceKeyDown = true;
        }
        else if (msg->vkcode == 0x41)
        {
            //  ����a��
            m->levelChange(m, mario_level_action_up);
        }
        else if (msg->vkcode == 0x44)
        {
            //  ����d��
            m->levelChange(m, mario_level_action_down);
        }
    }
    else if (msg->message == WM_KEYUP)
    {
        if (msg->vkcode == VK_LEFT)
        {
            m->isLeftKeyDown = false;
        }
        else if (msg->vkcode == VK_RIGHT)
        {
            m->isRightKeyDown = false;
        }
    }
}

void marioLevelChange(struct mario* m, enum marioLevelAction lvAction)
{
    if (m->formTransform != mario_form_transform_no_action)
        return;

    switch (m->form)
    {
        case mario_form_small:
        {
            if (lvAction == mario_level_action_up)
            {
                //  small --> super
                m->formTransform = mario_form_transform_small_to_super;
                m->index = 0;
                m->curFrameForm = marioGrowingFormArr[0];
                m->curFrameAction = marioGrowingActionArr[0];
                m->frames = sizeof(marioGrowingActionArr) / sizeof(marioGrowingActionArr[0]);
            }
            else if (lvAction == mario_level_action_down)
            {
                //  ���� marioToDeadStatus ��������������л�Ϊ����״̬
                marioToDeadStatus(m);
            }
            break;
        }
        case mario_form_super:
        {
            if (lvAction == mario_level_action_up)
            {
                // super --> fire
                m->formTransform = mario_form_transform_super_to_fire;
                m->form = mario_form_fire;
                m->index = 0;
                m->frames = sizeof(marioColorChangeArr) / sizeof(marioColorChangeArr[0]);
            }
            else if (lvAction == mario_level_action_down)
            {
                // super --> small
                m->formTransform = mario_form_transform_super_to_small;
                m->index = 0;
                m->curFrameForm = marioShrinkingFormArr[0];
                m->curFrameAction = marioShrinkingActionArr[0];
                m->frames = sizeof(marioShrinkingActionArr) / sizeof(marioShrinkingActionArr[0]);
            }
            break;
        }
        case mario_form_fire:
        {
            if (lvAction == mario_level_action_down)
            {
                // fire --> small
                m->formTransform = mario_form_transform_fire_to_small;
                m->index = 0;
                m->curFrameForm = marioShrinkingFormArr[0];
                m->curFrameAction = marioShrinkingActionArr[0];
                m->frames = sizeof(marioShrinkingActionArr) / sizeof(marioShrinkingActionArr[0]);
            }
            break;
        }
    }

    //  û����̬�任����ʱ������ִ�к�������
    if (m->formTransform == mario_form_transform_no_action)
        return;

    //  ��¼֮ǰ���ٶ�
    m->vxBeforeFormTransform = m->super.vx;
    m->vyBeforeFormTransform = m->super.vy;
    //  �����ٶ�
    m->super.vx = 0;
    m->super.vy = 0;
}

bool marioCollisionRule(struct mario* m, struct sprite* other)
{
    //  ������±���˴���ʱ��������´�������״̬�£���ײ���������� false
    if (other->spriteType == sprite_type_flagpole_1x9)
    {
        if (m->status == mario_status_climb)
            return false;
    }
    return true;
}

void marioTrigger(struct mario* m, struct sprite* other, int collision, struct mainScene *ms)
{

    if (other->spriteType == sprite_type_mushroom_1x1)
    {
        m->levelChange(m, mario_level_action_up);
    }
    else if (other->spriteType == sprite_type_flower_1x1)
    {
        m->levelChange(m, mario_level_action_up);
    }
    else if (other->spriteType == sprite_type_goombas_1x1 || other->spriteType == sprite_type_tortoise_1x1)
    {
        if (collision & direction_bottom)
            marioToJumpStatus(m, MARIO_SQUASHED_MONSTER_JUMP_SPEED);
        else
            m->levelChange(m, mario_level_action_down);
    }
    //  ������±���˴���
    else if (other->spriteType == sprite_type_flagpole_1x9 && m->status != mario_status_climb)
    {
        //  ��������л�Ϊ����״̬
        marioToClimbStatus(m);
    }
    //  ���������Ƿ����Ĵ���
    else if (other->spriteType == sprite_type_flag_2x1)
    {
        //  ��������л�Ϊ����Ǳ�״̬
        m->status = mario_status_goto_castle;
        //  ���ҷ�������±������Ϊ true
        m->isRightKeyDown = true;
    }
}

void marioDestroy(struct mario* m)
{
    delete m->imgColorChanged;
    delete m->imgColorChangedMask;
    for (int imageDir = 0; imageDir < num_of_mario_image_direction; imageDir++)
    {
        for (int form = 0; form < num_of_mario_form; form++)
        {
            for (int action = 0; action < num_of_mario_action; action++)
            {
                delete m->imgArrMario[imageDir][form][action];
                delete m->imgArrMarioMask[imageDir][form][action];
            }
        }
    }
}

void marioInit(struct mario* m)
{
    spriteInit((sprite*)m);
    m->super.draw = (void (*)(struct sprite*))marioDraw;
    m->super.update = (void (*)(struct sprite*))marioUpdate;
    m->super.destroy = (void (*)(struct sprite*))marioDestroy;
    m->super.collisionRule = (bool (*)(struct sprite*, struct sprite*))marioCollisionRule;
    m->super.trigger = (void (*)(struct sprite*, struct sprite*, int collision, struct mainScene*))marioTrigger;
    m->super.globalX = MARIO_INIT_GLOBAL_X;
    m->super.globalY = MARIO_INIT_GLOBAL_Y;
    m->super.zOrder = 2;
    m->super.width = GRID_HEIGHT;
    m->super.height = 1 * GRID_HEIGHT;
    m->super.isCollisionable = true;
    m->super.spriteType = sprite_type_mario;
    m->super.isMovable = true;

    //  members
    m->control = marioControl;
    m->levelChange = marioLevelChange;
    m->form = mario_form_small;
    m->formTransform = mario_form_transform_no_action;
    m->status = mario_status_stand;
    m->dir = direction_right;
    m->isLeftKeyDown = false;
    m->isRightKeyDown = false;
    m->isSpaceKeyDown = false;
    m->runAcc = 0;
    m->runFreqDevider = 0;
    m->climbingFreqDevider = 0;

    m->imgColorChanged = new IMAGE;
    m->imgColorChangedMask = new IMAGE;
    for (int form = 0; form < num_of_mario_form; form++)
    {
        for (int action = 0; action < num_of_mario_action; action++)
        {
            //  ƴ��ͼƬ·���ַ���
            char imgPath[50];
            sprintf(imgPath, "img/mario/%s/%s.png", marioFormStrs[form], marioActionStrs[action]);
            //  ����image������ͼƬ
            m->imgArrMario[mario_image_direction_right][form][action] = new IMAGE;
            loadimage(m->imgArrMario[mario_image_direction_right][form][action], imgPath);

            //  ƴ��ͼƬ����·���ַ���
            sprintf(imgPath, "img/mario/%s/%s_mask.png", marioFormStrs[form], marioActionStrs[action]);
            //  ����image������ͼƬ
            m->imgArrMarioMask[mario_image_direction_right][form][action] = new IMAGE;
            loadimage(m->imgArrMarioMask[mario_image_direction_right][form][action], imgPath);

            m->imgArrMario[mario_image_direction_left][form][action] = new IMAGE;
            *(m->imgArrMario[mario_image_direction_left][form][action]) = *(m->imgArrMario[mario_image_direction_right][form][action]);
            flipImage(m->imgArrMario[mario_image_direction_left][form][action]);

            m->imgArrMarioMask[mario_image_direction_left][form][action] = new IMAGE;
            *(m->imgArrMarioMask[mario_image_direction_left][form][action]) = *(m->imgArrMarioMask[mario_image_direction_right][form][action]);
            flipImage(m->imgArrMarioMask[mario_image_direction_left][form][action]);
        }
    }
}

struct sprite* marioGetMaxTopOverlapSprite(struct mario* pMario, struct vector *vecSprites)
{
    if (pMario == NULL)
        return NULL;
    struct vector vecCollisionSprites;
    vectorInit(&vecCollisionSprites);
    getCollisionSprites((struct sprite*)pMario, vecSprites, &vecCollisionSprites);

    struct sprite* maxTopOverlapSprite = NULL;
    int maxTopOverlap = 0;
    for (int i = 0; i < vecCollisionSprites.size; i++)
    {
        struct collisionSprite* pCs = (struct collisionSprite*)vecCollisionSprites.get(&vecCollisionSprites, i);
        if (pCs->dir != direction_top)
            continue;
        
        struct sprite* one = (struct sprite *)pMario;
        struct sprite *other = pCs->s;

        int oneLeft, oneRight;
        oneLeft = one->globalX;
        oneRight = oneLeft + one->width;

        int otherLeft, otherRight;
        otherLeft = other->globalX;
        otherRight = otherLeft + other->width;

        int overlap = min(oneRight, otherRight) - max(oneLeft, otherLeft);
        if (maxTopOverlap < overlap)
        {
            maxTopOverlap = overlap;
            maxTopOverlapSprite = other;
        }
    }

    destroyCollisionSprites(&vecCollisionSprites);
    vectorDestroy(&vecCollisionSprites);
    return maxTopOverlapSprite;
}
