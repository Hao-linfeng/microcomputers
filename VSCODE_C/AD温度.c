									  				   /********************************************

读取DS18B20温度，通过LCD1602显示出来
第一行: 实时温度值
第二行: 最大值和最小值
********************************************/



/*头文件*/
#include <reg52.h>      
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char

#define AddWr 0x90   //写数据地址 
#define AddRd 0x91   //读数据地址
unsigned char ReadADC(unsigned char Chl);	                         
bit ack;	              //应答标志位

sbit SDA=P2^0;
sbit SCL=P2^1;

sbit lcd_rs_port = P3^5;  /*定义LCD控制端口*/
sbit lcd_rw_port = P3^6;
sbit lcd_en_port = P3^4;
#define lcd_data_port P0

code unsigned int  vt_table[]=	// 电压温度对照表
	{
		4132,4098,4063,4026,3988,3949,3908,3866,3823,3779,
		3733,3686,3639,3590,3540,3489,3437,3385,3331,3277,
		3222,3166,3110,3054,2997,2940,2882,2824,2767,2709,
		2651,2593,2536,2478,2421,2365,2309,2253,2198,2143,
		2089,2036,1984,1932,1881,1831,1782,1734,1686,1640,
		1594,1550,1506,1464,1422,1381,1341,1303,1265,1228,
		1192
	
	};
////////////////////////////////////////
sbit WELA=P2^7; //数码管的位选信号
void delay1 (void)//关闭数码管延时程序
{
	int k;
	for (k=0; k<1000; k++);

}

void _Nop(void)//I2C延时程序
{
	int k;
	for (k=0; k<10; k++);

}

//////////////////////////////////////


/*------------------------------------------------
                    启动总线
------------------------------------------------*/
void Start_I2c()
{
  SDA=1;   //发送起始条件的数据信号
  _Nop();
  SCL=1;
  _Nop();    //起始条件建立时间大于4.7us,延时
  _Nop();
  _Nop();
  _Nop();
  _Nop();    
  SDA=0;     //发送起始信号
  _Nop();    //起始条件锁定时间大于4μ
  _Nop();
  _Nop();
  _Nop();
  _Nop();       
  SCL=0;    //钳住I2C总线，准备发送或接收数据
  _Nop();
  _Nop();
}
/*------------------------------------------------
                    结束总线
------------------------------------------------*/
void Stop_I2c()
{
  SDA=0;    //发送结束条件的数据信号
  _Nop();   //发送结束条件的时钟信号
  SCL=1;    //结束条件建立时间大于4μ
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  _Nop();
  SDA=1;    //发送I2C总线结束信号
  _Nop();
  _Nop();
  _Nop();
  _Nop();
}




/*----------------------------------------------------------------
                 字节数据传送函数               
函数原型: void  SendByte(unsigned char c);
功能:  将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
     此状态位进行操作.(不应答或非应答都使ack=0 假)     
     发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
------------------------------------------------------------------*/
void  SendByte(unsigned char c)
{
 unsigned char BitCnt;
 
 for(BitCnt=0;BitCnt<8;BitCnt++)  //要传送的数据长度为8位
    {
     if((c<<BitCnt)&0x80)SDA=1;   //判断发送位
       else  SDA=0;                
     _Nop();
     SCL=1;               //置时钟线为高，通知被控器开始接收数据位
      _Nop(); 
      _Nop();             //保证时钟高电平周期大于4μ
      _Nop();
      _Nop();
      _Nop();         
     SCL=0; 
    }
    
    _Nop();
    _Nop();
    SDA=1;               //8位发送完后释放数据线，准备接收应答位
    _Nop();
    _Nop();   
    SCL=1;
    _Nop();
    _Nop();
    _Nop();
    if(SDA==1)ack=0;     
       else ack=1;        //判断是否接收到应答信号
    SCL=0;
    _Nop();
    _Nop();
}







/*----------------------------------------------------------------
                 字节数据传送函数               
函数原型: unsigned char  RcvByte();
功能:  用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
     发完后请用应答函数。  
------------------------------------------------------------------*/	
unsigned char  RcvByte()
{
  unsigned char retc;
  unsigned char BitCnt;
  
  retc=0; 
  SDA=1;             //置数据线为输入方式
  for(BitCnt=0;BitCnt<8;BitCnt++)
      {
        _Nop();           
        SCL=0;       //置时钟线为低，准备接收数据位
        _Nop();
        _Nop();      //时钟低电平周期大于4.7us
        _Nop();
        _Nop();
        _Nop();
        SCL=1;       //置时钟线为高使数据线上数据有效
        _Nop();
        _Nop();
        retc=retc<<1;
        if(SDA==1)retc=retc+1; //读数据位,接收的数据位放入retc中
        _Nop();
        _Nop(); 
      }
  SCL=0;    
  _Nop();
  _Nop();
  return(retc);
}



/*----------------------------------------------------------------
                     应答子函数
原型:  void Ack_I2c(void);
 
----------------------------------------------------------------*/
/*void Ack_I2c(void)
{
  
  SDA=0;     
  _Nop();
  _Nop();
  _Nop();      
  SCL=1;
  _Nop();
  _Nop();              //时钟低电平周期大于4μ
  _Nop();
  _Nop();
  _Nop();  
  SCL=0;               //清时钟线，钳住I2C总线以便继续接收
  _Nop();
  _Nop();    
}*/
/*----------------------------------------------------------------
                     非应答子函数
原型:  void NoAck_I2c(void);
 
----------------------------------------------------------------*/
void NoAck_I2c(void)
{
  
  SDA=1;
  _Nop();
  _Nop();
  _Nop();      
  SCL=1;
  _Nop();
  _Nop();              //时钟低电平周期大于4μ
  _Nop();
  _Nop();
  _Nop();  
  SCL=0;                //清时钟线，钳住I2C总线以便继续接收
  _Nop();
  _Nop();    
}

/////////////////////////////////////


//////////////以下是LCD1602驱动程序////////////////

void lcd_delay(uchar ms) /*LCD1602 延时*/
{
    uchar j;
    while(ms--){
        for(j=0;j<250;j++)
            {;}
        }   
}


void lcd_busy_wait() /*LCD1602 忙等待*/
{
    lcd_rs_port = 0;
    lcd_rw_port = 1;
    lcd_en_port = 1;
    lcd_data_port = 0xff;
    while (lcd_data_port&0x80);
    lcd_en_port = 0; 

}


void lcd_command_write(uchar command) /*LCD1602 命令字写入*/
{
    lcd_busy_wait();
    lcd_rs_port = 0;
    lcd_rw_port = 0;
    lcd_en_port = 0;
    lcd_data_port = command;
    lcd_en_port = 1;
    lcd_en_port = 0;     
}



void lcd_system_reset() /*LCD1602 初始化*/
{
    lcd_delay(20);
    lcd_command_write(0x38);
    lcd_delay(100);
    lcd_command_write(0x38);
    lcd_delay(50);
    lcd_command_write(0x38);
    lcd_delay(10);
    lcd_command_write(0x08);
    lcd_command_write(0x01);
    lcd_command_write(0x06);
    lcd_command_write(0x0c); 
}



void lcd_char_write(uchar x_pos,y_pos,lcd_dat) /*LCD1602 字符写入*/
{
    x_pos &= 0x0f; /* X位置范围 0~15 */
    y_pos &= 0x01; /* Y位置范围 0~ 1 */
    if(y_pos==1) x_pos += 0x40;
    x_pos += 0x80;
    lcd_command_write(x_pos);
    lcd_busy_wait();
    lcd_rs_port = 1;
    lcd_rw_port = 0;
    lcd_en_port = 0;
    lcd_data_port = lcd_dat;
    lcd_en_port = 1;
    lcd_en_port = 0; 
}



void lcd_bad_check() /*LCD1602 坏点检查*/
{
    char i,j;
    for(i=0;i<2;i++){
        for(j=0;j<16;j++) {
            lcd_char_write(j,i,0xff);
            }
        }
    lcd_delay(200);
    lcd_delay(200);
	lcd_delay(200);
	lcd_delay(100);
	lcd_delay(200);
    lcd_command_write(0x01); /* clear lcd disp */
}
//////////////////以上是LCD1602驱动程序////////////////



//////////////////以下是DS18B20驱动程序////////////////

//////////////////以上是DS18B20驱动程序////////////////




/*定义数字ascii编码*/
unsigned char mun_char_table[]={"0123456789- "};
unsigned char temp_table[] ={"Temp:   'C"};
/*1MS为单位的延时程序*/
void delay_1ms(uchar x)
{
    uchar j;
    while(x--){
        for(j=0;j<125;j++)
            {;}
        }   
}

main()
{	
	unsigned int temp,i,num=0;
	lcd_system_reset(); /*LCD1602 初始化*/
////////////////////////////////////////////////////////////////
	P0=0X00;//关掉数码管的位选信号。阻止数码管受到P0口信号的影响。
	delay1();
	WELA=1;
	delay1();
	WELA=0;
////////////////////////////////////////////////////////////////
	lcd_bad_check(); /*LCD1602 坏点检查*/
	for (i=0;i<10;i++) lcd_char_write(i,0,temp_table[i]);  


	while(1)
	{
		num=ReadADC(0);//值取差值，用于显示光强越小，数值越小
		num=num*19;
		 for(i=0;i<61;i++)
   {
//判断采集电压值大于或者等于表格电压时，停下来并记录当前i值
    if(num>= vt_table[i])
       {
        temp=i;
//如果是前10个数据表明是负温度，需要把温度符号显示出来，并且前10位温度递增
        if(i<10) 
          {
          temp=10-temp;    //如果i=0，表示-10
        lcd_char_write(5,0,mun_char_table[10]);
          }
        else
          {
          temp-=10; //如果大于等于10表明正温度，把前10个表格负温度去掉
        lcd_char_write(5,0,mun_char_table[11]);
          }
		break;             //检测到即跳出循环，否则继续循环
        }
	  
      }
		lcd_char_write(6,0,mun_char_table[temp/10]);  /*把温度显示出来*/
		lcd_char_write(7,0,mun_char_table[temp%10]);

		delay_1ms(200);
	}
}
/*------------------------------------------------
             读AD转值程序
输入参数 Chl 表示需要转换的通道，范围从0-3
返回值范围0-255
------------------------------------------------*/
unsigned char ReadADC(unsigned char Chl)
 {
   unsigned char Val;
   Start_I2c();               //启动总线
   SendByte(AddWr);             //发送器件地址
     if(ack==0)return(0);
   SendByte(0x40|Chl);            //发送器件子地址
     if(ack==0)return(0);
   Start_I2c();
   SendByte(AddWr+1);
      if(ack==0)return(0);
   Val=RcvByte();
   NoAck_I2c();                 //发送非应位
   Stop_I2c();                  //结束总线
  return(Val);
 }
