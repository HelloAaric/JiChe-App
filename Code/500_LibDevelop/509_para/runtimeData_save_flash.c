#include "runtimeData_save_flash.h"
#include "flash.h"
#include "string.h"

/*
���Է�����
1. д��һҳ�ٲ�����SOC�洢��λΪ16�����֣�32���ֽ�һ�Ρ�
2. SOC�仯һ�δ洢һ�Ρ���8K���Դ洢250����λ���൱�ڵ��ѭ��һ�Σ�Flash����һ�Ρ�
3. ��������1W�Σ����൱��ѭ��1W�Ρ�����Ϊ���������ˡ�
4. ����Ҫ��ҳд��Ҳ����Ҫͷ�������ݺ�ֱ�ۡ�
5. Ѱ����Ч���ݣ�ֱ��һ�����250�ε�ѭ����ɡ����µķ�0xFFFF������Ч���ݡ�
*/
#define SOC_DATA_ADDR_SIZE                  (8*1024)                        // �洢SOC���ݴ�С
#define SOC_WORD_BYTE_NUM                   (16*2)                          // SOC�洢��λ��32���ֽ�
#define SOC_WORD_WORD_NUM                   (8)                             // SOC�洢��λ��8����
#define SOC_PAGE_WORD_NUM                   250                             // 8k/32 = 250 �����
#define SOC_DATA_ADDR_START                 FLASH_ADDR_RUNTIME_SOC_START
#define SOC_DATA_ADDR_END                   (FLASH_ADDR_RUNTIME_SOC_START + SOC_DATA_ADDR_SIZE - 1)

// Ŀǰʹ��SOC��λ����ͳ�ƣ�ָ����һ���յ�λ
volatile UINT8 gu8_AdressNum_temp = 0;

volatile UINT32 runtimeError = 0;

void runtimeSOC_PageErase(void)
{
    FlashErase_OnePage(SOC_DATA_ADDR_START);
}

UINT16 runtimeSOC_Read_UINT16(UINT32 address)
{
    if(address < SOC_DATA_ADDR_START || address > SOC_DATA_ADDR_END) {
        ++runtimeError;
        return 0;
    }
    return FlashRead_OneHalfWord(address);
}

UINT32 runtimeSOC_Read_UINT32(UINT32 address)
{
    if(address < SOC_DATA_ADDR_START || address > SOC_DATA_ADDR_END) {
        ++runtimeError;
        return 0;
    }
    return FlashRead_OneWord(address);
}

void runtimeSOC_Write_2Word(UINT32 address, UINT32 data0, UINT32 data1)
{
    if(address < SOC_DATA_ADDR_START || address > SOC_DATA_ADDR_END) {
        ++runtimeError;
        return;
    }
    FlashWrite_DoubleWord_NoErase(address, data0, data1);
}

// SOC�֣�Ĭ��8����
void runtimeSOC_Write_SocWord(UINT32* word)
{
    int i = 0;

    if(gu8_AdressNum_temp >= 250) {
        runtimeSOC_PageErase();
        gu8_AdressNum_temp = 0;
    }

    for(i = 0; i < (SOC_WORD_WORD_NUM/2); ++i) {
        runtimeSOC_Write_2Word(SOC_DATA_ADDR_START + gu8_AdressNum_temp*32 + i*8,
                               *(word + 2*i), *(word + 2*i + 1));
    }
    ++gu8_AdressNum_temp;
}

/*
SOC�֣�Ĭ��Ϊ8����
�������µ�soc��
*/
void runtimeSOC_Get_SocWord(UINT32* word)
{
    UINT16 i = 0;
    UINT16 j = 0;
    UINT16 k = 0;
    static UINT8 su8_StartUp_flag = 0;

    switch(su8_StartUp_flag) {
        case 0:
            for(i = 0; i < SOC_PAGE_WORD_NUM; ++i) {
                if(runtimeSOC_Read_UINT32(SOC_DATA_ADDR_START + i*32) == 0xFFFFFFFF) {
                    gu8_AdressNum_temp = i;
                    for(j = 0; j < SOC_WORD_WORD_NUM; ++j) {
                        *(word + j) = runtimeSOC_Read_UINT32(SOC_DATA_ADDR_START + (i-1)*32 + j*4);
                    }
                    // word = (UINT32*)runtimeSOC_Read_UINT16(SOC_DATA_ADDR_START + (i-1)*32);
                    su8_StartUp_flag = 1;
                    return;             // ��ִ�����棬break�Ļ����������ˣ�ֻ�˳�forѭ��
                }
            }
            // �����ѯ�����һ��soc��λ��Ҳ����0xFFFF�������һ��Ϊ���µġ�ָ��ָ��һ����ֵ���Ⱥ���������
            gu8_AdressNum_temp = 0xFF;
            for(j = 0; j < SOC_WORD_WORD_NUM; ++j) {
                *(word + j) = runtimeSOC_Read_UINT32(SOC_DATA_ADDR_START + (i-1)*32 + j*4);
            }
            su8_StartUp_flag = 1;
            break;

        case 1:
            // �������ò��ֻ�ڿ�����ʱ�򣬻���һ�¡����濴�������ڼ��Ƿ���á�
            k = gu8_AdressNum_temp - 1;
            if(k >= SOC_PAGE_WORD_NUM) {
                k = 0;
            }
            for(j = 0; j < SOC_WORD_WORD_NUM; ++j) {
                *(word + j) = runtimeSOC_Read_UINT32(SOC_DATA_ADDR_START + k*32 + j*4);
            }
            break;
        default:
            break;
    }
}



#define LOG_DATA_ADDR_SIZE                  (8*1024)                        // �洢LOG���ݴ�С
#define LOG_WORD_BYTE_NUM                   (16)                            // LOG�洢��λ��16���ֽ�
#define LOG_WORD_WORD_NUM                   (4)                             // LOG�洢��λ��4����
#define LOG_PAGE_WORD_NUM                   100                             // ��100�����ɡ�����Ҳû���塣
#define LOG_PAGE_SAVE_WORD_NUM              200                             // �洢��λΪ200�������Դ���FIFO���ݽṹ
#define LOG_DATA_ADDR_START                 FLASH_ADDR_RUNTIME_LOG_START
#define LOG_DATA_ADDR_OLD_START             (LOG_DATA_ADDR_START + 1600)
#define LOG_DATA_ADDR_END                   (FLASH_ADDR_RUNTIME_LOG_START + 3200 - 1)

volatile UINT32 runtimeError_LOG = 0;
volatile UINT16 gu16_AdressNum_temp_LOG = 0;

// ram�洢ӳ��
UINT32 gu32_LOGram_New[LOG_PAGE_WORD_NUM][LOG_WORD_WORD_NUM] = {0};
UINT32 gu32_LOGram_Old[LOG_PAGE_WORD_NUM][LOG_WORD_WORD_NUM] = {0};

void runtimeLOG_PageErase(void)
{
    FlashErase_OnePage(LOG_DATA_ADDR_START);
}

UINT16 runtimeLOG_Read_UINT16(UINT32 address)
{
    if(address < LOG_DATA_ADDR_START || address > LOG_DATA_ADDR_END) {
        ++runtimeError_LOG;
        return 0;
    }
    return FlashRead_OneHalfWord(address);
}

UINT32 runtimeLOG_Read_UINT32(UINT32 address)
{
    if(address < LOG_DATA_ADDR_START || address > LOG_DATA_ADDR_END) {
        ++runtimeError_LOG;
        return 0;
    }
    return FlashRead_OneWord(address);
}

void runtimeLOG_Write_2Word(UINT32 address, UINT32 data0, UINT32 data1)
{
    if(address < LOG_DATA_ADDR_START || address > LOG_DATA_ADDR_END) {
        ++runtimeError_LOG;
        return;
    }
    FlashWrite_DoubleWord_NoErase(address, data0, data1);
}


/*
���ֲ�����
1. ����ģʽ
    ֻ��Ҫ����ǰ100���������ұ���
2. ӳ��ģʽ
    ֱ����ram�л�ȡ����
˼·��
1. ����ǰ100����
2. ������ˣ���Ĩ��ȫ��FLASH���ݣ�Ȼ���ram�е�ȡ�����������ڱ����С�
3. ǰ��100�����¿�ʼ����ʹ洢��
4. ���Ҫ��ȡ��־��¼�����������µģ�Ȼ�����������ң��������Ϊ0xffff������ffff
*/
void runtimeLOG_Write_LogWord(UINT32* word)
{
    int i = 0;
    int j = 0;

    if(gu16_AdressNum_temp_LOG >= LOG_PAGE_WORD_NUM) {
        runtimeLOG_PageErase();
        gu16_AdressNum_temp_LOG = 0;

        // memcpy(gu32_LOGram_Old, gu32_LOGram_New, LOG_WORD_BYTE_NUM*LOG_PAGE_WORD_NUM);
        memcpy(gu32_LOGram_Old, gu32_LOGram_New, sizeof(gu32_LOGram_New));
        memset(gu32_LOGram_New, 0, sizeof(gu32_LOGram_New));

        // ���Ƶ������ݴ洢��
        for(j = 0; j < LOG_PAGE_WORD_NUM; ++j) {
            for(i = 0; i < LOG_WORD_WORD_NUM/2; ++i) {
                runtimeLOG_Write_2Word(LOG_DATA_ADDR_OLD_START + j*LOG_WORD_BYTE_NUM + i*8,
                                    gu32_LOGram_Old[j][2*i], gu32_LOGram_Old[j][2*i + 1]);
            }
        }
    }

    for(i = 0; i < LOG_WORD_WORD_NUM/2; ++i) {
        runtimeLOG_Write_2Word(LOG_DATA_ADDR_START + gu16_AdressNum_temp_LOG*LOG_WORD_BYTE_NUM + i*8,
                               *(word + 2*i), *(word + 2*i + 1));
        // ����
        gu32_LOGram_New[gu16_AdressNum_temp_LOG][2*i] = *(word + 2*i);
        gu32_LOGram_New[gu16_AdressNum_temp_LOG][2*i + 1] = *(word + 2*i + 1);
    }

    ++gu16_AdressNum_temp_LOG;
}


// LOG�֣�Ĭ��Ϊ4���֡��������µ�LOG�֡�
void runtimeLOG_Get_LogWord_Latest(UINT32* word)
{
    UINT16 i = 0;
    UINT16 j = 0;
    UINT16 k = 0;
    static UINT8 su8_StartUp_flag = 0;

    switch(su8_StartUp_flag) {
        case 0:
            for(i = 0; i < LOG_PAGE_WORD_NUM; ++i) {
                if(runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + i*LOG_WORD_BYTE_NUM) == 0xFFFFFFFF) {
                    gu16_AdressNum_temp_LOG = i;
                    for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
                        *(word + j) = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + (i-1)*LOG_WORD_BYTE_NUM + j*4);
                    }
                    su8_StartUp_flag = 1;
                    return;             // ��ִ�����棬break�Ļ����������ˣ�ֻ�˳�forѭ��
                }
            }
            // �����ѯ�����һ����λ��Ҳ����0xFFFF�������һ��Ϊ���µġ�ָ��ָ��һ����ֵ���Ⱥ���������
            gu16_AdressNum_temp_LOG = 0xFF;
            for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
                *(word + j) = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + (i-1)*LOG_WORD_BYTE_NUM + j*4);
            }
            su8_StartUp_flag = 1;
            break;

        case 1:
            // �������ò��ֻ�ڿ�����ʱ�򣬻���һ�¡����濴�������ڼ��Ƿ���á�
            k = gu16_AdressNum_temp_LOG - 1;
            if(k >= LOG_PAGE_WORD_NUM) {
                k = 0;
            }
            for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
                *(word + j) = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + k*LOG_WORD_BYTE_NUM + j*4);
            }
            break;
        default:
            break;
    }
}

void runtimeLOG_ResetAll(void)
{
    runtimeLOG_PageErase();

    gu16_AdressNum_temp_LOG = 0;
    memset(gu32_LOGram_New, 0, sizeof(gu32_LOGram_New));
    memset(gu32_LOGram_Old, 0, sizeof(gu32_LOGram_Old));
}

// ����LOG�ֽڳ��ȣ�ͬʱ��������100��LOG�֣�δ������LOGĿǰ����
UINT16 runtimeLOG_Get_LogPage(UINT32** word)
{
    UINT16 i = 0;
    UINT16 j = 0;

    // �Ȱ��������ݻ�ȡ��������Ҫ��̫�ࡣ
    for(i = 0; i < LOG_PAGE_WORD_NUM; ++i) {
        for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
            gu32_LOGram_New[i][j] = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + i*LOG_WORD_BYTE_NUM + j*4);
            // gu32_LOGram_New[i][j] = gu32_LOGram_New[i][j] >= 0xFFFFFFFF ? 0 : gu32_LOGram_New[i][j];
            UPLIMIT_TO_ZERO(gu32_LOGram_New[i][j], 0xFFFFFFFF, 0);
            gu32_LOGram_Old[i][j] = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_OLD_START + i*LOG_WORD_BYTE_NUM + j*4);
            UPLIMIT_TO_ZERO(gu32_LOGram_Old[i][j], 0xFFFFFFFF, 0);
        }
    }

    // �ҵ����µ�λ��
    for(i = 0; i < LOG_PAGE_WORD_NUM; ++i) {
        if(gu32_LOGram_New[i][0] == 0) {
            gu16_AdressNum_temp_LOG = i;
        }
    }
    if(i >= LOG_PAGE_WORD_NUM) {
        // �����ѯ�����һ����λ��Ҳ����0xFFFF�������һ��Ϊ���µġ�ָ��ָ��һ����ֵ
        gu16_AdressNum_temp_LOG = 0xFFFF;
    }

    // ��������
    if(gu16_AdressNum_temp_LOG == 0xFFFF) {                     // new����ȫ����100����
        for(i = 0; i < LOG_PAGE_WORD_NUM; ++i) {
            for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
                // *(word + j) = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + i*LOG_WORD_BYTE_NUM + j*4);
                // (word + i)[j] = gu32_LOGram_New[i][j];
                word[i][j] = gu32_LOGram_New[i][j];
            }
        }
    } else {
        if(gu32_LOGram_Old[0][0] == 0) {                        // δ��100����־��¼
            for(i = 0; i < gu16_AdressNum_temp_LOG; ++i) {      // ֱ�Ӹ�ֵ���¡�
                for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
                    word[i][j] = gu32_LOGram_New[i][j];
                }
            }
            return gu16_AdressNum_temp_LOG;

        } else {                                                // ��100���𲽡���һ�����ڱ�������
            // for(i = 0; i < gu16_AdressNum_temp_LOG; ++i) {
            for(i = 0; i < LOG_PAGE_WORD_NUM; ++i) {            // ���������ֻ�ȡ
                for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
                    // *(word + j) = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + i*LOG_WORD_BYTE_NUM + j*4);
                    if(i < 100 - gu16_AdressNum_temp_LOG) {
                        word[i][j] = gu32_LOGram_Old[i + gu16_AdressNum_temp_LOG][j];
                    } else {
                        word[i][j] = gu32_LOGram_New[i + gu16_AdressNum_temp_LOG - 100][j];
                    }
                }
            }
        }
    }

    return LOG_PAGE_WORD_NUM;
}


