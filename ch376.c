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
*      ��ʱ
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
*      CH376д����
*************************************************************
*/
void xWriteCH376Cmd(INT8U mCmd)	
{
	/* �ȴ�������� */
    while(!(UCSR1A&(1<<UDRE1))); 
	/* ���͵�һλͬ���� */
    UDR1 = 0x57;

	/* �ȴ�������� */
	while(!(UCSR1A&(1<<UDRE1))); 

   /* ���͵ڶ�λͬ���� */
    UDR1 = 0xAB;
	
   /* �ȴ�������� */
	while(!(UCSR1A&(1<<UDRE1))); 
	
	/* �������*/
    UDR1 = mCmd;
	
}
/*
*************************************************************
*        CH376д����
*************************************************************
*/
static  void xWriteCH376Data(INT8U mData)
{
	/* �ȴ�������� */
	while(!(UCSR1A&(1<<UDRE1))); 
	
	/* �������� */
    UDR1 = mData;
	
}
/*
*************************************************************
*          CH376��ȡ����
*************************************************************
*/
INT8U  xReadCH376Data(void)
{
    INT32U  liv_count;

    for (liv_count =500000; liv_count > 0; liv_count--)//�޸�������
	{
	      if (UCSR1A &(1 << RXC1))
		  {
		       return UDR1;
		  }					
	}
		
    return(0x00); /* ��ʱ */
}
/*
*************************************************************
*         ���ֽ�Ϊ��λ�ƶ���ǰָ��
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
*             U�̳�ʼ��
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
        //for(liv_delay=0;liv_delay<3000;liv_delay++);/* ��ʱ������������� */
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
*               CH376���ý�Ҫ�������ļ����ļ���
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
		//if ( c == DEF_SEPAR_CHAR1 || c == DEF_SEPAR_CHAR2 ) c = 0;  /* ǿ�н��ļ�����ֹ */
		xWriteCH376Data( c );
	}
	//xEndCH376Cmd( );
}
/*
*************************************************************
*               CH376���ļ� 
*             �ڸ�Ŀ¼���ߵ�ǰĿ¼�´��ļ�����Ŀ¼���ļ��У�          
*************************************************************
*/
INT8U	CH376FileOpen( PINT8U name )  /* �ڸ�Ŀ¼���ߵ�ǰĿ¼�´��ļ�����Ŀ¼(�ļ���) */
{
	 //INT16U  liv_delay;

	 INT8U   lcv_data;
	
	 CH376SetFileName( name );  /* ���ý�Ҫ�������ļ����ļ��� */
	 
	 USART_Flush();//��ջ���
	 
	 xWriteCH376Cmd( CMD_FILE_OPEN ) ;

	 while(Query376Interrupt( ));
	 
	 lcv_data  = xReadCH376Data();
	 	
	 return lcv_data;

}
/*
*************************************************************
*               CH376�ر��ļ� 
*          0 ��ֹ���³���,1 �Զ����³���                  
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
*          CH376���ֽ�д����
*************************************************************
*/

INT8U	CH376ByteWrite( PINT8U buf, INT16U ReqCount, INT16U RealCount )  /* ���ֽ�Ϊ��λ��ǰλ��д�����ݿ� */
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
			s = CH376WriteReqBlock( buf );  /* ���ڲ�ָ��������д����������ݿ�,���س��� */
			xWriteCH376Cmd( 0x3D );
			//xEndCH376Cmd( );
			buf += s;
		//	if ( RealCount ) *RealCount += s;
		}
/*		else if ( s == USB_INT_SUCCESS ) return( s );*/  /* ���� */
		else return( s );  /* ���� */
	}
}

/*
*************************************************************
*          CH376�����ļ�
*        �ڸ�Ŀ¼��ǰĿ¼���½��ļ�������ļ�
*        �Ѿ�������ô��ɾ��
*************************************************************
*/
INT8U CH376FileCreate(PINT8U name)
{
 	 INT16U  liv_delay;

	 INT8U   lcv_data;
 	  
	 CH376SetFileName( name );  /* ���ý�Ҫ�������ļ����ļ��� */

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
*          CH376����Ŀ¼
*        �ڸ�Ŀ¼��ǰĿ¼���½��ļ�������ļ�
*        �Ѿ�������ôֱ�Ӵ�
*************************************************************
*/
INT8U  CH376CreateList( PINT8U plcv_name)
{
    INT8U lcv_data;
	
    /* �����ļ������� */
    xWriteCH376Cmd(CMD_SET_FILE_NAME);
	  
	/* д���ļ��� */  
    while ((*plcv_name) != '\0')
    {
        xWriteCH376Data(*plcv_name);
	    plcv_name++;
	}
	xWriteCH376Data(0x00); /* ������ */
		
    xWriteCH376Cmd(CMD_DIR_CREATE);
	
	USART_Flush();//��ջ���
		
    lcv_data  = xReadCH376Data();
    if (lcv_data == 0x14 )
    {
        return success;
    }
    return fail;	
}
/*
*************************************************************
*          CH376��ʼ��
*************************************************************
*/
INT8U CH376_Init(void)
{
    INT8U   lcv_data,lcv_countflag;
	INT16U  liv_count;
	
	USARTx_Init (9600);
	
    for (lcv_countflag = 0 ; lcv_countflag < 10; lcv_countflag++)
	{
	      /* ����ͨѶ�ӿں͹���״̬*/
        xWriteCH376Cmd(CMD_CHECK_EXIST);
		
	    xWriteCH376Data(0x77);	   
		
        lcv_data = xReadCH376Data();
	
	    if (lcv_data == 0x88)
	    {
            /* ����ģʽ���ã�U��ģʽ06*/
            xWriteCH376Cmd(CMD_SET_USB_MODE);
			
            xWriteCH376Data(0x06);
			
            lcv_data = xReadCH376Data();
			
	        if (lcv_data == CMD_RET_USB_SUCCESS)
		    { 
		       break;
			}   
		}
		for(liv_count=0;liv_count<50000;liv_count++); /*��ʱ*/
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
*          CH376���Ӳ�����ڳ�ʼ��
*************************************************************
*/

void USARTx_Init (INT16U Baud)
{
    UCSR1B = 0x00;
   
    /* ���ò�����9600*/
    UBRR1H = (INT8U) ((Fosc/Baud/16-1) >>8);
	UBRR1L = (INT8U) (Fosc/Baud/16-1);
	
    /*�������٣�����ģʽ*/
    UCSR1A = 0x00;   

    /* �������뷢����ʹ�ܣ����ͺͽ��ն����ò�ѯ��ʽ*/
    UCSR1B = 0x1c;

    /* ����֡��ʽ: 8������λ, 1��ֹͣλ */
    UCSR1C = 0x06;
}

/*
*************************************************************
*          ���ڲ�ָ��������д����������ݿ�
*************************************************************
*/

INT8U	CH376WriteReqBlock( PINT8U buf )  
{
	INT8U	s, l;
	xWriteCH376Cmd( 0x2D );
	s = l = xReadCH376Data( );  /* ���� */
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
*          �ӵ�ǰ�����˵�Ľ��ջ�������ȡ���ݿ�,���س���
*************************************************************
*/

INT8U	CH376ReadBlock( PINT8U buf )  
{
	INT8U	s, l;
	
	xWriteCH376Cmd( CMD_RD_USB_DATA0 );
	
	s = l = xReadCH376Data( );  /* ���� */
	
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

*          ��ȡ��ǰ�ļ���Ŀ¼��Ϣ

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

*           �����ļ���Ŀ¼��Ϣ

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

*            ���ڲ�������ָ��ƫ�Ƶ�ַд�����ݿ� 

*************************************************************
*/
INT8U	CH376WriteOfsBlock( PINT8U buf, INT8U ofs, INT8U len )  
{
	xWriteCH376Cmd( CMD_WR_OFS_DATA );
	
	xWriteCH376Data( ofs );  /* ƫ�Ƶ�ַ */

	xWriteCH376Data( len );  /* ���� */
	
	if ( len ) {
		do {
			xWriteCH376Data( *buf );
			buf ++;
		} while ( -- len );
	}
	
	xWriteCH376Data(0x00); /* ������ */
	return 0;
}


INT8U CH376DiskMount(void)
{
    INT8U lcv_data;

    xWriteCH376Cmd(CMD_DISK_MOUNT);
	
	USART_Flush();//��ջ���
		
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

	USART_Flush();//��ջ���
	
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
��������: INT8U Query376Interrupt( void ) 
��ڲ���: ��
��    ��:  ��ѯCH376�ж�(INT#�͵�ƽ)
*************************************************************/

INT8U Query376Interrupt( void ) 
{ 
int s = UCSR1A & 0x80 ;
 if ( !s) ////���ڽ��ձ�־�����ڽ��յ����� 
{ 
//RI = 0; //�崮�ڱ�־ 
//USART_Flush();//��ջ���
return( 1 ); 
} 
else return( 0 ); 

} 
