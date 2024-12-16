#pragma once

#include "vector.h"

struct vector* createMap(const char* mapFilePath, int* pWidth, int* pHeight, int* pLevels);
void destroyMap(struct vector *vecMap);
bool addSecretitemsIntoMap(vector* pVecMap, const char* secretFilePath);