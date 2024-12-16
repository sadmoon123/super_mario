#include <easyx.h>
#include "gameloop.h"
#include "scene.h"

void gameLoop(struct scene* s, int fps)
{
    timeBeginPeriod(1);
    //  ��ʼʱ�䡢����ʱ�䡢Ƶ��F
    LARGE_INTEGER startCount, endCount, F;
    //  ��ȡƵ��F
    QueryPerformanceFrequency(&F);

    BeginBatchDraw();
    while (1)
    {
        //  ��ȡ��ʼ����
        QueryPerformanceCounter(&startCount);

        //  ��մ���
        cleardevice();

        //  1. ��������
        s->update(s);

        //  2. ���ƻ���
        s->draw(s);

        //  3.  �Ƿ����
        if (s->isQuit(s))
            break;

        //  ��ȡ��������
        QueryPerformanceCounter(&endCount);

        //  ����ʱ��
        long long elapse = (endCount.QuadPart - startCount.QuadPart)
            * 1000000 / F.QuadPart;

        //  ע������ĵ�λ��΢��
        while (elapse < 1000000 / fps)
        {
            Sleep(1);

            //  4. ������Ϣ
            ExMessage msg;
            bool isOk = peekmessage(&msg, EX_KEY);
            if (isOk == true)
            {
                s->control(s, &msg);
            }

            //  ���»�ȡ����ʱ��
            QueryPerformanceCounter(&endCount);
            //  ����ʱ��
            elapse = (endCount.QuadPart - startCount.QuadPart)
                * 1000000 / F.QuadPart;
        }

        //  ���ƻ���
        FlushBatchDraw();
    }
    EndBatchDraw();
    timeEndPeriod(1);
}