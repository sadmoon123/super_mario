#pragma once

#include <easyx.h>

void putTransparentImage(int x, int y, const IMAGE* mask, const IMAGE* img);
void flipImage(IMAGE* pImage);
void adjustImageHSV(IMAGE* pImage, int adjustH, int adjustS, int adjustV);