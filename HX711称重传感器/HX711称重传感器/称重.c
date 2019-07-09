#include <reg51.h>
#include <intrins.h>	
#define uchar unsigned char
#define uint  unsigned int
/*12864Һ���� �˿ڶ���*/
#define data  P0           //���ݿ�
sbit   RS=P3^5;            //�Ĵ���ѡ������ 
sbit   RW=P3^6;            //Һ����/д����
sbit    E=P3^4;            //Һ��ʹ�ܿ���
sbit  PSB=P3^7;            //��/����ʽ����
sbit wela=P2^6;
sbit dula=P2^7;
/*HX711���ش����� �˿ڶ���*/
unsigned long InitialWeight;
unsigned long FinalWeight;	
uchar string[]={"0000"};
sbit  HX711_SCK=P1^0;
sbit HX711_DOUT=P1^1;

uchar code dis1[] = {"����Ϊ��"};
uchar code dis2[] = {"g"};

//////////////////������12864��������////////////////                                                              
//�������ܣ���ʱ����
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

//�������ܣ����LCDæ״̬
//lcd_busyΪ1ʱ��æ���ȴ���lcd-busyΪ0ʱ,�У���дָ�������ݡ�
bit busy()
 { 
    bit result;
    P0=0x00;              //�ص�����ܵ��źš���ֹ������ܵ�P0���źŵ�Ӱ��
    RS=0;
    RW=1;
     E=1;
    delay(1);
    result = (bit)(P0&0x80);
     E=0;
    return(result); 
 }

//�������ܣ�дָ�����ݵ�LCD                                                  
//RS=L��RW=L��E=�����壬D0-D7=ָ���롣
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
                                                               
//�������ܣ�д��ʾ���ݵ�LCD                                                  
//RS=H��RW=L��E=�����壬D0-D7=���ݡ�                               
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
                                                             
//�������ܣ�LCD��ʼ���趨                                                 
void Init_12864()
{ 
    PSB=1;         //���ڷ�ʽ
    
    wcmd(0x34);      //����ָ�����
    delay(5);
    wcmd(0x30);      //����ָ�����
    delay(5);
    wcmd(0x0c);      //��ʾ�����ع��
    delay(5);
    wcmd(0x01);      //���LCD����ʾ����
    delay(5);
}

//�������ܣ��趨��ʾλ��                                          
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
   wcmd(pos);     //��ʾ��ַ
}
//////////////////������12864��������////////////////

//////////////////������HX711���ش�������������//////////////// 
//�������ܣ���ʱ����
void delay_HX711(void)
{
	_nop_();
	_nop_();
}

//�������ܣ���ȡHX711����ֵ
unsigned long HX711_Read(void)	//����128
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
    weight=weight^0x800000;   //��25�������½�����ʱ��ת������
	delay_HX711();
	HX711_SCK=0;  
	return(weight);
}

//�������ܣ���ȡHX711������ֵ 
void Get_InitialWeight()
{
  InitialWeight= HX711_Read();	
} 

//�������ܣ�����������������
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

//�������ܣ�Һ������ʾ�ַ�	
void display()
{
	uchar i;
	wela=0;
	dula=0; 
	wcmd(0x01);      //���LCD����ʾ����
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

/*****************��������������������***************/
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

