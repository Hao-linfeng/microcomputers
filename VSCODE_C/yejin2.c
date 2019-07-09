#include<reg51.h>
#include<intrins.h>
#include<stdlib.h>

#define uchar unsigned char
#define uint unsigned int
#define LCD_data P0

sbit LCD_RS=P3^5;
sbit LCD_RW=P3^6;
sbit LCD_EN=P3^4;
sbit LCD_PSB=P3^7;

uchar code dis1[] ={"hello world!"};
uchar code dis2[] = {"凌斌and 郝林峰"};
uchar code dis3[] = {"队名Bond"};
uchar code dis4[] = {"ABCDEFGH"};

void delay(uint x)//延迟X秒
{
  uint i,j;
  for(j=0;j<x;j++)
    for(i=0;i<110;i++)
      ;
}

void write_cmd(uchar cmd)  //写指令
{
  LCD_RS=0;
  LCD_RW=0;
  LCD_EN=0;
  P0=cmd;
  delay(5);
  LCD_EN=1;
  delay(5);
  LCD_EN=0;
}

void write_dat(uchar cmd)  //写数据
{
  LCD_RS=1;
  LCD_RW=0;
  LCD_EN=0;
  P0=cmd;
  delay(5);
  LCD_EN=1;
  delay(5);
  LCD_EN=0;
}

void lcd_pos(uchar X,uchar Y)
{
  uchar pos;
  if(X==0)
    X=0x80;
  else if(X==1)
    X=0x90;
  else if(X==2)
    X=0x88;
  else if(X==3)
    X=0x98;
    pos=X+Y;
    write_cmd(pos);
}

void lcd_init()
{
  LCD_PSB=1;
  write_cmd(0x30);
  delay(5);
  write_cmd(0x0c);
  delay(5);
  write_cmd(0x01);
  delay(5);
}
void one()
{
    uchar i;
    lcd_pos(0,0);
      i=0;
      while(dis2[i]!='\0')
      {
        write_dat(dis2[i]);
        i++;
      }

      lcd_pos(1,0);
      i=0;
      while(dis3[i]!='\0')
      {
        write_dat(dis3[i]);
        i++;
      }

      lcd_pos(2,0);
      i=0;
      while(dis4[i]!='\0')
      {
        write_dat(dis4[i]);
        i++;
      }

      lcd_pos(3,0);
      i=0;
      while(dis1[i]!='\0')
      {
        write_dat(dis1[i]);
        i++;
      }
}

void two()
{
    uchar i;
    lcd_pos(1,0);
      i=0;
      while(dis2[i]!='\0')
      {
        write_dat(dis2[i]);
        i++;
      }

      lcd_pos(2,0);
      i=0;
      while(dis3[i]!='\0')
      {
        write_dat(dis3[i]);
        i++;
      }

      lcd_pos(3,0);
      i=0;
      while(dis4[i]!='\0')
      {
        write_dat(dis4[i]);
        i++;
      }

      lcd_pos(0,0);
      i=0;
      while(dis1[i]!='\0')
      {
        write_dat(dis1[i]);
        i++;
      }
}

void three()
{
    uchar i;
    lcd_pos(2,0);
      i=0;
      while(dis2[i]!='\0')
      {
        write_dat(dis2[i]);
        i++;
      }

      lcd_pos(3,0);
      i=0;
      while(dis3[i]!='\0')
      {
        write_dat(dis3[i]);
        i++;
      }

      lcd_pos(0,0);
      i=0;
      while(dis4[i]!='\0')
      {
        write_dat(dis4[i]);
        i++;
      }

      lcd_pos(1,0);
      i=0;
      while(dis1[i]!='\0')
      {
        write_dat(dis1[i]);
        i++;
      }
}

void four()
{
    uchar i;
    lcd_pos(3,0);
      i=0;
      while(dis2[i]!='\0')
      {
        write_dat(dis2[i]);
        i++;
      }

      lcd_pos(0,0);
      i=0;
      while(dis3[i]!='\0')
      {
        write_dat(dis3[i]);
        i++;
      }

      lcd_pos(1,0);
      i=0;
      while(dis4[i]!='\0')
      {
        write_dat(dis4[i]);
        i++;
      }

      lcd_pos(2,0);
      i=0;
      while(dis1[i]!='\0')
      {
        write_dat(dis1[i]);
        i++;
      }

}

void main()
{
  uchar i;
  lcd_init();

  while(1)
  {
      one();
      delay(1000);delay(1000); delay(1000);
	    delay(1000);delay(1000); delay(1000);
      write_cmd(0x01)  ;

      two();
      delay(1000); delay(1000);	delay(1000);
	    delay(1000);delay(1000); delay(1000);
      write_cmd(0x01);

      three();
      delay(1000); delay(1000);	delay(1000);
	    delay(1000);delay(1000); delay(1000);
      write_cmd(0x01) ;

      four();
      delay(1000); delay(1000);delay(1000);
	    delay(1000);delay(1000); delay(1000);
      write_cmd(0x01);   
  }
}


