#include <reg51.h>
#include <intrins.h>
#define uchar unsigned char
#define uint  unsigned int
#define HEADLEN     5  //���ݰ�ͷ�ĳ���
#define LEN_OFFSET  2  
/*12864Һ���� �˿ڶ���*/
#define data  P0           //���ݿ�
sbit   RS=P3^5;            //�Ĵ���ѡ������ 
sbit   RW=P3^6;            //Һ����/д����
sbit    E=P3^4;            //Һ��ʹ�ܿ���
sbit  PSB=P3^7;            //��/����ʽ����
sbit wela=P2^6;
sbit dula=P2^7;
/*DS18B20�¶ȴ����� �˿ڶ���*/
sbit DQ=P2^4;
uchar dis1[]={"ʵʱ�¶�:"};
uchar head[HEADLEN] = {0xfd,0x00,0x00,0x01,0x00};
/*��������ascii����*/
unsigned char mun_char_table[]={"0123456789abcdef"};

uchar num,temp,k1num,k2num,k3num,k4num;
void pos(uchar X,uchar Y);  //ȷ����ʾλ��

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
void lnit_12864()
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

//////////////////������DS18B20��������/////////////
//��ʱ����
void delay1(unsigned int i)
{
	while(i--);
}

//��ʼ������
void Init_DS18B20(void)
{
	unsigned char x=0;
	DQ = 1; //DQ��λ
	delay1(64); //������ʱ
	DQ = 0; //��Ƭ����DQ����
	delay1(640); //��ȷ��ʱ ���� 480us
	DQ = 1; //��������
	delay1(112);
	x=DQ; //������ʱ�� ���x=0���ʼ���ɹ� x=1���ʼ��ʧ��
	delay1(160);
}

//��һ���ֽ�
ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--){
		DQ = 0; // �������ź�
		dat>>=1;
		DQ = 1; // �������ź�
		if(DQ)  dat|=0x80;
		delay1(32);
		}
	return(dat);
}

//дһ���ֽ�
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

//��ȡ�¶�
ReadTemperature(void)
{
	unsigned char a=0;
	unsigned char b=0;
	unsigned int t=0;

	float tt=0;
	Init_DS18B20();
	WriteOneChar(0xCC); // ����������кŵĲ���
	WriteOneChar(0x44); // �����¶�ת��
	Init_DS18B20();
	WriteOneChar(0xCC); //����������кŵĲ���
	WriteOneChar(0xBE); //��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
	a=ReadOneChar();
	b=ReadOneChar();
	t=b;
	t<<=8;
	t=t|a;
	tt=t*0.0625; //���¶ȵĸ�λ���λ�ϲ�
	t= tt*10+0.5; //�Խ������4��5��
	return(t);
}
//////////////////������DS18B20��������////////////////

/*****************��������������������***************/
void main()
{
	  uint t;
      int i=0;
	  float num ;
	  lnit_12864();              //��ʼ��12864 
	  Init_DS18B20();            //��ʼ��DS18B20
	  
	  pos(0,0);                 //12864��ʾλ��Ϊ��һ�еĵ�1���ַ�
      while(dis1[i] != '\0')
      {                         //��ʾ�ַ� ��"ʵʱ�¶�:"
       wdat(dis1[i]);
       i++;
      }
	 
	 while(1)
	 {
  	   t=ReadTemperature();
	   delay(500);
	   t=ReadTemperature();	 
		pos(0,5);				//12864��ʾʵʱ�¶�  λ�õ�һ�еĵ�5���ַ�
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

