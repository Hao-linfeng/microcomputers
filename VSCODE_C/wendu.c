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


/*定义数字ascii编码*/
unsigned char mun_char_table[]={"0123456789abcdef"};
unsigned char dis1[]  ={"郝林峰"};	  


void pos(uchar X,uchar Y);  //确定显示位置

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
void lnit_12864()
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


void main()
{	 
	uchar t;
	uchar i;
	uchar k=0;
	lnit_12864();              //初始化12864 
	 
	 while(1)
	 { 	
	 	for(t=0;t<11;t++)
		{
			if(t==0)
			{
				pos(0,0);
				wdat(dis1[2]);
				delay(1000);
				wcmd(0x01);
			}
			if(t==1)
			{
				pos(0,0);
				wdat(dis1[1]);
				wdat(dis1[2]);
				delay(1000);
				wcmd(0x01);
				
			}
			if(t==9)
			{
				pos(0,7);
				wdat(dis1[0]);
				wdat(dis1[1]);
				delay(1000);
				wcmd(0x01);
				
			}
			if(t==10)
			{
				pos(0,0);
				wdat(dis1[0]);
				delay(1000);
				wcmd(0x01);
				
			}
			else
			{
				pos(0,k-2);
				i=0;
				while(dis1[i]!='\0')
      			{
        			wdat(dis1[i]);
       				i++;
      			}
				delay(1000);
				wcmd(0x01);
			}
		}
	 }			
}