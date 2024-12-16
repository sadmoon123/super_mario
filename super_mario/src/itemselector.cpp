#include "itemselector.h"
#include "image.h"
#include "define.h"
#include "spritetype.h"
#include "direction.h"
#include "mario.h"
#include "mainscene.h"
#include "mushroom_1x1.h"
#include "flower_1x1.h"


struct sprite* createItemSelector()
{
	itemSelector* pItemSelector = (struct itemSelector*)malloc(sizeof(itemSelector));
	itemSelectorInit(pItemSelector);
	return (struct sprite*)pItemSelector;
}

void itemSelector1Draw(struct itemSelector* i)
{

}

void itemSelectorUpdate(struct itemSelector* i)
{

}

void itemSelectorDestroy(struct itemSelector* i)
{

}

void itemSelectorTrigger(struct itemSelector* i, struct sprite* other, int triggerDir, struct mainScene *ms)
{
	if (!(other->spriteType == sprite_type_mario))
		return;

	if (triggerDir != direction_bottom)
		return;

	struct mario *pMario = (struct mario *)other;
	struct sprite *rewardItem = NULL;
	if (pMario->form == mario_form_small)
	{
		//  mushroom
		mushroom_1x1* pMushroom = (mushroom_1x1*)createMushroom_1x1();
		pMushroom->super.globalX = i->super.globalX;
		pMushroom->super.globalY = i->super.globalY;
		pMushroom->super.zOrder = i->super.zOrder;
		rewardItem = (struct sprite *)pMushroom;
	}
	else
	{
		//  flower
		flower_1x1* pFlower = (flower_1x1*)createFlower_1x1();
		pFlower->super.globalX = i->super.globalX;
		pFlower->super.globalY = i->super.globalY;
		pFlower->super.zOrder = i->super.zOrder;
		rewardItem = (struct sprite*)pFlower;
	}
	if (rewardItem == NULL)
		return;

	ms->spriteAppend(ms, rewardItem);
	if (rewardItem->trigger != NULL)
		rewardItem->trigger(rewardItem, other, triggerDir, ms);

	i->super.status = sprite_status_destroy;
}

void itemSelectorInit(struct itemSelector* i)
{
	spriteInit((sprite*)i);
	i->super.draw = (void (*)(struct sprite*))itemSelector1Draw;
	i->super.update = (void (*)(struct sprite*))itemSelectorUpdate;
	i->super.destroy = (void (*)(struct sprite*))itemSelectorDestroy;
	i->super.trigger = (void (*)(struct sprite*, struct sprite*, int collision, struct mainScene*))itemSelectorTrigger;
	i->super.spriteType = sprite_type_itemselector;
	i->super.width = GRID_WIDTH;
	i->super.height = GRID_HEIGHT;
}
