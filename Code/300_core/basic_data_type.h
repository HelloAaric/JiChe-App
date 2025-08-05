/*============================================================================*/
/**
*         Copyright(c) 2010--2017 Shenzhen Klclear Technology Co.,Ltd
*                          ALL RIGHTS RESERVED
*
*   PRODUCT : BMAP (Battery Management Application Platform)
*
*   @file       basic_data_type.h
*   @brief      定义基本数据类型
*   @author     FanXiansheng
*   @date       2017-12-05 08:32
*   @version    V1001
*
*
*   @par 历史记录
*
*   Version |    Date    |    Author    | Modification
*   --------|------------|--------------|--------------
*   V1000   | 2016-10-21 | FanXiansheng | Created
*   V1001   | 2017-11-22 | SuXiaohong   | 修改接口
*
**/
/*============================================================================*/



#ifndef BASIC_DATA_TYPE_H
#define BASIC_DATA_TYPE_H


/**
 *  @defgroup  BASIC_DATA_TYPE 基本数据类型
 *  @ingroup BASE
 *  @{
 */

/**************************************************************************
 *                               Include Files                            *
 **************************************************************************/



/**************************************************************************
 *                           Macros/Defines/Structures                    *
 **************************************************************************/

#include <stdint.h>

typedef char				                        CHAR;
typedef signed char			                        SCHAR;
typedef unsigned char		                        UCHAR;


typedef signed char			                        INT8;
typedef signed short int	                        INT16;
typedef signed int			                        INT32;

typedef signed char			                        SINT8;
typedef signed short int	                        SINT16;
typedef signed int			                        SINT32;

typedef unsigned char		                        UINT8;
typedef unsigned short int	                        UINT16;
typedef unsigned int		                        UINT32;

typedef signed __int64		                        INT64;
typedef signed __int64		                        SINT64;
typedef unsigned __int64	                        UINT64;

typedef UINT8		                                BOOL;

typedef UINT8		                                BYTE;
typedef UINT16		                                WORD;
typedef UINT32		                                DWORD;

typedef float		                                FLOAT32;
typedef double		                                FLOAT64;

#ifndef NULL
#define NULL		                                0
#endif


#ifndef TRUE
#define TRUE		                                1
#endif

#ifndef FALSE
#define FALSE		                                0
#endif

#ifndef BMS_OK
#define BMS_OK		                                0
#define BMS_ERROR	                                (~0)
#endif


#define BMS_DISABLE		                            0
#define BMS_ENABLE		                            1

#define BMS_LOW			                            0x00
#define BMS_HIGH		                            0x01

#define BMS_OFF			                            0x00
#define BMS_ON			                            0x01

#define __at(_addr)                                 __attribute__ ((at(_addr)))




typedef enum _IO_STATUS {
OPEN = 1, CLOSE = 0
}IO_STATUS;


//typedef unsigned char   bool;                     /* for use with TRUE/FALSE        */


/**************************************************************************
 *                             Declare Functions                          *
 **************************************************************************/
#ifndef PRINT_DEBUG_ENABLE
#define PRINT_DEBUG_ENABLE		                    1		/* 打印调试信息 */
#endif

#ifndef PRINT_ERR_ENABLE
#define PRINT_ERR_ENABLE			                1 	    /* 打印错误信息 */
#endif

#ifndef PRINT_INFO_ENABLE
#define PRINT_INFO_ENABLE			                1		/* 打印个人信息 */
#endif

#ifndef PRINT_CLEAR_STRING_ENABLE
#define PRINT_CLEAR_STRING_ENABLE	                1		/* 纯打印，没表头 */
#endif

#ifndef PRINT_AFE
#define PRINT_AFE	                                0		/* AFE或者他人使用 */
#endif

 #if PRINT_DEBUG_ENABLE
 #define PRINT_DEBUG(fmt, args...)                  do{(printf("\r\n[DEBUG] >> "), printf(fmt, ##args));}while(0)     
 #else
 #define PRINT_DEBUG(fmt, args...)	     
 #endif
 
 #if PRINT_ERR_ENABLE
 #define PRINT_ERR(fmt, args...) 	                do{(printf("\r\n[ERROR] >> "), printf(fmt, ##args));}while(0)     
 #else
 #define PRINT_ERR(fmt, args...)	       
 #endif
 
 #if PRINT_INFO_ENABLE
 #define PRINT_INFO(fmt, args...) 	                do{(printf("\r\n[INFO] >> "), printf(fmt, ##args));}while(0)     
 #else
 #define PRINT_INFO(fmt, args...)	       
 #endif

#if PRINT_CLEAR_STRING_ENABLE
#define PRINT_CLEAR_STRING(fmt) 	                printf("%s",fmt);   
#else
#define PRINT_CLEAR_STRING(fmt, args...)	       
#endif

#if PRINT_AFE
#define PRINT_AFE_INFO(fmt, args...) 	            do{(printf("\r\n[AFE_INFO] >> "), printf(fmt, ##args));}while(0)    
#else
#define PRINT_AFE_INFO(fmt, args...)	       
#endif


#define UPLIMIT_TO_ZERO(Var,Max,Min)                {(Var)=((Var)>=(Max))?(0):(Var);(Var)=((Var)<=(Min))?(0):(Var);}     
#define UPDNLMT16(Var,Max,Min)	                    {(Var)=((Var)>=(Max))?(Max):(Var);(Var)=((Var)<=(Min))?(Min):(Var);}
#define S2U(x)                                      (*((volatile UINT16*)(&(x))))

#endif //BASIC_DATA_TYPE_H


/*================================ END OF FILE ===============================*/
