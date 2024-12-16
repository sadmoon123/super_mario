#include "moveable.h"
#include "vector.h"
#include "sprite.h"
#include <stdlib.h>

struct vector *createMovables(int layers)
{
	struct vector *movables = (struct vector *)malloc(sizeof(struct vector));
	if (movables == NULL)
		return movables;
	vectorInit(movables);

    for (int i = 0; i < layers; i++)
    {
        struct vector *pLayer = (struct vector *)malloc(sizeof(struct vector));
        movables->append(movables, pLayer);
        if (pLayer == NULL)
            continue;
        vectorInit(pLayer);
    }
    return movables;
}

void destroyMovables(struct vector* movables)
{
    for (int i = 0; i < movables->size; i++)
    {
        struct vector *pLayer = (struct vector*)movables->get(movables, i);
        if (pLayer == NULL)
            continue;
        for (int j = 0; j < pLayer->size; j++)
        {
            struct sprite *s = (struct sprite *)pLayer->get(pLayer, j);
            if (s == NULL)
                continue;
            s->destroy(s);
            free(s);
        }
        vectorDestroy(pLayer);
        free(pLayer);
    }
    vectorDestroy(movables);
    free(movables);
}
