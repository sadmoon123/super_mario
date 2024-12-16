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

//  长大growing数组
static enum marioForm marioGrowingFormArr[] = { mario_form_small, mario_form_small, mario_form_small, mario_form_small, mario_form_small, mario_form_small, mario_form_super, mario_form_small, mario_form_small, mario_form_super, mario_form_small, mario_form_super };
static enum marioAction marioGrowingActionArr[] = { mario_action_stand, mario_action_growing, mario_action_stand, mario_action_growing, mario_action_stand, mario_action_growing, mario_action_stand, mario_action_stand, mario_action_growing, mario_action_stand, mario_action_growing, mario_action_stand };

//  缩小shrinking数组
static enum marioForm marioShrinkingFormArr[] = { mario_form_super, mario_form_super, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small, mario_form_super, mario_form_small };
static enum marioAction marioShrinkingActionArr[] = { mario_action_jump, mario_action_jump, mario_action_jump, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_hurt, mario_action_stand };

//  变色color change数组
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
    //  马里奥加速度
    //  水平方向
    //  ra, run acc跑步加速度
    //  fa, friction acc摩擦力加速度
    int runAcc = 0;
    int frictionAcc = 0;

    //  垂直方向
    //  ga, gravity acceleration重力加速度
    int ga = 0;

    //  run acc跑步加速度
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

    //  friction acc摩擦力加速度
    if (abs(m->super.vx) != 0)
    {
        if (abs(m->super.vx) < MARIO_MAX_SPEED)
            frictionAcc = MARIO_FRICTION_ACCELERATION;
        else
            frictionAcc = MARIO_RUN_ACCELERATION;
        //  摩擦力方向与水平速度方向相反
        frictionAcc *= -sign(m->super.vx);
    }

    //  gravity acc重力加速度
    //  重力加速度在未触及地面时出现
    if (!(m->super.collisionDirs & direction_bottom))
        ga = GRAVITY_ACCELERATION;

    //  水平方向合加速度ax
    int ax = runAcc + frictionAcc;
    //  垂直方向合加速度
    int ay = ga;

    //  计算速度
    //  vx
    m->super.vx += ax;
    //  vy
    m->super.vy += ay;

    //  保存runAcc
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
                //  不在范围内
                m->action = mario_action_run0;
            }
            else
            {
                //  在范围内
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
            //  将马里奥动作切换为死亡动作
            m->action = mario_action_dead;
            break;
        }
        case mario_status_climb:
        {
            //  检查马里奥的动作是否为攀爬动作
            if (m->action < mario_action_climb0 || m->action > mario_action_climb1)
            {
                //  不是攀爬动作，将动作切换为攀爬动作0
                m->action = mario_action_climb0;
            }
            else
            {
                //  坐在地上
                if (isOnGround == true)
                {
                    //  动作固定在攀爬动作0
                    m->action = mario_action_climb0;
                    break;
                }

                //  攀爬动作切换分频器自增
                m->climbingFreqDevider++;
                //  若攀爬动作切换分频器大于2，才切换攀爬动作
                if (m->climbingFreqDevider > 2)
                {
                    if (m->action == mario_action_climb0)
                        m->action = mario_action_climb1;
                    else
                        m->action = mario_action_climb0;
                    //  清零分频器
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
	//  马里奥速度更新
    if (m->formTransform == mario_form_transform_no_action)
	    marioSpeedUpdate(m);
	//  马里奥状态更新
    marioStatusUpdate(m);
    //  马里奥形态变化过程
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
            //  按下a键
            m->levelChange(m, mario_level_action_up);
        }
        else if (msg->vkcode == 0x44)
        {
            //  按下d键
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
                //  调用 marioToDeadStatus 函数，将马里奥切换为死亡状态
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

    //  没有形态变换过程时，无需执行后面的语句
    if (m->formTransform == mario_form_transform_no_action)
        return;

    //  记录之前的速度
    m->vxBeforeFormTransform = m->super.vx;
    m->vyBeforeFormTransform = m->super.vy;
    //  锁定速度
    m->super.vx = 0;
    m->super.vy = 0;
}

bool marioCollisionRule(struct mario* m, struct sprite* other)
{
    //  当马里奥被旗杆触发时，且马里奥处于攀爬状态下，碰撞规则函数返回 false
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
    //  若马里奥被旗杆触发
    else if (other->spriteType == sprite_type_flagpole_1x9 && m->status != mario_status_climb)
    {
        //  将马里奥切换为攀爬状态
        marioToClimbStatus(m);
    }
    //  检查马里奥是否被旗帜触发
    else if (other->spriteType == sprite_type_flag_2x1)
    {
        //  将马里奥切换为跑向城堡状态
        m->status = mario_status_goto_castle;
        //  将右方向键按下标记设置为 true
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
            //  拼接图片路径字符串
            char imgPath[50];
            sprintf(imgPath, "img/mario/%s/%s.png", marioFormStrs[form], marioActionStrs[action]);
            //  创建image并加载图片
            m->imgArrMario[mario_image_direction_right][form][action] = new IMAGE;
            loadimage(m->imgArrMario[mario_image_direction_right][form][action], imgPath);

            //  拼接图片掩码路径字符串
            sprintf(imgPath, "img/mario/%s/%s_mask.png", marioFormStrs[form], marioActionStrs[action]);
            //  创建image并加载图片
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
