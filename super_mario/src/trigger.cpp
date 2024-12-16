#include "trigger.h"
#include "overlap.h"
#include "direction.h"
#include <stdlib.h>
#include <easyx.h>
#include "utility.h"
#include <math.h>

int getTriggerDirs(struct sprite* one, struct vector* vecSprites)
{
    int dir = direction_none;
    //  判断 one->isTriggerable 属性是否为假
    if (one->isTriggerable == false)
        return dir;

    for (int i = 0; i < vecSprites->size; i++)
    {
        sprite* other = (sprite*)vecSprites->get(vecSprites, i);

        //  判断 one->isTriggerable 属性是否为假
        if (other->isTriggerable == false)
            continue;

        if (one == other)
            continue;
        //  one、other是否重叠
        bool isOverlap = overlapDetection(one, other);
        if (isOverlap == false)
            continue;

        //  两个物体所占空间矩形
        RECT rectOne;
        rectOne.left = one->globalX;
        rectOne.top = one->globalY;
        rectOne.right = one->globalX + one->width - 1;
        rectOne.bottom = one->globalY + one->height - 1;

        RECT rectOther;
        rectOther.left = other->globalX;
        rectOther.top = other->globalY;
        rectOther.right = other->globalX + other->width - 1;
        rectOther.bottom = other->globalY + other->height - 1;

        //  水平投影
        int horizontalProjection = max(rectOne.bottom, rectOther.bottom) - min(rectOne.top, rectOther.top) + 1;
        if (horizontalProjection < one->height + other->height - 1)
        {
            //  left
            if (rectOther.left < rectOne.left)
                dir |= direction_left;
            //  right
            if (rectOther.right > rectOne.right)
                dir |= direction_right;
        }

        //  垂直投影
        int verticalProjection = max(rectOne.right, rectOther.right) - min(rectOne.left, rectOther.left) + 1;
        if (verticalProjection < one->width + other->width - 1)
        {
            //  top
            if (rectOther.top < rectOne.top)
                dir |= direction_top;
            //  bottom
            if (rectOther.bottom > rectOne.bottom)
                dir |= direction_bottom;
        }
    }
    return dir;
}

int getTriggerSprites(struct sprite* one, struct vector* vecSprites, struct vector* vecTriggerSprites)
{
    int dir = direction_none;
    //  判断 one->isTriggerable 属性是否为假
    if (one->isTriggerable == false)
        return dir;

    for (int i = 0; i < vecSprites->size; i++)
    {
        sprite* other = (sprite*)vecSprites->get(vecSprites, i);

        //  判断 one->isTriggerable 属性是否为假
        if (other->isTriggerable == false)
            continue;

        if (one == other)
            continue;
        //  one、other是否重叠
        bool isOverlap = overlapDetection(one, other);
        if (isOverlap == false)
            continue;

        //  两个物体所占空间矩形
        RECT rectOne;
        rectOne.left = one->globalX;
        rectOne.top = one->globalY;
        rectOne.right = one->globalX + one->width - 1;
        rectOne.bottom = one->globalY + one->height - 1;

        RECT rectOther;
        rectOther.left = other->globalX;
        rectOther.top = other->globalY;
        rectOther.right = other->globalX + other->width - 1;
        rectOther.bottom = other->globalY + other->height - 1;

        int d = direction_none;
        //  水平投影
        int horizontalProjection = max(rectOne.bottom, rectOther.bottom) - min(rectOne.top, rectOther.top) + 1;
        if (horizontalProjection < one->height + other->height - 1)
        {
            //  left
            if (rectOther.left < rectOne.left)
            {
                d = direction_left;
                dir |= direction_left;
            }
            //  right
            if (rectOther.right > rectOne.right)
            {
                d = direction_right;
                dir |= direction_right;
            }
        }

        //  垂直投影
        int verticalProjection = max(rectOne.right, rectOther.right) - min(rectOne.left, rectOther.left) + 1;
        if (verticalProjection < one->width + other->width - 1)
        {
            //  top
            if (rectOther.top < rectOne.top)
            {
                d = direction_top;
                dir |= direction_top;
            }

            //  bottom
            if (rectOther.bottom > rectOne.bottom)
            {
                d = direction_bottom;
                dir |= direction_bottom;
            }

        }

        struct triggerSprite* pTs = (struct triggerSprite*)malloc(sizeof(struct triggerSprite));
        if (pTs == NULL)
            continue;
        pTs->s = other;
        pTs->dir = d;
        vecTriggerSprites->append(vecTriggerSprites, pTs);
    }
    return dir;
}

void destroyTriggerSprites(struct vector* vecTriggerSprites)
{
    for (int i = 0; i < vecTriggerSprites->size; i++)
    {
        struct TriggerSprite* pTs = (struct TriggerSprite*)vecTriggerSprites->get(vecTriggerSprites, i);
        if (pTs == NULL)
            continue;
        free(pTs);
    }
}