#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spriteFactory.h"
#include "define.h"
#include "spritetype.h"
#include "secretbox_1x1.h"

vector* createMap(const char *mapFilePath, int *pWidth, int *pHeight, int *pLayers)
{
    //  open file
    FILE* pFile = fopen(mapFilePath, "r");
    if (pFile == NULL)
        return NULL;

    //  read width, height, layers
    int  width, height, layers;
    int readNums = fscanf(pFile, "%d,%d,%d\n", &width, &height, &layers);
    if (readNums != 3)
        return NULL;
    *pWidth = width;
    *pHeight = height;
    *pLayers = layers;

    //  create map
    vector *pMap = NULL;
    pMap = (vector *)malloc(sizeof(vector));
    if (pMap == NULL)
        return pMap;
    vectorInit(pMap);
    for (int i = 0; i < layers; i++)
    {
        //  create leyers
        vector* pLayer = (vector*)malloc(sizeof(vector));
        pMap->append(pMap, pLayer);
        if (pLayer == NULL)
            continue;
        vectorInit(pLayer);

        for (int j = 0; j < width; j++)
        {
            //  create column vector
            vector* pLayCol = (vector*)malloc(sizeof(vector));
            pLayer->append(pLayer, pLayCol);
            if (pLayCol == NULL)
                continue;
            vectorInit(pLayCol);

            for (int k = 0; k < height; k++)
            {
                //  create row vector
                vector* pLayColRow = (vector*)malloc(sizeof(vector));
                pLayCol->append(pLayCol, pLayColRow);
                if (pLayColRow == NULL)
                    continue;
                vectorInit(pLayColRow);
            }
        }
    }

    //  sprite factory
    spriteFactory sf;
    spriteFactoryInit(&sf);

    //  create sprites from factory
    //  and append it to corresponding layer
    char spriteName[128];
    char coordinates[1024];
    while (1)
    {
        //  spriteName
        char* ret = fgets(spriteName, 128, pFile);
        if (ret == NULL)
            break;
        size_t len = strlen(spriteName);
        spriteName[len - 1] = '\0';
        //  coordinates
        ret = fgets(coordinates, 1024, pFile);
        if (ret == NULL)
            break;
        //  分解坐标
        for (char* token = strtok(coordinates, "()\n"); token != NULL; token = strtok(NULL, "()\n"))
        {
            int x, y, z;
            (void)sscanf(token, "%d,%d,%d", &x, &y, &z);
            //  create sprite
            sprite *s = sf.createSprite(&sf, spriteName);
            if (s == NULL)
                continue;
            s->globalX = x * GRID_WIDTH;
            s->globalY = y * GRID_HEIGHT;
            s->zOrder = z;
            //  find corresponding layer
            vector *pLayer = (vector *)pMap->get(pMap, z);
            if (pLayer == NULL)
                continue;
            //  find corresponding col
            vector *pCol = (vector*)pLayer->get(pLayer, x);
            if (pCol == NULL)
                continue;
            //  find corresponding row
            vector* pColRow = (vector*)pCol->get(pCol, y);
            if (pColRow == NULL)
                continue;
            pColRow->append(pColRow, s);
        }
    }

    fclose(pFile);
    return pMap;
}

void destroyMap(vector *pMap)
{
    if (pMap == NULL)
        return;

    for (int i = 0; i < pMap->size; i++)
    {
        vector *pLayer = (vector*)pMap->get(pMap, i);
        if (pLayer == NULL)
            continue;
        for (int j = 0; j < pLayer->size; j++)
        {
            vector* pCol = (vector*)pLayer->get(pLayer, j);
            if (pCol == NULL)
                continue;
            for (int k = 0; k < pCol->size; k++)
            {
                vector* pColRow = (vector*)pCol->get(pCol, k);
                if (pColRow == NULL)
                    continue;
                for (int l = 0; l < pColRow->size; l++)
                {
                    sprite* s = (sprite*)pColRow->get(pColRow, l);
                    if (s == NULL)
                        continue;
                    s->destroy(s);
                    free(s);
                }
                vectorDestroy(pColRow);
                free(pColRow);
            }
            vectorDestroy(pCol);
            free(pCol);
        }
        vectorDestroy(pLayer);
        free(pLayer);
    }
    vectorDestroy(pMap);
    free(pMap);
}

bool addSecretitemsIntoMap(vector* pMap, const char* secretFilePath)
{
    FILE* pFile = fopen(secretFilePath, "r");
    if (pFile == NULL)
        return false;

    spriteFactory ef;
    spriteFactoryInit(&ef);

    char spriteName[128];
    char coordinates[1024];
    while (1)
    {
        int get = fscanf(pFile, "%s\n", spriteName);
        if (get == EOF)
            break;
        char* ret = fgets(coordinates, 1024, pFile);
        if (ret == NULL)
            break;
        for (char* token = strtok(coordinates, "()\n"); token != NULL; token = strtok(NULL, "()\n"))
        {
            //  声明itemZ用于存储奖励物品所在图层
            int x, y, z, itemZ;
            //  增加一个占位符%d，将读取到的数值存入itemZ
            (void)sscanf(token, "%d,%d,%d,%d", &x, &y, &z, &itemZ);
            sprite* item = ef.createSprite(&ef, spriteName);
            if (item == NULL)
                continue;
            item->globalX = x * GRID_WIDTH;
            item->globalY = y * GRID_HEIGHT;
            item->zOrder = itemZ;

            vector* pLayer = (vector*)pMap->get(pMap, z);
            if (pLayer == NULL)
                continue;
            vector* pCol = (vector*)pLayer->get(pLayer, x);
            if (pCol == NULL)
                continue;
            vector* pColRow = (vector*)pCol->get(pCol, y);
            if (pColRow == NULL)
                continue;

            struct secretbox_1x1* pSecretbox = NULL;
            for (int i = 0; pColRow->size; i++)
            {
                sprite* s = (sprite*)pColRow->get(pColRow, i);
                if (s->spriteType == sprite_type_secretbox_1x1)
                {
                    pSecretbox = (struct secretbox_1x1*)s;
                    break;
                }
            }

            if (pSecretbox == NULL)
                continue;
            pSecretbox->appendSecretitem(pSecretbox, item);
        }
    }

    fclose(pFile);
    return true;
}