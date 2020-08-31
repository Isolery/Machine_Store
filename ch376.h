#ifndef __CH376_H
#define	__CH376_H

#include <avr/io.h>
#define  Fosc  8000000


#ifdef   GLOBALS
#define  EXT
#else
#define  EXT  extern
#endif

#ifndef		NULL
#define		NULL	0
#endif

#ifndef  INT8U
#define  INT8U  unsigned char 
#else 
#warning "CH376.h��had Definition INT8U in other "
#endif

#ifndef   INT16U
#define   INT16U   unsigned  int  
#else 
#warning "CH376.h��had Definition INT8U in other "
#endif

#ifndef  INT32U
#define  INT32U  unsigned long int 
#else 
#warning "CH376.h��had Definition INT32U in other"
#endif

#ifndef  PINT8U
#define  PINT8U  unsigned char * 
#else 
#warning "CH376.h��had Definition INT32U in other"
#endif

/*
***********************************************************************
*
*                   �������
*
***********************************************************************
*/
#define	CMD_GET_IC_VER		    0x01			/* ��ȡоƬ���̼��汾 */
/* ���: �汾��( λ7Ϊ1, λ6Ϊ0, λ5~λ0Ϊ�汾�� ) */
/*           CH376B���ذ汾�ŵ�ֵΪ0B7H���汾��Ϊ37H */

#define	CMD_ENTER_SLEEP		    0x03			/* ����˯��״̬ */

#define	CMD_RESET_ALL		    0x05			/* ִ��Ӳ����λ */

#define CMD_CHECK_EXIST         0x06      /* ���ڲ���ͨѶ�ӿں͹���״̬ */

#define	CMD_SET_USB_MODE	    0x15			/* ����USB����ģʽ */
/* ����: ģʽ���� */
/*       00H=δ���õ��豸��ʽ, 01H=�����õ��豸��ʽ����ʹ���ⲿ�̼�ģʽ, 02H=�����õ��豸��ʽ����ʹ�����ù̼�ģʽ */
/*       04H=δ���õ�������ʽ, 05H=�����õ�������ʽ, 06H=�����õ�������ʽ�����Զ�����SOF��, 07H=�����õ�������ʽ���Ҹ�λUSB���� */
/* ���: ����״̬( CMD_RET_SUCCESS��CMD_RET_ABORT, ����ֵ˵������δ��� ) */

#define CMD_GET_STATUS        0x22      /* ��ȡ�ж�״̬��ȡ���ж����� */

#define CMD_SET_FILE_NAME     0x2F      /* ���ý�Ҫ�������ļ���*/

#define CMD_DISK_CONNECT      0x30      /*  �����жϲ������ж�״̬����⵽U�̲����ж�*/

#define CMD_DISK_MOUNT        0x31      /*  ��ʼ��U�̲������Ƿ�����������ж�*/

#define CMD_FILE_OPEN         0x32      /* ���ļ�����Ŀ¼��ö���ļ���Ŀ¼*/

#define CMD_FILE_CREATE       0x34      /*  �����ļ� �������ж� */

#define CMD_FILE_CLOSE        0x36      /*  �رյ�ǰ�Ѿ��򿪵��ļ���Ŀ¼*/

#define CMD_BYTE_WRITE        0x3C      /*  ���ֽ�Ϊ��λ��ǰλ��д������*/

#define CMD_WR_REQ_DATA       0x2D      /*  ���ڲ�ָ���Ļ�����д����������ݿ� */

#define CMD_BYTE_WR_GO        0x3D      /*  �����ֽ�д���Ὣ��ǰ����ѹ��flash*/

#define CMD_BYTE_LOCATE       0x39      /*  ���ֽ�Ϊ��λ�ƶ���ǰָ��      */

#define CMD_DIR_CREATE        0x40      /*  �½�Ŀ¼���򿪻���Ѵ��ڵ�Ŀ¼  */

#define CMD50_WRITE_VAR32        0x0D      /*    */
 
#define CMD_RET_SUCCESS       0x14      /*  SD����USB�������������ļ������ɹ�*/
#define USB_INT_CONNECT       0x15      /*  ��⵽USB�豸���ӳɹ� */
#define CMD_RET_USB_SUCCESS   0x51      /*  ����USB����ģʽ�����Բ��ü�� */

#define CMD_RD_USB_DATA0	  0x27			/* �ӵ�ǰUSB�жϵĶ˵㻺�������������˵�Ľ��ջ�������ȡ���ݿ� */

#define CMD_DIR_INFO_READ	  0x37               /* �����ļ�ģʽ: ��ȡ�ļ���Ŀ¼��Ϣ */
/* ����: ָ����Ҫ��ȡ��Ŀ¼��Ϣ�ṹ�������ڵ������� */
/*           �����ŷ�ΧΪ00H~0FH, ������0FFH��Ϊ��ǰ�Ѿ��򿪵��ļ� */
/* ����ж� */

#define CMD_DIR_INFO_SAVE	  0x38			/* �����ļ�ģʽ: �����ļ���Ŀ¼��Ϣ */
#define CMD_WR_OFS_DATA		  0x2E			/* ���ڲ�������ָ��ƫ�Ƶ�ַд�����ݿ� */
#define ATTR_READ_ONLY        0x10
#define	ERR_MISS_FILE		0x42			/* ָ��·�����ļ�û���ҵ�,�������ļ����ƴ��� */

void  USARTx_Init (INT16U Baud);           /*  ���ڳ�ʼ��   */
 void  xWriteCH376Cmd(INT8U mCmd);          /*  д����       */
// void  xWriteCH376Data(INT8U mData);        /*  д����       */

INT8U  xReadCH376Data(void);               /*  ������       */
INT8U  CH376_UDisk_Init(void);             /*  U�̳�ʼ��    */
INT8U  CH376ByteLocate(INT32U liv_offset); /*  ����ƫ����   */
INT8U	CH376FileOpen( PINT8U name );      /*  ���ļ�     */
INT8U  CH376CloseFile(INT8U lcv_data);     /*  �ر��ļ�     */
INT8U  CH376WriteByte(PINT8U plcv_buff,INT16U lic_number); /* ���ֽ�д */
INT8U  CH376CreateFile(PINT8U name);        /*�����ļ� */
INT8U  CH376CreateList( PINT8U plcv_name);/*  �������ߴ�Ŀ¼*/
void   CH376SetFileName( PINT8U name );   /* CH376���ý�Ҫ�������ļ����ļ���*/  
INT8U  Query376Interrupt( void );
INT8U	CH376WriteReqBlock( PINT8U buf );  
void xWriteCH376Cmd(INT8U mCmd);	
INT8U CH376_Init(void);
INT8U CH376FileCreate(PINT8U name);
INT8U CH376DiskMount(void);
INT8U	CH376ByteWrite( PINT8U buf, INT16U ReqCount, INT16U RealCount );

#endif 