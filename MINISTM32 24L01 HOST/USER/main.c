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
 
	delay_init();	    	 //��ʱ������ʼ��	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	EXTIX_Init();
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	LCD_Init();			   	//��ʼ��LCD 	
 	NRF24L01_Init();    //��ʼ��NRF24L01  
	tp_dev.init();	    //��������ʼ��
	TIM3_Int_Init(9999,7199);
	home_page();
 	while(NRF24L01_Check())	//���NRF24L01�Ƿ���λ.	
	{
		LCD_ShowString(60,130,200,16,16,"NRF24L01 Error");
		delay_ms(200);
		LCD_Fill(60,130,239,130+16,WHITE);
 		delay_ms(200);
	}	
	NRF24L01_TX_Mode();
	POINT_COLOR=BLUE;//��������Ϊ��ɫ	

	while(1){
		LED1=!LED1;
		delay_ms(1000);
		
		//1.�����˰��� 
		//2.���Ϳ�ʼ�ź�
		//NRF24L01��������ģʽ
		//�����ź�
		//��������ģʽ  �ȴ�ѡ�ֺ��뷵��
		//�����ش𵹼�ʱ
		//�����ж϶Դ�
		//����ʾ��ǰѡ�ַ���
		
		while(!host_press)    //�����˰���
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
		
		//����ѡ�ֺ����ö�ʱ����ʼ����
		if(Rx_buf[0]-'0'>0){
			TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
			Rx_buf[0]=0;
		}
		
		//���պ���
		//�ȴ������˰��»ش��
		//�жϻش��Ƿ�Դ�
	
		if(num > 0){		
			switch(isRight){
				case true:
					stop_timing();
					LCD_ShowString(25,280,200,16,16,"The answer is true"); 				
					score[num-1]++;
					isRight=-1;
					LCD_Fill(15,210,70,230,WHITE);   //���start��ʾ
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
		//�л�����	
		lcd_click();
	}
}
 
void home_page(void){
	POINT_COLOR=BLACK;
	show_chinese16(20,80,16,"��Ŀ����");
	show_chinese16(180,80,16,"��һ��");
	LCD_ShowString(60,120,100,16,16,qus[isAns]);
	LCD_DrawLine(0,170,240,170);
	show_chinese24(60,20,24,"������ϵͳ");	 
	LCD_DrawLine(0,60,240,60);
	show_chinese16(190,190,16,"����ʱ"); 
	LCD_ShowxNum(218,210,0,2,16,0);
	
	show_chinese16(20,190,16,"ѡ��");
	LCD_Draw_Circle(115,230,20);
	LCD_ShowNum(109,219,num,1,24);
	
	LCD_DrawRectangle(160,248,230,270); 
	show_chinese16(165,250,16,"�鿴����"); 
	
	
}

	
	void lcd_click(){
		POINT_COLOR=BLUE;
		tp_dev.scan(
		0); 
		if(page==0){
			LCD_ShowNum(109,219,num,1,24);
			if(tp_dev.sta&TP_PRES_DOWN){
				//�ж��Ƿ��ڵ������
				if(tp_dev.x[0]<220&&tp_dev.y[0]<140){
						if(tp_dev.x[0]>160&&tp_dev.y[0]>60){
						//���ԭ����Ŀ
							LCD_Fill(55,115,155,140,WHITE);
						//��ʾ��һ����
							isAns++;
							if(isAns==3)isAns=0;
							LCD_ShowString(60,120,100,16,16,qus[isAns]);
							
						}
				}else if(tp_dev.x[0]<230&&tp_dev.y[0]<270){
						if(tp_dev.x[0]>160&&tp_dev.y[0]>248){
								LCD_Clear(WHITE);
								//��ʾѡ�ֵ÷����
							page=1;
							show_score();		
					}
				}	
			}
		}else{
			if(tp_dev.sta&TP_PRES_DOWN){   //��ʾ������
				//�ж��Ƿ��ڵ������
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
		//����
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
		show_chinese16(95,255,16,"������");
		
		
	}


























