#include <reg51.h>
#include <stdio.h>
#include <intrins.h>
#include <string.h>
#define uchar unsigned char
#define uint  unsigned int
/*�������̶˿ڶ���*/
sbit  S1=P1^1;
/*DS18B20�¶ȴ���������*/
#define HEADLEN       5  //���ݰ�ͷ�ĳ���
#define LEN_OFFSET    2 
sbit DQ=P2^4; 			
uchar head[HEADLEN] = {0xfd,0x00,0x00,0x01,0x00};
/*�����ϳ�ģ�鶨��*/
#define BUFLEN      32  //���ֻ������ĳ���
sbit  BY=P1^0;			//BY���ڼ�ɨ��
//sbit TXD=P3^0;			//TXD���ڷ���
//sbit RXD=P3^1;			//RXD���ڽ���
/*12864Һ���� �˿ڶ���*/
#define data  P0           //���ݿ�
sbit   RS=P3^5;            //�Ĵ���ѡ������ 
sbit   RW=P3^6;            //Һ����/д����
sbit    E=P3^4;            //Һ��ʹ�ܿ���
sbit  PSB=P3^7;            //��/����ʽ����
sbit wela=P2^6;
sbit dula=P2^7;
/*��������ascii����*/
unsigned char mun_char_table[]={"0123456789abcdef"};

uchar num,temp,k1num,k2num,k3num,k4num;

//���֣��м�����Ҫ�仯�����ݿ��ڷ���ǰ�޸ģ�
uchar text[BUFLEN] = {"��ǰ�¶�"};
uchar  dison[]={"q007"};
uchar  clear[]={"                "};
uchar  dis1[]={"��ǰ�¶�:"};

void delay0(uchar x);  //x*0.14MS
void beep();
void dataconv();
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
	unsigned int  t=0;

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
	t=tt*10+0.5; //�Խ������4��5��
	return(t);
}
//////////////////������DS18B20��������////////////////

//////////////////����������ģ����������///////////////
//�������ܣ����ڳ�ʼ��
void UART_InitBaud(void)
{
	SCON=0xD8 ;  //����Ϊ11.059MHZʱ���趨���ڲ�����Ϊ9600bit/s����ʽ3  
	TMOD=0x20 ;
	PCON=0x00 ; 
	TH1=0xFD ; 
	TR1=1;	
}

//�������ܣ����ڷ�������
void UART_Trans(uchar n)
{
	SBUF = n;    
	while(TI==0);  //��������    
	TI=0;	
}				

//�������ܣ���������
void Speech(uchar *buf, uchar len)
{
	uchar i = 0;          //ѭ����������
	uchar xor = 0x00;     //У�����ʼ��
			
	head[LEN_OFFSET] = len + 3;  //�������ĳ��ȣ�1������ + 1������� + ���ֳ��� + 1У��λ��

//�������ܣ��������ݰ�ͷ��0xFD + 2�ֽڳ��� + 1�ֽ������� + 1�ֽ��������)
	for(i = 0; i < HEADLEN; i++)
	{
		xor ^= head[i];
		UART_Trans(head[i]);
		delay(2);
   	}

//�������ܣ�������������
	for(i=0; i<len;i++)
	{
	  xor ^= buf[i];
	  UART_Trans(buf[i]);
	  delay(2);
	}

	UART_Trans(xor);        //����У��λ
}
//////////////////����������ģ����������///////////////

/*****************��������������������***************/
void main()
{	  
   char numbuf[10];
   uchar xor = 0;
   uint t;
   int i=0;
   float num ;
   Init_12864();              //��ʼ��LCD 
   Init_DS18B20();            //��ʼ��DS18B20
   t=ReadTemperature();
	 delay(500);
   t=ReadTemperature();
     pos(0,0);             //������ʾλ��Ϊ��һ�еĵ�1���ַ�
   	 
	memset(numbuf,0,sizeof numbuf);
	UART_InitBaud();  //��ʼ������Ϊ 9600bps
    
	while(dis1[i] != '\0')
     {                         //��ʾ�����¶�
       wdat(dis1[i]);
       i++;
     }
	
	while(1)
	 {
  	 t=ReadTemperature();
	   delay(500);
	 t=ReadTemperature();	 
		pos(0,5);							  //��һ����ʾʵʱ�¶�
		wdat(mun_char_table[t/100]);
		wdat(mun_char_table[t%100/10]);
		wdat(0x2e);
		wdat(mun_char_table[t%10]);
		num= t/10.0;
		delay(10);
		if(BY!= 0xff)
		{
		  delay(10);
		  if(BY!= 0xff)
		  {
			if(S1==0)
			{
			 delay(10);
			 if(S1==0)
			 {
			   sprintf(text+15,"%6.2f",num);  //�������� num תΪ�ַ�����
				                              //��׷�ӵ�����text[]�ĵ�6�ֽں�
											  //��ǰ6�ֽ�Ϊ�ַ�������ǰֵ����
											 //�μ��⺯��sprintf()���÷�		
			   Speech(text,20);   //���÷�������������ָ����������
			 }
			}
		  }
		}
		delay(100);	//��ʱ
	}
}