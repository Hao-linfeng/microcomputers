sbit DQ=P2^4;
uchar dis1[]={"实时温度:"};
uchar head[HEADLEN] = {0xfd,0x00,0x00,0x01,0x00};
/*定义数字ascii编码*/
unsigned char mun_char_table[]={"0123456789abcdef"};

uchar num,temp,k1num,k2num,k3num,k4num;



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
