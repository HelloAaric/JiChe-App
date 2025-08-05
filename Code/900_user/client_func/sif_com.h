#ifndef _SIF_COM_H
#define _SIF_COM_H

typedef struct {
    UINT8 u8_TxData[64];
    UINT8 u8_RxData[64];
    UINT8 TxLength;                         // 要发送数据长度
    UINT8 RxLength;                         // 要接收数据长度
    UINT8 RxTxJudge;                        // 1:要发送数据。0：接收数据
    UINT8 dealResult;                       // 处理结果，1：成功，0：失败。
    UINT8 sif_status;                       // 目前通讯状况，0：空闲，1：正在发送，2：正在接收
}SIF_FRAME;

extern SIF_FRAME gst_SifFrame;

void SIF_SendData_Handle(void);

#endif	/* _SIF_COM_H */

