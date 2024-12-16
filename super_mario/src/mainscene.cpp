#include "mainscene.h"
#include "vector.h"
#include "map.h"
#include "define.h"
#include "sprite.h"
#include "overlap.h"
#include "collision.h"
#include "direction.h"
#include <stdio.h>
#include "secretbox_1x1.h"
#include "spritetype.h"
#include "moveable.h"
#include "trigger.h"

void getSprites(struct vector *vecSprites, int m, int n, int left, int right, struct mainScene *ms)
{
    for (int i = 0; i < ms->pMap->size; i++)
    {
        //  ��ͼ��ÿһ��
        vector* pLayer = (vector*)ms->pMap->get(ms->pMap, i);
        if (pLayer == NULL)
            continue;
        //  ����ÿһ��
        for (int j = m; j <= n; j++)
        {
            if (j < 0)
                continue;
            vector* pCol = (vector*)pLayer->get(pLayer, j);
            if (pCol == NULL)
                continue;
            //  ����ÿһ����Ԫ��
            for (int k = 0; k < pCol->size; k++)
            {
                struct vector* pColRow = (struct vector*)pCol->get(pCol, k);
                if (pColRow == NULL)
                    continue;
                //  ��Ԫ����ÿһ���������
                for (int l = 0; l < pColRow->size; l++)
                {
                    struct sprite* s = (struct sprite*)pColRow->get(pColRow, l);
                    if (s == NULL)
                        continue;
                    //  ׷�ӽ���vector
                    vecSprites->append(vecSprites, s);
                }
            }
        }

        //  movable sprite
        pLayer = (struct vector*)ms->movables->get(ms->movables, i);
        if (pLayer == NULL)
            continue;
        for (int j = 0; j < pLayer->size; j++)
        {
            struct sprite* s = (struct sprite*)pLayer->get(pLayer, j);
            if (s == NULL)
                continue;
            if (s->globalX + s->width >= left && s->globalX <= right)
            {
                //  ׷�ӽ���vector
                vecSprites->append(vecSprites, s);
            }
        }
    }

    if (ms->pMario != NULL)
        vecSprites->append(vecSprites, ms->pMario);
}

void mainSceneDraw(struct mainScene* ms)
{
    int m, n;
    m = ms->viewX / GRID_WIDTH;
    n = (ms->viewX + VIEW_WIDTH) / GRID_WIDTH;

    int left, right;
    left = ms->viewX;
    right = ms->viewX + VIEW_WIDTH;

    cleardevice();
    //  draw
    struct vector vecSprites;
    vectorInit(&vecSprites);
    getSprites(&vecSprites, m - 4, n, left, right, ms);
    for (int i = 0; i < vecSprites.size; i++)
    {
        sprite* s = (sprite*)vecSprites.get(&vecSprites, i);
        //  ������������draw����
        s->draw(s);
    }
    vectorDestroy(&vecSprites);
}

void spritesMoving(struct mainScene *ms, int m, int n, int left, int right)
{
    struct vector vecSprites;
    vectorInit(&vecSprites);
    getSprites(&vecSprites, m, n, left, right, ms);

    //  ���m��n�ڵ��������ײ�������ƶ�
    for (int i = 0; i < vecSprites.size; i++)
    {
        sprite* one = (sprite*)vecSprites.get(&vecSprites, i);

        int vx, vy;
        vx = one->vx;
        vy = one->vy;

        while (1)
        {
            //  get collision dirs
            if (one->isCollisionable == true)
                one->collisionDirs = getCollisionDirs(one, &vecSprites);

            //  mario can not leave view
            if (one->spriteType == sprite_type_mario)
            {
                if (one->globalX <= ms->viewX)
                    one->collisionDirs |= direction_left;
            }

            //  v to zero
            //  left
            if ((one->collisionDirs & direction_left) && vx < 0)
            {
                vx = 0;
            }
            //  right
            if ((one->collisionDirs & direction_right) && vx > 0)
            {
                vx = 0;
            }
            //  top
            if ((one->collisionDirs & direction_top) && vy < 0)
            {
                vy = 0;
            }
            //  bottom
            if ((one->collisionDirs & direction_bottom) && vy > 0)
            {
                vy = 0;
            }

            //  one v to zero
            //  left
            if ((one->collisionDirs & direction_left) && one->vx < 0)
            {
                one->vx = 0;
            }
            //  right
            if ((one->collisionDirs & direction_right) && one->vx > 0)
            {
                one->vx = 0;
            }
            //  top
            if ((one->collisionDirs & direction_top) && one->vy < 0)
            {
                one->vy = 0;
            }
            //  bottom
            if ((one->collisionDirs & direction_bottom) && one->vy > 0)
            {
                one->vy = 0;
            }

            //  move
            if (vx > 0)
            {
                one->globalX += 1;
                vx -= 1;
            }
            else if (vx < 0)
            {
                one->globalX -= 1;
                vx += 1;
            }
            if (vy > 0)
            {
                one->globalY += 1;
                vy -= 1;
            }
            else if (vy < 0)
            {
                one->globalY -= 1;
                vy += 1;
            }

            if (vx == 0 && vy == 0)
                break;
        }
    }
    vectorDestroy(&vecSprites);
}

void spritesTrigger(struct mainScene* ms, int m, int n, int left, int right)
{
    struct vector vecSprites;
    vectorInit(&vecSprites);
    getSprites(&vecSprites, m, n, left, right, ms);

    struct sprite* marioMaxTopOverlapSprite = marioGetMaxTopOverlapSprite(ms->pMario, &vecSprites);

    //  ���m��n�ڵ��������ײ������
    for (int i = 0; i < vecSprites.size; i++)
    {
        struct sprite* one = (struct sprite*)vecSprites.get(&vecSprites, i);
        if (one->trigger == NULL)
            continue;

        struct vector vecTriggerSprites;
        vectorInit(&vecTriggerSprites);
        getTriggerSprites(one, &vecSprites, &vecTriggerSprites);
        for (int j = 0; j < vecTriggerSprites.size; j++)
        {
            struct triggerSprite* pTs = (struct triggerSprite*)vecTriggerSprites.get(&vecTriggerSprites, j);
            if (pTs->s->spriteType == sprite_type_mario && pTs->dir == direction_bottom)
                if (one != marioMaxTopOverlapSprite)
                    continue;
            one->trigger(one, pTs->s, pTs->dir, ms);
        }
        destroyTriggerSprites(&vecTriggerSprites);
        vectorDestroy(&vecTriggerSprites);
    }
    vectorDestroy(&vecSprites);
}

void clearInvalidSprites(struct mainScene* ms, int m, int n)
{
    for (int i = 0; i < ms->pMap->size; i++)
    {
        //  ��ͼ��ÿһ��
        struct vector* pLayer = (struct vector*)ms->pMap->get(ms->pMap, i);
        if (pLayer == NULL)
            continue;
        //  ����ÿһ��
        for (int j = m; j <= n; j++)
        {
            if (j < 0)
                continue;
            struct vector* pCol = (struct vector*)pLayer->get(pLayer, j);
            if (pCol == NULL)
                continue;
            //  ����ÿһ����Ԫ��
            for (int k = 0; k < pCol->size; k++)
            {
                struct vector* pColRow = (struct vector*)pCol->get(pCol, k);
                if (pColRow == NULL)
                    continue;
                //  ��Ԫ����ÿһ���������
                for (int l = 0; l < pColRow->size; l++)
                {
                    struct sprite* s = (struct sprite*)pColRow->get(pColRow, l);
                    if (s == NULL)
                    {
                        pColRow->remove(pColRow, l);
                        l--;
                        continue;
                    }

                    if (s->globalX + s->width < ms->viewX)
                    {
                        s->status = sprite_status_destroy;
                    }

                    if (s->status == sprite_status_destroy)
                    {
                        pColRow->remove(pColRow, l);
                        l--;
                        s->destroy(s);
                        free(s);
                    }
                }
            }
        }
    }
	
	//  movables
	for (int i = 0; i < ms->movables->size; i++)
	{
        vector *pLayer = (struct vector*)ms->movables->get(ms->movables, i);
        if (pLayer == NULL)
            continue;
        for (int j = 0; j < pLayer->size; j++)
        {
            struct sprite* s = (struct sprite*)pLayer->get(pLayer, j);
            if (s == NULL)
            {
                pLayer->remove(pLayer, j);
                j--;
                continue;
            }

            if (s->globalX + s->width < ms->viewX || s->globalY > VIEW_HEIGHT)
            {
                s->status = sprite_status_destroy;
            }

            if (s->status == sprite_status_destroy)
            {
                pLayer->remove(pLayer, j);
                j--;
                s->destroy(s);
                free(s);
            }
        }
	}

    if (ms->pMario != NULL)
    {
        if (ms->pMario->super.globalY > VIEW_HEIGHT)
        {
            marioDestroy(ms->pMario);
            free(ms->pMario);
            ms->pMario = NULL;
        }
    }
}

void mainSceneSpriteAppend(struct mainScene *ms, struct sprite *s)
{
    if (s->isMovable == false)
    {
        // add sprite into scene
        int x, y, z;
        x = s->globalX / GRID_WIDTH;
        y = s->globalY / GRID_HEIGHT;
        z = s->zOrder;

        //  find corresponding layer
        vector* pLayer = (vector*)ms->pMap->get(ms->pMap, z);
        if (pLayer == NULL)
            return;
        //  find corresponding col
        vector* pCol = (vector*)pLayer->get(pLayer, x);
        if (pCol == NULL)
            return;
        //  find corresponding row
        vector* pColRow = (vector*)pCol->get(pCol, y);
        if (pColRow == NULL)
            return;
        pColRow->append(pColRow, s);
    }
    else
    {
        struct vector* pLayer = (struct vector*)ms->movables->get(ms->movables, s->zOrder);
        if (pLayer == NULL)
            return;
        pLayer->append(pLayer, s);
    }
}

void mainSceneGamePass(struct mainScene* ms)
{
    ms->isGamePass = true;
    ms->pMario->super.globalY = 13 * GRID_HEIGHT;
}

void mainSceneActivateMonsters(struct mainScene* ms, int m, int n)
{
    for (int i = 0; i < ms->pMonsterMap->size; i++)
    {
        //  ��ͼ��ÿһ��
        struct vector* pLayer = (struct vector*)ms->pMonsterMap->get(ms->pMonsterMap, i);
        if (pLayer == NULL)
            continue;
        //  ����m��n
        for (int j = m; j <= n; j++)
        {
            if (j < 0)
                continue;
            struct vector* pCol = (struct vector*)pLayer->get(pLayer, j);
            if (pCol == NULL)
                continue;
            //  ����ÿһ����Ԫ��
            for (int k = 0; k < pCol->size; k++)
            {
                struct vector* pColRow = (struct vector*)pCol->get(pCol, k);
                if (pColRow == NULL)
                    continue;
                //  ��Ԫ����ÿһ���������
                for (int l = 0; l < pColRow->size; l++)
                {
                    //  �õ�monster�������
                    struct sprite* pMonster = (struct sprite*)pColRow->get(pColRow, l);
                    if (pMonster == NULL)
                        continue;

                    //  ������׷�ӵ�movables�ж�Ӧ�Ĳ�
                    vector *pMonsterLayer = (struct vector *)ms->movables->get(ms->movables, i);
                    if (pMonsterLayer == NULL)
                        continue;
                    pMonsterLayer->append(pMonsterLayer, pMonster);

                    //  ��pColRow��ɾ����monster�������
                    pColRow->remove(pColRow, l);
                    l--;
                }
            }
        } 
    }
}

void mainSceneUpdate(struct mainScene* ms)
{
    //  �ƶ�viewX
    //  ���������viewX�ľ������VIEW_MARIO_MAX_MARGINʱ����Ҫ�ƶ�viewX
    if (ms->pMario != NULL)
    {
        if (ms->pMario->super.globalX - ms->viewX > VIEW_MARIO_MAX_MARGIN)
            ms->viewX = ms->pMario->super.globalX - VIEW_MARIO_MAX_MARGIN;
    }
	
    //  m to n
    int m, n;
    m = ms->viewX / GRID_WIDTH;
    n = (ms->viewX + VIEW_WIDTH) / GRID_WIDTH;

    int left, right;
    left = ms->viewX;
    right = ms->viewX + VIEW_WIDTH;

    //  ����monster
    mainSceneActivateMonsters(ms, m, n);

    //  update
    struct vector vecSprites;
    vectorInit(&vecSprites);
    getSprites(&vecSprites, m - 4, n, left, right, ms);
    for (int i = 0; i < vecSprites.size; i++)
    {
        sprite* s = (sprite*)vecSprites.get(&vecSprites, i);
        //  ������������update����
        s->update(s);
    }

    spritesMoving(ms, m - 4, n, left, right);
    spritesTrigger(ms, m - 4, n, left, right);
    secrectboxUpdateUnopendSeqId();

    vecSprites.clear(&vecSprites);
    getSprites(&vecSprites, m - 4, n, left, right, ms);
    for (int i = 0; i < vecSprites.size; i++)
    {
        sprite* s = (sprite*)vecSprites.get(&vecSprites, i);
        //  ������������x��y
        s->x = s->globalX - ms->viewX;
        s->y = s->globalY;
    }
    vectorDestroy(&vecSprites);
    clearInvalidSprites(ms, m - 5, n);
}

void mainSceneControl(struct mainScene* ms, ExMessage* msg)
{
    if (ms->pMario != NULL)
        ms->pMario->control(ms->pMario, msg);
}

bool mainSceneIsQuit(struct mainScene* ms)
{
    //  ����Ϸ�ӳ��˳����������ڵ��� 2 * FPS ʱ���˳���Ϸ
    if (ms->exitDelayCounter >= 2 * FPS)
        return true;

    //  ������¶���ָ��Ϊ�գ�������Ϸ�ӳ��˳�������  
    if (ms->pMario == NULL)
        ms->exitDelayCounter++;

    return false;
}

void mainSceneInit(struct mainScene* ms)
{
    ms->super.draw = (void (*)(struct scene*))mainSceneDraw;
    ms->super.update = (void (*)(struct scene*))mainSceneUpdate;
    ms->super.control = (void (*)(struct scene*, ExMessage*))mainSceneControl;
    ms->super.isQuit = (bool (*)(struct scene*))mainSceneIsQuit;
    ms->spriteAppend = mainSceneSpriteAppend;
    //  ��gamePass��������ΪmainSceneGamePass����
    ms->gamePass = mainSceneGamePass;
    //  ��ͨ�ر�� isGamePass ��ʼ��Ϊ false
    ms->isGamePass = false;

    //  viewX
    ms->viewX = 0;
    ms->exitDelayCounter = 0;
    
    //  map
    ms->pMap = createMap("map.txt", &ms->width, &ms->height, &ms->layers);
    addSecretitemsIntoMap(ms->pMap, "secretitems.txt");

    //  monster map
	int mWidth, mHeight, mLayers;
    ms->pMonsterMap = createMap("monsters.txt", &mWidth, &mHeight, &mLayers);

    //  moveable sprites
    ms->movables = createMovables(ms->layers);

    //  mario
    ms->pMario = (struct mario*)malloc(sizeof(struct mario));
    if (ms->pMario != NULL)
        marioInit(ms->pMario);
}

void mainSceneDestroy(struct mainScene* ms)
{
    destroyMap(ms->pMap);
    destroyMap(ms->pMonsterMap);
    destroyMovables(ms->movables);

    //  ��������
    if (ms->pMario != NULL)
    {
        marioDestroy(ms->pMario);
        free(ms->pMario);
        ms->pMario = NULL;
    }
}