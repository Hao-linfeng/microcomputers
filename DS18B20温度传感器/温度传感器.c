#include <reg51.h>
#include <intrins.h>
#define uchar unsigned char
#define uint  unsigned int
#define HEADLEN     5  //数据包头的长度
#define LEN_OFFSET  2  
/*12864液晶屏 端口定义*/
#define data  P0           //数据口
sbit   RS=P3^5;            //寄存器选择输入 
sbit   RW=P3^6;            //液晶读/写控制
sbit    E=P3^4;            //液晶使能控制
sbit  PSB=P3^7;            //串/并方式控制
sbit wela=P2^6;
sbit dula=P2^7;
/*DS18B20温度传感器 端口定义*/
sbit DQ=P2^4;
uchar dis1[]={"实时温度:"};
uchar head[HEADLEN] = {0xfd,0x00,0x00,0x01,0x00};
/*定义数字ascii编码*/
unsigned char mun_char_table[]={"0123456789abcdef"};

uchar num,temp,k1num,k2num,k3num,k4num;
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
//////////////////以下是12864驱动程序////////////////

//////////////////以下是DS18B20驱动程序/////////////
//延时函数
void delay1(unsigned int i)
{
	while(i--);
}

//初始化函数
void Init_DS18B20(void)
{
	unsigned char x=0;
	DQ = 1; //DQ复位
	delay1(64); //稍做延时
	DQ = 0; //单片机将DQ拉低
	delay1(640); //精确延时 大于 480us
	DQ = 1; //拉高总线
	delay1(112);
	x=DQ; //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	delay1(160);
}

//读一个字节
ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--){
		DQ = 0; // 给脉冲信号
		dat>>=1;
		DQ = 1; // 给脉冲信号
		if(DQ)  dat|=0x80;
		delay1(32);
		}
	return(dat);
}

//写一个字节
void WriteOneChar(unsigned char dat)
{
	unsigned char i=0;
	for (i=8; i>0; i--){
		DQ = 0;
		DQ = dat&0x01;
		delay1(40);
		DQ = 1;
		dat>>=1;
		}
}

//读取温度
ReadTemperature(void)
{
	unsigned char a=0;
	unsigned char b=0;
	unsigned int t=0;

	float tt=0;
	Init_DS18B20();
	WriteOneChar(0xCC); // 跳过读序号列号的操作
	WriteOneChar(0x44); // 启动温度转换
	Init_DS18B20();
	WriteOneChar(0xCC); //跳过读序号列号的操作
	WriteOneChar(0xBE); //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	a=ReadOneChar();
	b=ReadOneChar();
	t=b;
	t<<=8;
	t=t|a;
	tt=t*0.0625; //将温度的高位与低位合并
	t= tt*10+0.5; //对结果进行4舍5入
	return(t);
}
//////////////////以上是DS18B20驱动程序////////////////

/*****************以下是主函数驱动程序***************/
void main()
{
	  uint t;
      int i=0;
	  float num ;
	  lnit_12864();              //初始化12864 
	  Init_DS18B20();            //初始化DS18B20
	  
	  pos(0,0);                 //12864显示位置为第一行的第1个字符
      while(dis1[i] != '\0')
      {                         //显示字符 即"实时温度:"
       wdat(dis1[i]);
       i++;
      }
	 
	 while(1)
	 {
  	   t=ReadTemperature();
	   delay(500);
	   t=ReadTemperature();	 
		pos(0,5);				//12864显示实时温度  位置第一行的第5个字符
		wdat(mun_char_table[t/100]);
		wdat(mun_char_table[t%100/10]);
		wdat(0x2e);
		wdat(mun_char_table[t%10]);
		wdat(0x27);
		wdat(0x43);
		num= t/10.0;
		delay(10);
	}
}

