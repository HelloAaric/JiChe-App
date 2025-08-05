#include "runtimeData_save_flash.h"
#include "flash.h"
#include "string.h"

/*
策略分析：
1. 写满一页再擦除。SOC存储单位为16个半字，32个字节一次。
2. SOC变化一次存储一次。则8K可以存储250个单位。相当于电池循环一次，Flash擦除一次。
3. 寿命我算1W次，则相当于循环1W次。我认为寿命到底了。
4. 不需要分页写，也不需要头管理，数据很直观。
5. 寻找有效数据，直接一个最大250次的循环便可。最新的非0xFFFF就是有效数据。
*/
#define SOC_DATA_ADDR_SIZE                  (8*1024)                        // 存储SOC数据大小
#define SOC_WORD_BYTE_NUM                   (16*2)                          // SOC存储单位，32个字节
#define SOC_WORD_WORD_NUM                   (8)                             // SOC存储单位，8个字
#define SOC_PAGE_WORD_NUM                   250                             // 8k/32 = 250 个最多
#define SOC_DATA_ADDR_START                 FLASH_ADDR_RUNTIME_SOC_START
#define SOC_DATA_ADDR_END                   (FLASH_ADDR_RUNTIME_SOC_START + SOC_DATA_ADDR_SIZE - 1)

// 目前使用SOC单位个数统计，指向下一个空单位
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

// SOC字，默认8个字
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
SOC字，默认为8个字
返回最新的soc字
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
                    return;             // 不执行下面，break的话就有问题了，只退出for循环
                }
            }
            // 如果轮询到最后一个soc单位，也不是0xFFFF，则最后一个为最新的。指针指向一个大值，等后续擦除。
            gu8_AdressNum_temp = 0xFF;
            for(j = 0; j < SOC_WORD_WORD_NUM; ++j) {
                *(word + j) = runtimeSOC_Read_UINT32(SOC_DATA_ADDR_START + (i-1)*32 + j*4);
            }
            su8_StartUp_flag = 1;
            break;

        case 1:
            // 这个函数貌似只在开机的时候，会用一下。后面看看运行期间是否会用。
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



#define LOG_DATA_ADDR_SIZE                  (8*1024)                        // 存储LOG数据大小
#define LOG_WORD_BYTE_NUM                   (16)                            // LOG存储单位，16个字节
#define LOG_WORD_WORD_NUM                   (4)                             // LOG存储单位，4个字
#define LOG_PAGE_WORD_NUM                   100                             // 先100个最多吧。多了也没意义。
#define LOG_PAGE_SAVE_WORD_NUM              200                             // 存储单位为200个，用以处理FIFO数据结构
#define LOG_DATA_ADDR_START                 FLASH_ADDR_RUNTIME_LOG_START
#define LOG_DATA_ADDR_OLD_START             (LOG_DATA_ADDR_START + 1600)
#define LOG_DATA_ADDR_END                   (FLASH_ADDR_RUNTIME_LOG_START + 3200 - 1)

volatile UINT32 runtimeError_LOG = 0;
volatile UINT16 gu16_AdressNum_temp_LOG = 0;

// ram存储映射
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
两种操作：
1. 备份模式
    只需要处理前100条，不足找备份
2. 映射模式
    直接在ram中获取数据
思路：
1. 处理前100条。
2. 如果满了，先抹除全部FLASH数据，然后把ram中的取过来，保存在备份中。
3. 前面100条重新开始计算和存储。
4. 如果要获取日志记录，则先找最新的，然后不足往备份找，如果备份为0xffff，返回ffff
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

        // 复制到旧数据存储处
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
        // 备份
        gu32_LOGram_New[gu16_AdressNum_temp_LOG][2*i] = *(word + 2*i);
        gu32_LOGram_New[gu16_AdressNum_temp_LOG][2*i + 1] = *(word + 2*i + 1);
    }

    ++gu16_AdressNum_temp_LOG;
}


// LOG字，默认为4个字。返回最新的LOG字。
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
                    return;             // 不执行下面，break的话就有问题了，只退出for循环
                }
            }
            // 如果轮询到最后一个单位，也不是0xFFFF，则最后一个为最新的。指针指向一个大值，等后续擦除。
            gu16_AdressNum_temp_LOG = 0xFF;
            for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
                *(word + j) = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + (i-1)*LOG_WORD_BYTE_NUM + j*4);
            }
            su8_StartUp_flag = 1;
            break;

        case 1:
            // 这个函数貌似只在开机的时候，会用一下。后面看看运行期间是否会用。
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

// 返回LOG字节长度，同时返回所有100个LOG字，未满返回LOG目前长度
UINT16 runtimeLOG_Get_LogPage(UINT32** word)
{
    UINT16 i = 0;
    UINT16 j = 0;

    // 先把所有数据获取回来，不要管太多。
    for(i = 0; i < LOG_PAGE_WORD_NUM; ++i) {
        for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
            gu32_LOGram_New[i][j] = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + i*LOG_WORD_BYTE_NUM + j*4);
            // gu32_LOGram_New[i][j] = gu32_LOGram_New[i][j] >= 0xFFFFFFFF ? 0 : gu32_LOGram_New[i][j];
            UPLIMIT_TO_ZERO(gu32_LOGram_New[i][j], 0xFFFFFFFF, 0);
            gu32_LOGram_Old[i][j] = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_OLD_START + i*LOG_WORD_BYTE_NUM + j*4);
            UPLIMIT_TO_ZERO(gu32_LOGram_Old[i][j], 0xFFFFFFFF, 0);
        }
    }

    // 找到最新的位置
    for(i = 0; i < LOG_PAGE_WORD_NUM; ++i) {
        if(gu32_LOGram_New[i][0] == 0) {
            gu16_AdressNum_temp_LOG = i;
        }
    }
    if(i >= LOG_PAGE_WORD_NUM) {
        // 如果轮询到最后一个单位，也不是0xFFFF，则最后一个为最新的。指针指向一个大值
        gu16_AdressNum_temp_LOG = 0xFFFF;
    }

    // 返回数据
    if(gu16_AdressNum_temp_LOG == 0xFFFF) {                     // new里面全都是100个。
        for(i = 0; i < LOG_PAGE_WORD_NUM; ++i) {
            for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
                // *(word + j) = runtimeLOG_Read_UINT32(LOG_DATA_ADDR_START + i*LOG_WORD_BYTE_NUM + j*4);
                // (word + i)[j] = gu32_LOGram_New[i][j];
                word[i][j] = gu32_LOGram_New[i][j];
            }
        }
    } else {
        if(gu32_LOGram_Old[0][0] == 0) {                        // 未满100个日志记录
            for(i = 0; i < gu16_AdressNum_temp_LOG; ++i) {      // 直接赋值完事。
                for(j = 0; j < LOG_WORD_WORD_NUM; ++j) {
                    word[i][j] = gu32_LOGram_New[i][j];
                }
            }
            return gu16_AdressNum_temp_LOG;

        } else {                                                // 满100个起步。有一部分在备份区。
            // for(i = 0; i < gu16_AdressNum_temp_LOG; ++i) {
            for(i = 0; i < LOG_PAGE_WORD_NUM; ++i) {            // 分两个部分获取
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


