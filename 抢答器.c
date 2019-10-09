  #include<reg51.h>
 sbit led_one=P0^0;
 sbit led_two=P0^1;
 sbit control=P3^7;
 sbit song=P3^0;

unsigned char number[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char time =0;
unsigned char num=0;


 void delay(int  k)			 //延迟
 {
 	unsigned int i,j;
	for(i=k;i>0;i--)
		for(j=110;j>0;j--);
 }

 void display(int i,int n)   //显示 i为数码管   num为显示的数字
 {
     if(i==1)
     {
        led_one=0;
        led_two=1;
     }
     else
     {
         led_one=1;
         led_two=0;
     }
     P2=number[n];
 }

 void display_two()
{	  
 	 display(2,time%10);
   	 delay(30);
    display(1,time/10);
    delay(50);
   
}

void init()
{
    TMOD=0x01;
     TH0=(65536-45872)/256;
     TL0=(65536-45872)%256;
     EA=1;
     ET0=1;
     TR0=0;
}

void S()
{
	  song=0;
	  delay(900);
	  song=1;
	 
}

 void main()
 {
    init();
 	while(1)
	{	
		if(control==0)
        {
                
			if(TR0==0)
			{
				TR0=1;
			}
			else
			{
				TR0=0;
			}				      
			time=0;
		  
        }

        display_two();
	}
 }

 void timer() interrupt 1
 {
     TH0=(65536-45872)/256;
     TL0=(65536-45872)%256;
     num++;
     if(num==20)
     {
         num=0;
		 time++;
         if(time==31)
         {
             time=0;
			 TR0=0;
			 S();
         }       
     }
	 if(P1==0xfe)
        {
			TR0=0;               
			time=0;
			S();
        }

        if(P1==0xfd)
        {
			TR0=0;               
			time=1;
			 S();
        }
        if(P1==0xfb)
        {
			TR0=0;               
			time=2;
			  S();
        }
        if(P1==0xf7)
        {
			TR0=0;               
			time=3;
			  S();
        }
        if(P1==0xef)
        {
			TR0=0;               
			time=4;
			  S();
        }
        if(P1==0xdf)
        {
			TR0=0;               
			time=5;
			 S();
        }
        if(P1==0xbf)
        {
			TR0=0;              
			time=6;
			S();
        }
        if(P1==0x7f)
        {
			TR0=0;               
			time=7;
			 S();
		}
 }
