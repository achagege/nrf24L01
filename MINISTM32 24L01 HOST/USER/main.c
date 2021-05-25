#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "spi.h"
#include "24l01.h"  
#include "24cxx.h" 
#include "myiic.h"
#include "touch.h" 
#include "timer.h"
#include "chinese.h"

#define MAX 2
 int host_press=1;
 int score[8];
 int page=0;
 int isRight=-1;
 u8 num;
 u8 Tx_buf[2]={'1',0};	 
 u8 Rx_buf[32];
 int isAns=0;
 u8 qus[3][6]={{'1','+','1','=','?','\0'},{'2','+','2','=','?','\0'},{'3','+','3','=','?','\0'}};
 void home_page(void);
 void show_question(void);
 void lcd_click();
 void show_score(void);
 int main(void)
 { 
	u8 Tx_Cnt;
	u8 Rx_Cnt;
	u8 key,mode=1;	
 
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	EXTIX_Init();
	uart_init(9600);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
	LCD_Init();			   	//初始化LCD 	
 	NRF24L01_Init();    //初始化NRF24L01  
	tp_dev.init();	    //触摸屏初始化
	TIM3_Int_Init(9999,7199);
	home_page();
 	while(NRF24L01_Check())	//检查NRF24L01是否在位.	
	{
		LCD_ShowString(60,130,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}	
	NRF24L01_TX_Mode();
	POINT_COLOR=BLUE;//设置字体为蓝色	

	while(1){
		LED1=!LED1;
		delay_ms(1000);
		
		//1.主持人按下 
		//2.发送开始信号
		//NRF24L01开启发送模式
		//发送信号
		//开启接收模式  等待选手号码返回
		//开启回答倒计时
		//按键判断对错
		//可显示当前选手分数
		
		while(!host_press)    //主持人按下
		{
			if(page==0)
			{
				LCD_ShowString(20,213,200,16,16,"start!");
				LCD_Fill(25,280,240,300,WHITE); 
			} 
			if(mode==1){
				if(NRF24L01_TxPacket(Tx_buf)==TX_OK || Tx_Cnt==MAX){
					Tx_Cnt=0;
					mode=0;
					NRF24L01_RX_Mode();	
					break;
				}
				Tx_Cnt++;
			}else{
				if(NRF24L01_RxPacket(Rx_buf)==0){
					num=Rx_buf[0]-'0';
					Rx_Cnt=MAX;
					host_press=1;
				}
				if(Rx_Cnt==MAX){
					Rx_Cnt=0;
					mode=1;
					NRF24L01_TX_Mode();
					break;
				}
				Rx_Cnt++;
			}
			delay_ms(10);
		}
		
		//接收选手号码让定时器开始工作
		if(Rx_buf[0]-'0'>0){
			TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
			Rx_buf[0]=0;
		}
		
		//接收号码
		//等待主持人按下回答键
		//判断回答是否对错
	
		if(num > 0){		
			switch(isRight){
				case true:
					stop_timing();
					LCD_ShowString(25,280,200,16,16,"The answer is true"); 				
					score[num-1]++;
					isRight=-1;
					LCD_Fill(15,210,70,230,WHITE);   //清除start显示
					num=0;
					break;
				case false:
					stop_timing();
					LCD_ShowString(25,280,200,16,16,"The answer is false"); 
					isRight=-1;
					LCD_Fill(15,210,70,230,WHITE);
					num=0;
					break;
				default:
					break;	
			}	
		}
		//切换画面	
		lcd_click();
	}
}
 
void home_page(void){
	POINT_COLOR=BLACK;
	show_chinese16(20,80,16,"题目如下");
	show_chinese16(180,80,16,"下一道");
	LCD_ShowString(60,120,100,16,16,qus[isAns]);
	LCD_DrawLine(0,170,240,170);
	show_chinese24(60,20,24,"抢答器系统");	 
	LCD_DrawLine(0,60,240,60);
	show_chinese16(190,190,16,"倒计时"); 
	LCD_ShowxNum(218,210,0,2,16,0);
	
	show_chinese16(20,190,16,"选手");
	LCD_Draw_Circle(115,230,20);
	LCD_ShowNum(109,219,num,1,24);
	
	LCD_DrawRectangle(160,248,230,270); 
	show_chinese16(165,250,16,"查看分数"); 
	
	
}

	
	void lcd_click(){
		POINT_COLOR=BLUE;
		tp_dev.scan(
		0); 
		if(page==0){
			LCD_ShowNum(109,219,num,1,24);
			if(tp_dev.sta&TP_PRES_DOWN){
				//判断是否在点击区域
				if(tp_dev.x[0]<220&&tp_dev.y[0]<140){
						if(tp_dev.x[0]>160&&tp_dev.y[0]>60){
						//清除原来题目
							LCD_Fill(55,115,155,140,WHITE);
						//显示下一道题
							isAns++;
							if(isAns==3)isAns=0;
							LCD_ShowString(60,120,100,16,16,qus[isAns]);
							
						}
				}else if(tp_dev.x[0]<230&&tp_dev.y[0]<270){
						if(tp_dev.x[0]>160&&tp_dev.y[0]>248){
								LCD_Clear(WHITE);
								//显示选手得分情况
							page=1;
							show_score();		
					}
				}	
			}
		}else{
			if(tp_dev.sta&TP_PRES_DOWN){   //显示主界面
				//判断是否在点击区域
				if(tp_dev.x[0]<140&&tp_dev.y[0]<280){
					if(tp_dev.x[0]>90&&tp_dev.y[0]>255){
							LCD_Clear(WHITE);
							home_page();
							page=0;
					}
				}
			}
		}
	}
	
	void show_score(void){
		//清屏
		LCD_Clear(WHITE);
		
		LCD_ShowString(20,50,40,16,16,"NO.1:");
		LCD_ShowNum(70,50,score[0],2,16);
		
		LCD_ShowString(140,50,40,16,16,"NO.2:");
		LCD_ShowNum(190,50,score[1],2,16);
		
		LCD_ShowString(20,100,40,16,16,"NO.3:");
		LCD_ShowNum(70,100,score[2],2,16);
		
		LCD_ShowString(140,100,40,16,16,"NO.4:");
		LCD_ShowNum(190,100,score[3],2,16);
		
		LCD_ShowString(20,150,40,16,16,"NO.5:");
		LCD_ShowNum(70,150,score[4],2,16);	
		
		LCD_ShowString(140,150,40,16,16,"NO.6:");
		LCD_ShowNum(190,150,score[5],2,16);
		
		LCD_ShowString(20,200,40,16,16,"NO.7:");
		LCD_ShowNum(70,200,score[6],2,16);
		
		LCD_ShowString(140,200,40,16,16,"NO.8:");
		LCD_ShowNum(190,200,score[7],2,16);
		
		LCD_DrawRectangle(90,255,145,275);
		show_chinese16(95,255,16,"主界面");
		
		
	}


























