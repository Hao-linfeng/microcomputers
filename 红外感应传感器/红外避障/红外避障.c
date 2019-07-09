#include<reg52.h>
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char
#define data P0	 //数据口

sbit psb=P3^7; //串/并口选择
sbit  rw=P3^6;	 //读写控制
sbit  rs=P3^5;  //寄存器选择  0为指令  1为数据
sbit   E=P3^4;  //lcd使能信号（并口）
sbit dat=P1^0;
unsigned char mun_table[]={'Y','N'};
void wcmd(uchar cmd);
void delay(int ms);
/*******************************************************************/
/*                                                                 */
/*检查LCD忙状态                                                    */
/*lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。      */
/*                                                                 */
/*******************************************************************/
bit busy()
 {                       
    bit result;
    P0=0X00;              //关掉数码管的信号。阻止数码管受到P0口信号的影响
    rs=0;
    rw=1;
     E=1;
    delay(1);
    result=(bit)(P0&0x80);
     E=0;
    return(result); 
 }
/*初始化*/
void init(void)
{
	psb=1;		//并口方式
    wcmd(0x34);		 //扩展指令
	delay(5);
	wcmd(0x30);		//基本指令
	delay(5);
	wcmd(0x0c);		 //显示光标开关
	delay(5);
	wcmd(0x01);		 //清除lcd的显示内容
	delay(5);
}

/*传递指令*/
void wcmd(uchar cmd)
{
	while(busy());
	rs=0;
	rw=0;
	 E=0;
	data=cmd;
	delay(1);
	 E=1;
	delay(1);
	 E=0;
}
/*传递数据*/
void wdata(uchar dat)
{
	while(busy());
	rs=1;
	rw=0;
	 E=0;
	data=dat;
	delay(1);
	 E=1;
	delay(1);
	 E=0;
}
/*显示的位置*/
void pos(uchar x,uchar y)
{
	uchar pos;
	if(x==0)
	{x=0x80;}
	else if(x==1)
	{x=0x90;}
	else if(x==2)
	{x=0x88;}
	else if(x==3)
	{x=0x98;}
	pos=x+y;
	wcmd(pos);
}										
/*******************************************************************/
/*                                                                 */
/*  延时函数                                                       */
/*                                                                 */
/*******************************************************************/
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
//主函数程序
void main(void)
{
	uint i = 0;
	uchar name[] = {"是否遇到障碍："};
	delay(400); 
	init();

	pos(0,0);
	while(name[i] != '\0')
	{
	  wdata(name[i]);
	  i++;
	}
   delay(500);
   
	while(1)
	{	
	    pos(0,7);
		if(dat==1)
		{
		  wdata(mun_table[1]);	  
		}
		else
		{
		   wdata(mun_table[0]);	  //有人经过输出低电平，导通检测
		   delay(1000);
		}	
	}
}