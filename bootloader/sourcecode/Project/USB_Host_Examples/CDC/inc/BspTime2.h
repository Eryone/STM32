#ifndef _DrvTime2_h_
#define _DrvTime2_h_

#include "stm32f10x_type.h"



#ifndef NULL
#define NULL        ((void *)0)
#endif





enum {
    eTim1,
    eTim2,
    eTimUpdata,
    eTimYModem,
    eTimMax,
};



//���index��Ӧ��ʱ�䳬ʱ�����㲢����1��δ��ʱ����0
#define IS_TIMEOUT_1MS(index, count)    ((g_Tim2Array[(u16)(index)] >= (count))?  \
                                        ((g_Tim2Array[(u16)(index)] = 0) == 0): 0)


extern volatile int g_Tim2Array[(u16)eTimMax];

void BspTim2Init(void);
void BspTim2Close(void);




#endif
/*********************************** END **************************************/


