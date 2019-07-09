#include<reg52.h>
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char
#define data P0	 //���ݿ�

sbit psb=P3^7; //��/����ѡ��
sbit  rw=P3^6;	 //��д����
sbit  rs=P3^5;  //�Ĵ���ѡ��  0Ϊָ��  1Ϊ����
sbit   E=P3^4;  //lcdʹ���źţ����ڣ�
sbit dat=P1^0;
unsigned char mun_table[]={'Y','N'};
void wcmd(uchar cmd);
void delay(int ms);
/*******************************************************************/
/*                                                                 */
/*���LCDæ״̬                                                    */
/*lcd_busyΪ1ʱ��æ���ȴ���lcd-busyΪ0ʱ,�У���дָ�������ݡ�      */
/*                                                                 */
/*******************************************************************/
bit busy()
 {                       
    bit result;
    P0=0X00;              //�ص�����ܵ��źš���ֹ������ܵ�P0���źŵ�Ӱ��
    rs=0;
    rw=1;
     E=1;
    delay(1);
    result=(bit)(P0&0x80);
     E=0;
    return(result); 
 }
/*��ʼ��*/
void init(void)
{
	psb=1;		//���ڷ�ʽ
    wcmd(0x34);		 //��չָ��
	delay(5);
	wcmd(0x30);		//����ָ��
	delay(5);
	wcmd(0x0c);		 //��ʾ��꿪��
	delay(5);
	wcmd(0x01);		 //���lcd����ʾ����
	delay(5);
}

/*����ָ��*/
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
/*��������*/
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
/*��ʾ��λ��*/
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
/*  ��ʱ����                                                       */
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
//����������
void main(void)
{
	uint i = 0;
	uchar name[] = {"�Ƿ������ϰ���"};
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
		   wdata(mun_table[0]);	  //���˾�������͵�ƽ����ͨ���
		   delay(1000);
		}	
	}
}