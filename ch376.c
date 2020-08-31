//#define GLOBALS
#include "ch376.h"

#define  success   1
#define  fail      0


void USART_Flush( void )
{
	unsigned char dummy;
	while ( UCSR1A & (1<<RXC1) ) dummy = UDR1;
}
/*
*************************************************************
*      延时
*************************************************************
*/
void mDelaymS(unsigned int z)
{
    unsigned int x;
    for(x=z;x>0;x--)
	{
    
	}
}
/*
*************************************************************
*      CH376写命令
*************************************************************
*/
void xWriteCH376Cmd(INT8U mCmd)	
{
	/* 等待发送完成 */
    while(!(UCSR1A&(1<<UDRE1))); 
	/* 发送第一位同步码 */
    UDR1 = 0x57;

	/* 等待发送完成 */
	while(!(UCSR1A&(1<<UDRE1))); 

   /* 发送第二位同步码 */
    UDR1 = 0xAB;
	
   /* 等待发送完成 */
	while(!(UCSR1A&(1<<UDRE1))); 
	
	/* 串口输出*/
    UDR1 = mCmd;
	
}
/*
*************************************************************
*        CH376写数据
*************************************************************
*/
static  void xWriteCH376Data(INT8U mData)
{
	/* 等待发送完成 */
	while(!(UCSR1A&(1<<UDRE1))); 
	
	/* 发送数据 */
    UDR1 = mData;
	
}
/*
*************************************************************
*          CH376读取数据
*************************************************************
*/
INT8U  xReadCH376Data(void)
{
    INT32U  liv_count;

    for (liv_count =500000; liv_count > 0; liv_count--)//修改了数字
	{
	      if (UCSR1A &(1 << RXC1))
		  {
		       return UDR1;
		  }					
	}
		
    return(0x00); /* 超时 */
}
/*
*************************************************************
*         以字节为单位移动当前指针
*************************************************************
*/
INT8U	CH376ByteLocate(INT32U liv_offset) 
{
    INT8U lcv_data;
	
    xWriteCH376Cmd(CMD_BYTE_LOCATE);
	
    xWriteCH376Data((INT8U)liv_offset);
    xWriteCH376Data((INT8U)(liv_offset>>8));
    xWriteCH376Data((INT8U)(liv_offset>>16));
    xWriteCH376Data((INT8U)(liv_offset>>24));
	
	lcv_data  = xReadCH376Data();
    if (lcv_data == 0x14 )
    {
        return success;
    }
	return fail;
}
/*
*************************************************************
*             U盘初始化
*************************************************************
*/
INT8U CH376_UDisk_Init(void)
{
    //INT16U  liv_delay;
	INT16U  liv_count;
	INT8U   lcv_data;
	
    for(liv_count=0;liv_count<10;liv_count++)
    {
	    xWriteCH376Cmd(CMD_DISK_MOUNT);
        //for(liv_delay=0;liv_delay<3000;liv_delay++);/* 延时，可用其他替代 */
		//for(liv_delay=0;liv_delay<50000;liv_delay++);

		lcv_data = xReadCH376Data();

		if(lcv_data ==0x14)
		{
		    return success;
		}		
	}
	return fail;									 
}

/*
*************************************************************
*               CH376设置将要操作的文件的文件名
*************************************************************
*/
void	CH376SetFileName( PINT8U name )  
{
/*	UINT8	i;*/
	INT8U	c;
	xWriteCH376Cmd( CMD_SET_FILE_NAME );
/*	for ( i = MAX_FILE_NAME_LEN; i != 0; -- i ) {
		c = *name;
		xWriteCH376Data( c );
		if ( c == 0 ) break;
		name ++;
	}*/
	c = *name;
	xWriteCH376Data( c );
	while ( c ) {
		name ++;
		c = *name;
		//if ( c == DEF_SEPAR_CHAR1 || c == DEF_SEPAR_CHAR2 ) c = 0;  /* 强行将文件名截止 */
		xWriteCH376Data( c );
	}
	//xEndCH376Cmd( );
}
/*
*************************************************************
*               CH376打开文件 
*             在根目录或者当前目录下打开文件或者目录（文件夹）          
*************************************************************
*/
INT8U	CH376FileOpen( PINT8U name )  /* 在根目录或者当前目录下打开文件或者目录(文件夹) */
{
	 //INT16U  liv_delay;

	 INT8U   lcv_data;
	
	 CH376SetFileName( name );  /* 设置将要操作的文件的文件名 */
	 
	 USART_Flush();//清空缓存
	 
	 xWriteCH376Cmd( CMD_FILE_OPEN ) ;

	 while(Query376Interrupt( ));
	 
	 lcv_data  = xReadCH376Data();
	 	
	 return lcv_data;

}
/*
*************************************************************
*               CH376关闭文件 
*          0 禁止更新长度,1 自动更新长度                  
*************************************************************
*/
INT8U CH376CloseFile(INT8U lcv_data)
{
	  xWriteCH376Cmd(CMD_FILE_CLOSE);
	  xWriteCH376Data( 1 );
	  
	  xWriteCH376Data(lcv_data);
	   
	 USART_Flush();
	 
	 lcv_data  = xReadCH376Data();
	 
	 if(lcv_data == 0x14 )
	 {
		 return success;
	 }
	 return fail;
}
/*
*************************************************************
*          CH376按字节写数据
*************************************************************
*/

INT8U	CH376ByteWrite( PINT8U buf, INT16U ReqCount, INT16U RealCount )  /* 以字节为单位向当前位置写入数据块 */
{
	INT8U	s;
	xWriteCH376Cmd( 0x3C );
	xWriteCH376Data( (INT8U)ReqCount );
	xWriteCH376Data( (INT8U)(ReqCount>>8) );
	//xEndCH376Cmd( );
	//if ( RealCount ) *RealCount = 0;
	USART_Flush();
	
	while ( 1 ) {
		s = xReadCH376Data( );
		if ( s == 0x1E ) {
			s = CH376WriteReqBlock( buf );  /* 向内部指定缓冲区写入请求的数据块,返回长度 */
			xWriteCH376Cmd( 0x3D );
			//xEndCH376Cmd( );
			buf += s;
		//	if ( RealCount ) *RealCount += s;
		}
/*		else if ( s == USB_INT_SUCCESS ) return( s );*/  /* 结束 */
		else return( s );  /* 错误 */
	}
}

/*
*************************************************************
*          CH376创建文件
*        在根目录或当前目录下新建文件，如果文件
*        已经存在那么先删除
*************************************************************
*/
INT8U CH376FileCreate(PINT8U name)
{
 	 INT16U  liv_delay;

	 INT8U   lcv_data;
 	  
	 CH376SetFileName( name );  /* 设置将要操作的文件的文件名 */

	 xWriteCH376Cmd( CMD_FILE_CREATE ) ;
	 
	 for(liv_delay=0;liv_delay<5000;liv_delay++);
	 
	 USART_Flush();
	 
	 lcv_data  = xReadCH376Data();
	 
	 if(lcv_data == 0x14 )
	 {
		 return success;
	 }
	 return fail;
}

/*
*************************************************************
*          CH376创建目录
*        在根目录或当前目录下新建文件，如果文件
*        已经存在那么直接打开
*************************************************************
*/
INT8U  CH376CreateList( PINT8U plcv_name)
{
    INT8U lcv_data;
	
    /* 设置文件命令码 */
    xWriteCH376Cmd(CMD_SET_FILE_NAME);
	  
	/* 写入文件名 */  
    while ((*plcv_name) != '\0')
    {
        xWriteCH376Data(*plcv_name);
	    plcv_name++;
	}
	xWriteCH376Data(0x00); /* 结束符 */
		
    xWriteCH376Cmd(CMD_DIR_CREATE);
	
	USART_Flush();//清空缓存
		
    lcv_data  = xReadCH376Data();
    if (lcv_data == 0x14 )
    {
        return success;
    }
    return fail;	
}
/*
*************************************************************
*          CH376初始化
*************************************************************
*/
INT8U CH376_Init(void)
{
    INT8U   lcv_data,lcv_countflag;
	INT16U  liv_count;
	
	USARTx_Init (9600);
	
    for (lcv_countflag = 0 ; lcv_countflag < 10; lcv_countflag++)
	{
	      /* 测试通讯接口和工作状态*/
        xWriteCH376Cmd(CMD_CHECK_EXIST);
		
	    xWriteCH376Data(0x77);	   
		
        lcv_data = xReadCH376Data();
	
	    if (lcv_data == 0x88)
	    {
            /* 工作模式设置，U盘模式06*/
            xWriteCH376Cmd(CMD_SET_USB_MODE);
			
            xWriteCH376Data(0x06);
			
            lcv_data = xReadCH376Data();
			
	        if (lcv_data == CMD_RET_USB_SUCCESS)
		    { 
		       break;
			}   
		}
		for(liv_count=0;liv_count<50000;liv_count++); /*延时*/
	}
	
	if (lcv_countflag < 10)
	{
	    return success;
	}
	else 
	{
        return fail;
	}
}
/*
*************************************************************
*          CH376相关硬件串口初始化
*************************************************************
*/

void USARTx_Init (INT16U Baud)
{
    UCSR1B = 0x00;
   
    /* 设置波特率9600*/
    UBRR1H = (INT8U) ((Fosc/Baud/16-1) >>8);
	UBRR1L = (INT8U) (Fosc/Baud/16-1);
	
    /*正常倍速，单机模式*/
    UCSR1A = 0x00;   

    /* 接收器与发送器使能，发送和接收都采用查询方式*/
    UCSR1B = 0x1c;

    /* 设置帧格式: 8个数据位, 1个停止位 */
    UCSR1C = 0x06;
}

/*
*************************************************************
*          向内部指定缓冲区写入请求的数据块
*************************************************************
*/

INT8U	CH376WriteReqBlock( PINT8U buf )  
{
	INT8U	s, l;
	xWriteCH376Cmd( 0x2D );
	s = l = xReadCH376Data( );  /* 长度 */
	if ( l ) {
		do {
			xWriteCH376Data( *buf );
			buf ++;
		} while ( -- l );
	}
	//xEndCH376Cmd( );
	return( s );
}

/*
*************************************************************
*          从当前主机端点的接收缓冲区读取数据块,返回长度
*************************************************************
*/

INT8U	CH376ReadBlock( PINT8U buf )  
{
	INT8U	s, l;
	
	xWriteCH376Cmd( CMD_RD_USB_DATA0 );
	
	s = l = xReadCH376Data( );  /* 长度 */
	
	if ( l ) {
		do {
		
			*buf = xReadCH376Data( );
			
			buf ++;
			
		} while ( -- l );
	}
	
	//xEndCH376Cmd( );
	
	return( s );
}

/*
*************************************************************

*          读取当前文件的目录信息

*************************************************************
*/
INT8U	CH376DirInfoRead( void )  
{
    INT8U lcv_data;
	
	xWriteCH376Cmd( CMD_DIR_INFO_READ );
	
	xWriteCH376Data( 0xFF );
	
	lcv_data  = xReadCH376Data();
	
    if (lcv_data == 0x14 )
    {
        return success;
    }
    return fail;	
}


/*
*************************************************************

*           保存文件的目录信息

*************************************************************
*/
INT8U	CH376DirInfoSave( void )  
{
    INT8U lcv_data;
	
	xWriteCH376Cmd( CMD_DIR_INFO_SAVE );
	
	lcv_data  = xReadCH376Data();
	
    if (lcv_data == 0x14 )
    {
        return success;
    }
    return fail;	
}

/*
*************************************************************

*            向内部缓冲区指定偏移地址写入数据块 

*************************************************************
*/
INT8U	CH376WriteOfsBlock( PINT8U buf, INT8U ofs, INT8U len )  
{
	xWriteCH376Cmd( CMD_WR_OFS_DATA );
	
	xWriteCH376Data( ofs );  /* 偏移地址 */

	xWriteCH376Data( len );  /* 长度 */
	
	if ( len ) {
		do {
			xWriteCH376Data( *buf );
			buf ++;
		} while ( -- len );
	}
	
	xWriteCH376Data(0x00); /* 结束符 */
	return 0;
}


INT8U CH376DiskMount(void)
{
    INT8U lcv_data;

    xWriteCH376Cmd(CMD_DISK_MOUNT);
	
	USART_Flush();//清空缓存
		
    lcv_data  = xReadCH376Data();
	
    if (lcv_data == 0x14 )
    {
        return success;
    }
    return fail;	
}


INT8U CH376DiskConnect(void)
{
 	INT8U lcv_data;

	USART_Flush();//清空缓存
	
    xWriteCH376Cmd(CMD_DISK_CONNECT);
	
	while(Query376Interrupt());
		
    lcv_data  = xReadCH376Data();
	
	if (lcv_data == 0x14 || lcv_data == 0x15)
    {
        return success;
    }
	if (lcv_data == 0xb4 )
    {
         CH376_Init();
		 return success;
    }
    return fail;	

}


 /************************************************************
函数名称: INT8U Query376Interrupt( void ) 
入口参数: 无
功    能:  查询CH376中断(INT#低电平)
*************************************************************/

INT8U Query376Interrupt( void ) 
{ 
int s = UCSR1A & 0x80 ;
 if ( !s) ////串口接收标志，串口接收到数据 
{ 
//RI = 0; //清串口标志 
//USART_Flush();//清空缓存
return( 1 ); 
} 
else return( 0 ); 

} 
