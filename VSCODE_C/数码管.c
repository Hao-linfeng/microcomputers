#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int

sbit wei_1=P2^0;
sbit wei_2=P2^1;
uchar code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uchar num=0;

void delay(uchar xms);
void display(uchar num);

void main(void)
{
    TMOD=0x01;
    TH0=(65536-50000)/256;
    TL0=(65536-50000)%256;
    EA=1;
    ET0=1;
    TR0=1;
    while(1)
    {

        display(num);

    }


}
void delay(uchar xms)
{
    uint i,j;
    for(i=xms;i>0;i--)
        for(i=110;j>0;j--);
}

void display(uchar num)
{
    uchar i=(num/10);
    uchar j=(num%10);

    wei_1=0;
    P0=table[i];
    delay(20);
    wei_1=1;

    wei_2=0;
    P0=table[j];
    delay(20);
    wei_2=0;
}

void time() interrupt 1
{
    TH0=(65536-50000)/256;
    TL0=(65536-50000)%256;
    delay(100);
    num++;
    if(num==60)
        num=0;
}
