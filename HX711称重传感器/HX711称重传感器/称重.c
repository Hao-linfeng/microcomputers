#include <reg51.h>
#include <intrins.h>	
#define uchar unsigned char
#define uint  unsigned int
/*12864液晶屏 端口定义*/
#define data  P0           //数据口
sbit   RS=P3^5;            //寄存器选择输入 
sbit   RW=P3^6;            //液晶读/写控制
sbit    E=P3^4;            //液晶使能控制
sbit  PSB=P3^7;            //串/并方式控制
sbit wela=P2^6;
sbit dula=P2^7;
/*HX711称重传感器 端口定义*/
unsigned long InitialWeight;
unsigned long FinalWeight;	
uchar string[]={"0000"};
sbit  HX711_SCK=P1^0;
sbit HX711_DOUT=P1^1;

uchar code dis1[] = {"重量为："};
uchar code dis2[] = {"g"};

//////////////////以下是12864驱动程序////////////////                                                              
//函数功能：延时函数
void delay(int ms)
{
    while(ms--)
	{
      uchar i;
	  for(i=0;i<250;i++)  
	   {
	    _nop_();			   
		_nop_();
		_nop_();
		_nop_();
	   }
	}
}

//函数功能：检查LCD忙状态
//lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。
bit busy()
 { 
    bit result;
    P0=0x00;              //关掉数码管的信号。阻止数码管受到P0口信号的影响
    RS=0;
    RW=1;
     E=1;
    delay(1);
    result = (bit)(P0&0x80);
     E=0;
    return(result); 
 }

//函数功能：写指令数据到LCD                                                  
//RS=L，RW=L，E=高脉冲，D0-D7=指令码。
void wcmd(uchar cmd)
{                       
   while(busy());
    RS=0;
    RW=0;
     E=0;
    _nop_();
    _nop_(); 
    P0=cmd;
    delay(1);
     E=1;
    delay(1);
     E=0;  
}
                                                               
//函数功能：写显示数据到LCD                                                  
//RS=H，RW=L，E=高脉冲，D0-D7=数据。                               
void wdat(uchar dat)
{                          
   while(busy());
    RS=1;
    RW=0;
     E=0;
    P0=dat;
    delay(1);
     E=1;
    delay(1);
     E=0; 
}
                                                             
//函数功能：LCD初始化设定                                                 
void Init_12864()
{ 
    PSB=1;         //并口方式
    
    wcmd(0x34);      //扩充指令操作
    delay(5);
    wcmd(0x30);      //基本指令操作
    delay(5);
    wcmd(0x0c);      //显示开，关光标
    delay(5);
    wcmd(0x01);      //清除LCD的显示内容
    delay(5);
}

//函数功能：设定显示位置                                          
void pos(uchar X,uchar Y)
{                          
   uchar  pos;
   if (X==0)
     {X=0x80;}
   else if (X==1)
     {X=0x90;}
   else if (X==2)
     {X=0x88;}
   else if (X==3)
     {X=0x98;}
   pos=X+Y ;  
   wcmd(pos);     //显示地址
}
//////////////////以上是12864驱动程序////////////////

//////////////////以下是HX711称重传感器驱动程序//////////////// 
//函数功能：延时函数
void delay_HX711(void)
{
	_nop_();
	_nop_();
}

//函数功能：读取HX711测量值
unsigned long HX711_Read(void)	//增益128
{
	unsigned long weight; 
	unsigned char i; 
  	HX711_DOUT=1; 
	delay_HX711();
  	HX711_SCK=0; 
  	weight=0;
	E=1; 
  	while(HX711_DOUT); 
	E=0;
  	for(i=0;i<24;i++)
	{ 
	  HX711_SCK=1; 
	  weight=weight<<1; 
	  HX711_SCK=0; 
	  if(HX711_DOUT)
		weight++; 
	} 
 	HX711_SCK=1; 
    weight=weight^0x800000;   //第25个脉冲下降沿来时，转换数据
	delay_HX711();
	HX711_SCK=0;  
	return(weight);
}

//函数功能：读取HX711重量初值 
void Get_InitialWeight()
{
  InitialWeight= HX711_Read();	
} 

//函数功能：测算最终物体重量
void Weight()
{		
	FinalWeight=HX711_Read();
	FinalWeight=FinalWeight-InitialWeight;
	FinalWeight=FinalWeight/430;
	string[0]=FinalWeight/1000+0x30;
	string[1]=FinalWeight%1000/100+0x30;
	string[2]=FinalWeight%100/10+0x30;
	string[3]=FinalWeight%10+0x30;
}

//函数功能：液晶屏显示字符	
void display()
{
	uchar i;
	wela=0;
	dula=0; 
	wcmd(0x01);      //清除LCD的显示内容
	delay(10);
	
	pos(0,0);
	for(i=0;i<=8;i++)
	{
	  wdat(dis1[i]);
	}
	
	pos(0,4);
	wdat(string[0]);
	wdat(string[1]);
	wdat(string[2]);
	wdat(string[3]);
	wdat(dis2[0]);
}

/*****************以下是主函数驱动程序***************/
void main()
{
	Get_InitialWeight();
	Init_12864();
	delay(100);
	while(1)
	{
	  Weight();
	  display();
	  delay(5);
	}
}

