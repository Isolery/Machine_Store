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
#warning "CH376.h：had Definition INT8U in other "
#endif

#ifndef   INT16U
#define   INT16U   unsigned  int  
#else 
#warning "CH376.h：had Definition INT8U in other "
#endif

#ifndef  INT32U
#define  INT32U  unsigned long int 
#else 
#warning "CH376.h：had Definition INT32U in other"
#endif

#ifndef  PINT8U
#define  PINT8U  unsigned char * 
#else 
#warning "CH376.h：had Definition INT32U in other"
#endif

/*
***********************************************************************
*
*                   命令代码
*
***********************************************************************
*/
#define	CMD_GET_IC_VER		    0x01			/* 获取芯片及固件版本 */
/* 输出: 版本号( 位7为1, 位6为0, 位5~位0为版本号 ) */
/*           CH376B返回版本号的值为0B7H即版本号为37H */

#define	CMD_ENTER_SLEEP		    0x03			/* 进入睡眠状态 */

#define	CMD_RESET_ALL		    0x05			/* 执行硬件复位 */

#define CMD_CHECK_EXIST         0x06      /* 用于测试通讯接口和工作状态 */

#define	CMD_SET_USB_MODE	    0x15			/* 设置USB工作模式 */
/* 输入: 模式代码 */
/*       00H=未启用的设备方式, 01H=已启用的设备方式并且使用外部固件模式, 02H=已启用的设备方式并且使用内置固件模式 */
/*       04H=未启用的主机方式, 05H=已启用的主机方式, 06H=已启用的主机方式并且自动产生SOF包, 07H=已启用的主机方式并且复位USB总线 */
/* 输出: 操作状态( CMD_RET_SUCCESS或CMD_RET_ABORT, 其它值说明操作未完成 ) */

#define CMD_GET_STATUS        0x22      /* 获取中断状态并取消中断请求 */

#define CMD_SET_FILE_NAME     0x2F      /* 设置将要操作的文件名*/

#define CMD_DISK_CONNECT      0x30      /*  产生中断并返回中断状态，检测到U盘产生中断*/

#define CMD_DISK_MOUNT        0x31      /*  初始化U盘并测试是否就绪，产生中断*/

#define CMD_FILE_OPEN         0x32      /* 打开文件或者目录，枚举文件和目录*/

#define CMD_FILE_CREATE       0x34      /*  创建文件 ，产生中断 */

#define CMD_FILE_CLOSE        0x36      /*  关闭当前已经打开的文件或目录*/

#define CMD_BYTE_WRITE        0x3C      /*  以字节为单位向当前位置写入数据*/

#define CMD_WR_REQ_DATA       0x2D      /*  向内部指定的缓冲区写入请求的数据块 */

#define CMD_BYTE_WR_GO        0x3D      /*  继续字节写，会将先前数据压入flash*/

#define CMD_BYTE_LOCATE       0x39      /*  以字节为单位移动当前指针      */

#define CMD_DIR_CREATE        0x40      /*  新建目录并打开或打开已存在的目录  */

#define CMD50_WRITE_VAR32        0x0D      /*    */
 
#define CMD_RET_SUCCESS       0x14      /*  SD卡或USB事务或传输操作或文件操作成功*/
#define USB_INT_CONNECT       0x15      /*  检测到USB设备连接成功 */
#define CMD_RET_USB_SUCCESS   0x51      /*  设置USB工作模式，可以不用检测 */

#define CMD_RD_USB_DATA0	  0x27			/* 从当前USB中断的端点缓冲区或者主机端点的接收缓冲区读取数据块 */

#define CMD_DIR_INFO_READ	  0x37               /* 主机文件模式: 读取文件的目录信息 */
/* 输入: 指定需要读取的目录信息结构在扇区内的索引号 */
/*           索引号范围为00H~0FH, 索引号0FFH则为当前已经打开的文件 */
/* 输出中断 */

#define CMD_DIR_INFO_SAVE	  0x38			/* 主机文件模式: 保存文件的目录信息 */
#define CMD_WR_OFS_DATA		  0x2E			/* 向内部缓冲区指定偏移地址写入数据块 */
#define ATTR_READ_ONLY        0x10
#define	ERR_MISS_FILE		0x42			/* 指定路径的文件没有找到,可能是文件名称错误 */

void  USARTx_Init (INT16U Baud);           /*  串口初始化   */
 void  xWriteCH376Cmd(INT8U mCmd);          /*  写命令       */
// void  xWriteCH376Data(INT8U mData);        /*  写数据       */

INT8U  xReadCH376Data(void);               /*  读数据       */
INT8U  CH376_UDisk_Init(void);             /*  U盘初始化    */
INT8U  CH376ByteLocate(INT32U liv_offset); /*  设置偏移量   */
INT8U	CH376FileOpen( PINT8U name );      /*  打开文件     */
INT8U  CH376CloseFile(INT8U lcv_data);     /*  关闭文件     */
INT8U  CH376WriteByte(PINT8U plcv_buff,INT16U lic_number); /* 按字节写 */
INT8U  CH376CreateFile(PINT8U name);        /*创建文件 */
INT8U  CH376CreateList( PINT8U plcv_name);/*  创建或者打开目录*/
void   CH376SetFileName( PINT8U name );   /* CH376设置将要操作的文件的文件名*/  
INT8U  Query376Interrupt( void );
INT8U	CH376WriteReqBlock( PINT8U buf );  
void xWriteCH376Cmd(INT8U mCmd);	
INT8U CH376_Init(void);
INT8U CH376FileCreate(PINT8U name);
INT8U CH376DiskMount(void);
INT8U	CH376ByteWrite( PINT8U buf, INT16U ReqCount, INT16U RealCount );

#endif 