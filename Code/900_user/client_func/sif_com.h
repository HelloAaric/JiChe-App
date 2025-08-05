#ifndef _SIF_COM_H
#define _SIF_COM_H

typedef struct {
    UINT8 u8_TxData[64];
    UINT8 u8_RxData[64];
    UINT8 TxLength;                         // Ҫ�������ݳ���
    UINT8 RxLength;                         // Ҫ�������ݳ���
    UINT8 RxTxJudge;                        // 1:Ҫ�������ݡ�0����������
    UINT8 dealResult;                       // ��������1���ɹ���0��ʧ�ܡ�
    UINT8 sif_status;                       // ĿǰͨѶ״����0�����У�1�����ڷ��ͣ�2�����ڽ���
}SIF_FRAME;

extern SIF_FRAME gst_SifFrame;

void SIF_SendData_Handle(void);

#endif	/* _SIF_COM_H */

