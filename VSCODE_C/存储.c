#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int

uchar code table[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uchar num=0;
sbit sda=P2^0;
sbit scl=P2^1;
sbit dula=P1^0;

void delay()  //短暂延迟
{;;}

void delay_1ms(uint z)//长一点的延迟
{
    uint x,y;
    for(x=z;x>0;x--)
        for(y=110;y>0;y--)
            ;
}

void init()//初始化  sda与scl都为高电平
{

    sda=1;
    delay();
    scl=1;
    delay();

}

void display(uchar x)//二极管显示
{
	dula=0;
	P0=table[x]; 
}


void start()//开始 scl为高时 sda由高变为低
{
    sda=1;
    delay();
    scl=1;
    delay();
    sda=0;
    delay();
}

void stop()//停止 scl为高时sda由低变为高
{
    sda=0;
    delay();
    scl=1;
    delay();
    sda=1;
    delay();
}

void respons()//应答 如果时间过多 默认以回答
{
    uchar i;
    scl=1;
    delay();
    while((sda==1)&&(i<250))
        i++;
    scl=0;
    delay();
}



void write_byte(uchar date)
{
    uchar i,temp;
    temp=date;
    for(i=0;i<8;i++)
    {
        temp=temp<<1;
        scl=0;
        delay();
        sda=CY;
        delay();
        scl=1;
        delay();
    }
    scl=0;
    delay();
    sda=1;
    delay();
}

uchar read_byte()
{
    uchar i,k;
    scl=0;
    delay();
    sda=1;
    delay();
    for(i=0;i<8;i++)
    {
        scl=1;
        delay();
        k=(k<<1)|sda;
        scl=0;
        delay();
    }
    return k;
}

void write_add(uchar address,uchar date)
{
    start();
    write_byte(0xa0);//设备地址
    respons();
    write_byte(address);//要写入的地址
    respons();
    write_byte(date);//写入数据
    respons();
    stop();
}

uchar read_add(uchar address)
{
    uchar date;
    start();
    write_byte(0xa0);//设备地址
    respons();
    write_byte(address);
    respons();
    start();
    write_byte(0xa1);//读地址
    respons();
    date=read_byte();
    stop();
    return date;
}


void main()
{
    EA=1;
    ET0=1;
    TR0=1;
	num=read_add(1);
    while(1)
    {
        if(num>9)
            num=0;
        display(num);
		write_add(1,num) ;
        num++;
        delay_1ms(5000);

    }

}