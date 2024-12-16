#include "image.h"

void putTransparentImage(int x, int y, const IMAGE* mask, const IMAGE* img)
{
	putimage(x, y, mask, SRCAND);
	putimage(x, y, img, SRCPAINT);
}

void flipImage(IMAGE* pImage)
{
    int width;
    int height;
    width = pImage->getwidth();
    height = pImage->getheight();

    DWORD* pDwImage = GetImageBuffer(pImage);
    for (int y = 0; y < height; y++)
    {
        //  记录头尾指针
        DWORD* pHead = pDwImage;
        DWORD* pTail = pDwImage + width - 1;
        //  翻转一行
        while (pHead < pTail)
        {
            //  交换
            DWORD temp;
            temp = *pHead;
            *pHead = *pTail;
            *pTail = temp;
            //  头尾指针更新
            pHead++;
            pTail--;
        }
        //  去下一行
        pDwImage += width;
    }
}

void adjustImageHSV(IMAGE* pImage, int adjustH, int adjustS, int adjustV)
{
    int width;
    int height;
    width = pImage->getwidth();
    height = pImage->getheight();

    DWORD* pDwImage = GetImageBuffer(pImage);
    for (int i = 0; i < width * height; i++)
    {
        //  将DWORD *转换为RGBTRIPLE *
        RGBTRIPLE* pRgbTriple = (RGBTRIPLE*)(pDwImage + i);
        //  获得原始RGB颜色before
        COLORREF before = RGB(pRgbTriple->rgbtRed, pRgbTriple->rgbtGreen, pRgbTriple->rgbtBlue);
        //  将before转换为HSV格式
        float h, s, v;
        RGBtoHSV(before, &h, &s, &v);
        //  h、s、v分别加上需要调整的值
        h = h + adjustH;
        s = s + adjustS;
        v = v + adjustV;
        //  重新映射色相值在0到360以内
        if (h >= 360)
            h = (float)((int)h % 360);
        else if (h < 0)
            h = (float)(360 - abs((int)h) % 360);
        //  调整颜色的HSV，调整后的颜色为after
        COLORREF after;
        after = HSVtoRGB(h, s, v);
        //  将RGB颜色转为BGR，并更新像素点
        *(pDwImage + i) = BGR(after);
    }
}