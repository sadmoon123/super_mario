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
        //  ��¼ͷβָ��
        DWORD* pHead = pDwImage;
        DWORD* pTail = pDwImage + width - 1;
        //  ��תһ��
        while (pHead < pTail)
        {
            //  ����
            DWORD temp;
            temp = *pHead;
            *pHead = *pTail;
            *pTail = temp;
            //  ͷβָ�����
            pHead++;
            pTail--;
        }
        //  ȥ��һ��
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
        //  ��DWORD *ת��ΪRGBTRIPLE *
        RGBTRIPLE* pRgbTriple = (RGBTRIPLE*)(pDwImage + i);
        //  ���ԭʼRGB��ɫbefore
        COLORREF before = RGB(pRgbTriple->rgbtRed, pRgbTriple->rgbtGreen, pRgbTriple->rgbtBlue);
        //  ��beforeת��ΪHSV��ʽ
        float h, s, v;
        RGBtoHSV(before, &h, &s, &v);
        //  h��s��v�ֱ������Ҫ������ֵ
        h = h + adjustH;
        s = s + adjustS;
        v = v + adjustV;
        //  ����ӳ��ɫ��ֵ��0��360����
        if (h >= 360)
            h = (float)((int)h % 360);
        else if (h < 0)
            h = (float)(360 - abs((int)h) % 360);
        //  ������ɫ��HSV�����������ɫΪafter
        COLORREF after;
        after = HSVtoRGB(h, s, v);
        //  ��RGB��ɫתΪBGR�����������ص�
        *(pDwImage + i) = BGR(after);
    }
}