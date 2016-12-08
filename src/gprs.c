#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stm32f10x_conf.h"

#include "gprs.h"
#include "hard_init.h"
#include "usart.h"
//#include "../USART/usart.h"
//#include "../TIMERS/times.h"
//#include "../lcd_nokia5110/nokia_5110.h"
//#include "EEPROM/eeprom.h"

//porting
//#define PUT dbg_printf
#define delay_GSM delay_ms
//#define PUT_2 dbg_printf



int num_of_sms;
#define SMS_CMD_LEN 30
char at_readsms[SMS_CMD_LEN];
char tpy_sms_content[SMS_CMD_LEN];
char tpy_send_tel_num[SMS_CMD_LEN];

unsigned char is_gprs_mode_ok = 1;                  //gprs start succeed
unsigned char is_gprs_mode_start_succeed = 0;       //gprs connect ok
SIM_STATUS current_status = INIT0 ;


//REG
#define ATE0_CMD "ATE0\r\n"           		/// 禁止回显
#define CREG_CMD "AT+CREG?\x00D\x00A"       /// 查询网络注册情况
#define CGMM_CMD "AT+CGMM\x00D\x00A"
#define SIMCARD_CMD "AT+CPIN?\x00D\x00A"


//LD GPRS
#define GPRS_QD "AT+CSTT\x00D\x00A"         /// GPRS启动
#define GPRS_JH "AT+CIICR\x00D\x00A"        /// 移动场景开启,激活
//LGL GPRS
#define GPRS_DEF_PDP "AT+CGDCONT=1,\"IP\",\"CMNET\"\x00D\x00A"  /// 接入点
#define GPRS_ACT_PDP "AT+CGATT=1\x00D\x00A"                     /// GPRS激活PDP

#define GPRS_CMNET_APN "AT+CIPCSGP=1,\"CMNET\"\x00D\x00A"       /// 设置GPRS模式


//CONNECT... GPRS
#define GPRS_BJ_ADDR "AT+CIFSR\x00D\x00A"   /// 获取本地IP
#define GPRS_TCP1 "AT+CIPSTART="            /// 建立一个IP连接
#define GPRS_TCP2 PROTOTOCOL
#define GPRS_TCP3 IPADDR
#define GPRS_TCP4 PORTNUM

#define PM "^SIM800A^"
//SEND  GPRS
#define GPRS_SEND_DATA "AT+CIPSEND\x00D\x00A"


//SEND SMS
#define GSM_MSG_MODE "AT+CMGF=1\x00D\x00A"
//#define GSM_CENTER_NUM "AT+CSCA=\"+8613800512500\"\x00D\x00A"
#define GSM_CHAR_MODE "AT+CSCS=\"GSM\"\x00D\x00A"
#define GSM_SEND_TEXT "AT+CMGS=\"+8613512963352\"\x00D\x00A"
//READ SMS
#define SMSCAT "CMTI: \"SM\","
#define SMSCATRHOS "+CMT:"
//ds18b20
#define NO_SENSOR "no sensor"

char g_gprs_conn_ok=0;	   //GPRS处于连接状态
char ibusy  = 0;		   //在发送数据的时候,心跳包停止

void start_gprs_mode(void)
{
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);
    delay_GSM(100);
    GPIO_SetBits(GPIOB,GPIO_Pin_0);
    delay_GSM(10000);
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);
}


/// 建立一个IP连接 AT+CIPSTART=”TCP”,”211.136.42.12”,”2020”
void __GPRS_TCPIP()
{//AT+CMGR=1

    send_string_uart3(GPRS_TCP1);
    send_data_uart3(0x22);
    send_string_uart3(PROTOTOCOL);
    send_data_uart3(0x22);
    send_data_uart3(',');
    send_data_uart3(0x22);
    send_string_uart3(IPADDR);
    send_data_uart3(0x22);
    send_data_uart3(',');
    send_data_uart3(0x22);
	send_string_uart3(PORTNUM);
    send_data_uart3(0x22);
    send_data_uart3(0x0D);
    send_data_uart3(0x0A);
    PUT("destination address:");
	PUT(IPADDR);
/*
	LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(g_config_data.ipaddr)*6)/2,3,LCD_BANK_LINE);
    if(0)
    {
        LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(g_config_data.ipaddr)*6)/2,3,g_config_data.ipaddr);
    }
    else
    {
        LCD_write_english_string(0,3,g_config_data.ipaddr);
        LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(PM)*6)/2,1,PM);
    }
*/

    delay_GSM(3000);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

static void S_GSM_INIT0()
{

    PUT_2("S_GSM_INIT0 \r\n");
    clear_buf_uart3();
    send_string_uart3(ATE0_CMD);		  //关闭回显
    current_status = RST ;
}
static void S_RST()
{
    PUT_2(buf_uart3.buf);
    if(buf_uart3.buf[2] == 'O' &&
       buf_uart3.buf[3] == 'K')
    {

        PUT_2("S_RST OK \r\n");
        is_gprs_mode_start_succeed = 1;		 //GPRS模块和单片机的串口3已经通信上了
        clear_buf_uart3();
        current_status = SIMCARD ;
        send_string_uart3(SIMCARD_CMD);

    }
	else
	{
      //  LCD_clear();
      //  LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_START_FALSE)*6)/2,2,SIM_START_FALSE);
        delay_GSM(5000);
        clear_buf_uart3();
        current_status = RST ;
        send_string_uart3(ATE0_CMD);
    }
}

static void S_SIMCARD_CMD()
{
    char *pp = NULL;
    PUT(buf_uart3.buf);
    pp = strstr((const char*)buf_uart3.buf,"OK");
    if(pp)
    {
        PUT_2("SIMCARD_CMD OK \r\n");
		//LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIMCARD_SUCCEED)*6)/2,2,SIMCARD_SUCCEED);
        clear_buf_uart3();
        current_status = CREG ;
        send_string_uart3(CREG_CMD);
    }
	else
	{
        PUT_2("SIMCARD_CMD ERR \r\n");
		//LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIMCARD_FALSE)*6)/2,2,SIMCARD_FALSE);
        clear_buf_uart3();
        current_status = SIMCARD ;
        send_string_uart3(SIMCARD_CMD);
    }
}

static void S_CREG()
{
    if(((buf_uart3.buf[9]=='0')&&
        (buf_uart3.buf[11]=='1'))||
       ((buf_uart3.buf[9]=='0')&&
        (buf_uart3.buf[11]=='5')))
    {
        PUT_2("S_REG OK \r\n");
        current_status = INIT_FINISH_OK;
    }else{
        PUT_2(buf_uart3.buf);
        PUT_2("S_REG ERR \r\n");

        debug_put_word(buf_uart3.buf[9]);
        debug_put_word(buf_uart3.buf[11]);

        clear_buf_uart3();
        current_status = CREG ;
        send_string_uart3(CREG_CMD);
    }
}


void init_gprs(void)
{

    PUT("start init GPRS\r\n");
    current_status = INIT0;

    while(1)
    {
        err ++;
        if(err>MAX_ERROR_COUNT)
        {
            err = 0 ;
            is_gprs_mode_ok = 0;
            //LCD_clear();
            //LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_REGISTER_FALSE)*6)/2,2,SIM_REGISTER_FALSE);
            delay_GSM(100000);
            break;
        }

        if(is_gprs_mode_start_succeed)
        {
            //LCD_clear();
            //LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_REGISTER)*6)/2,2,SIM_REGISTER);
        }

        delay_GSM(1000);

        switch(current_status)
        {
        case INIT0:
            S_GSM_INIT0();
            break ;

        case RST:
            S_RST();
            break ;

        case SIMCARD:
            S_SIMCARD_CMD();
            break ;

        case CREG:
            S_CREG();
            break ;

        case INIT_FINISH_OK:
            return ;

        default:
            clear_buf_uart3();
            break ;
        }
    }
    return ;
}

 ///////////////////////////////////////////////////////////////////////////

static void S_START_OPEN_GPRS_S()               //开始GPRS 检测SIM卡先
{
    PUT_2("S_START_OPEN_GPRS_S OK \r\n");

    clear_buf_uart3();
    current_status = GPRS_DEF_PDP_S ;           //定义PDP移动场景
    send_string_uart3(SIMCARD_CMD);
}


static void S_GPRS_DEF_PDP_S()                  //检测SIM卡 定义PDP
{
    char *t = NULL ;

    t = strstr((const char*)buf_uart3.buf,"OK");
    if(t)
    {
        PUT_2("SIMCARD_CMD OK \r\n");
        clear_buf_uart3();
        current_status = GPRS_ACT_PDP_S ;
        send_string_uart3(GPRS_DEF_PDP);
    }
    else
    {
        PUT_2("SIMCARD_CMD NO \r\n");
        clear_buf_uart3();
        send_string_uart3(SIMCARD_CMD);
    }
}

static void S_GPRS_ACT_PDP_S()                  //判断定义PDP 激活PDP
{
    if((buf_uart3.buf[2]=='O')&&(buf_uart3.buf[3]=='K'))
    {
        PUT_2("S_GPRS_DEF_PDP_S OK \r\n");
        clear_buf_uart3();
        current_status = GPRS_ACT_PDP_S_RET ;       // 发送激活
        send_string_uart3(GPRS_ACT_PDP);
    }
    else
    {
        PUT_2("S_GPRS_DEF_PDP_S NO \r\n");

        clear_buf_uart3();
        send_string_uart3(GPRS_DEF_PDP);			//接入点

    }
}

static void S_GPRS_ACT_PDP_S_RET()                  //判断激活
{
    if((buf_uart3.buf[2]=='O')&&(buf_uart3.buf[3]=='K'))
    {
        PUT_2("S_GPRS_ACT_PDP_S_RET OK \r\n");
        clear_buf_uart3();
        current_status = GPRS_OPEN_FINISH;
    }
    else
    {
        PUT_2(buf_uart3.buf);
        PUT_2("S_GPRS_ACT_PDP_S_RET NO \r\n");
        clear_buf_uart3();
		send_string_uart3(GPRS_ACT_PDP);
    }
}

void open_gprs_simple(void)
{

    if(!is_gprs_mode_ok)
        return ;

    current_status = INIT_FINISH_OK;

    while(1)
    {
        err ++;
        if(err>MAX_ERROR_COUNT)
        {
            err = 0 ;
            //LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_GPRS_FALSE)*6)/2,2,SIM_GPRS_FALSE);
            delay_GSM(100000);
            break;
        }

        //LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_GPRS)*6)/2,2,SIM_GPRS);
        delay_GSM(1000);


        switch(current_status)
        {
        case INIT_FINISH_OK:
            S_START_OPEN_GPRS_S();		   //再次发送卡状态
            break ;

        case  GPRS_DEF_PDP_S:
            S_GPRS_DEF_PDP_S();			   //发送接入点
            break ;

        case  GPRS_ACT_PDP_S:
            S_GPRS_ACT_PDP_S();			//发送激活请求
            break ;

        case GPRS_ACT_PDP_S_RET:
            S_GPRS_ACT_PDP_S_RET();
            break ;

        case GPRS_OPEN_FINISH:
            return ;
        default:
            clear_buf_uart2();
            break ;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////
static void S_GPRS_OPEN_FINISH()
{
    clear_buf_uart3();
    current_status = TCPIP_BJ_ADDR ;
    PUT_2("S_GPRS_OPEN_FINISH\r\n");
}

static void S_TCPIP_BJ_ADDR()
{
    clear_buf_uart3();
    current_status = TCPIP_CONNECT ;
    send_string_uart3(GPRS_BJ_ADDR);		  //获取本地IP地址
    PUT_2("S_TCPIP_BJ_ADDR\r\n");
}

static void  S_TCPIP_CONNECT()
{		
	clear_buf_uart3();
    current_status = TCP_IP_CONNECTING ;
    __GPRS_TCPIP();							   //创建TCPIP通信
}

static void S_TCP_IP_CONNECTING()
{

    char *ret_val = NULL;
	char *aret_val = NULL;
	char *bret_val = NULL ;
    static unsigned int count = 0 ;

    ret_val =  strstr((const char*)buf_uart3.buf,"CONNECT OK");
    aret_val = strstr((const char*)buf_uart3.buf,"ALREADY CONNECT");
	bret_val = strstr((const char*)buf_uart3.buf,"CONNECT FAIL");
	
    PUT("\r\nS_TCP_IP_CONNECTING...\r\n");
	PUT(buf_uart3.buf);
	g_gprs_conn_ok = 0;
	if (ret_val)				  					 //查询到CONNECT的反馈
    {
        current_status = TCP_IP_OK ;
		g_gprs_conn_ok=1;
		return ;
    }
    if ((ret_val||aret_val)&&!bret_val)				  					 //查询到CONNECT的反馈
    {
        current_status = TCP_IP_OK ;
		g_gprs_conn_ok=1;
    }
    else
    {
        count ++ ;
        delay_GSM(5000);
        if (count == 30)
        {
            current_status = TCP_IP_NO ;
            count = 0 ;
			return ;
        }
		clear_buf_uart3();
    	current_status = TCP_IP_CONNECTING ;
    	__GPRS_TCPIP();	

    }
}


void creat_tcp()
{
    if(!is_gprs_mode_ok)
        return ;

    while(1)
    {
        err ++;

        if(err>MAX_ERROR_COUNT)
        {
            err = 0 ;
            //LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_TCPIP_FALSE)*6)/2,2,SIM_TCPIP_FALSE);
            break;
        }

        delay_GSM(1000);
        switch(current_status)
        {
        case GPRS_OPEN_FINISH:
            S_GPRS_OPEN_FINISH();
            break ;

        case TCPIP_BJ_ADDR:
            S_TCPIP_BJ_ADDR();
            break ;

        case TCPIP_CONNECT:
            S_TCPIP_CONNECT();
            break ;

        case TCP_IP_CONNECTING:
            S_TCP_IP_CONNECTING();
            break ;

        case TCP_IP_OK :
            clear_buf_uart3();
            return ;
        case  TCP_IP_NO:
            clear_buf_uart3();
            return ;
        default:
            clear_buf_uart3();
            break ;
        }
    }
}



void send_gprs_data(char * buf , unsigned int count)
{
    unsigned int i ,j;
    if (current_status != TCP_IP_OK)        // 如果协议没有连接成功直接返回
        return ;

	ibusy = 1;
    //LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(LCD_BANK_LINE)*6)/2,5,LCD_BANK_LINE);
    //LCD_write_english_string((LCD_WIDTH_PIXELS - strlen(SIM_SEND_GPRS_DATA_TEST)*6)/2,5,SIM_SEND_GPRS_DATA_TEST);
#if 1
    send_string_uart3(GPRS_SEND_DATA);		  //发送数据命令 注意没长度限制
    delay_GSM(2000);

    for( j = 0 ;j < count ; j ++)
    {
        for ( i = 0 ; i < strlen((const char*)buf) ; i ++)
        {
            send_data_uart3(buf[i]);
        }
    }

    send_data_uart3(0x1A);  //LF			  //没长度限制一定要用1A表示结束
#endif
	ibusy = 0;
}

void gprs_heart(char *w,int t)										 //温度和心跳参数
{	
	//char *ptt = NULL;
	char tstr[BUFLEN];
	char tbuf[BUFLEN];
	memset(tbuf,0,BUFLEN);
	memset(tstr,0,BUFLEN);
	sprintf(tstr,"%d",t);
/*	ptt =  strstr(SERNUM,"temp");		//检测到温度序列号
    if(ptt)
	{
		if(strlen(w)>0)
			send_gprs_data(w,1);									//发送温度
		else
		    send_gprs_data(NO_SENSOR,1);
	}
	else
	{*/
		strcat(tbuf,"SN:");
		strcat(tbuf,SERNUM);
		strcat(tbuf," BEAT:");
		strcat(tbuf,tstr);
		send_gprs_data(tbuf,1);										 //发送心跳和序列号
	//}
}



static void S_GSM_MSG_MODE_S()
{

    PUT("S_GSM_MSG_MODE_S OK \r\n");
    clear_buf_uart3();
    current_status = GSM_MSG_MODE_S ;           //定义PDP移动场景
    send_string_uart3(GSM_MSG_MODE);

}

static void S_GSM_CHAR_MODE_S()
{
    if((buf_uart3.buf[2]=='O')&&(buf_uart3.buf[3]=='K'))
    {
        PUT("S_GSM_CHAR_MODE_S OK \r\n");
        clear_buf_uart3();
        current_status = GSM_CHAR_MODE_S;
        send_string_uart3(GSM_CHAR_MODE);
    }
    else
    {
        PUT("S_GSM_CHAR_MODE_S NO \r\n");
        clear_buf_uart3();
        send_string_uart3(GSM_MSG_MODE);
    }
}

static void S_GSM_CHAR_MODE_RET_S()
{
    if((buf_uart3.buf[2]=='O')&&(buf_uart3.buf[3]=='K'))
    {
        PUT("S_GSM_CHAR_MODE_RET_S OK \r\n");
        clear_buf_uart3();
        current_status = GSM_TEXT_MODE_FINISH;
    }
    else
    {
        PUT("S_GSM_CHAR_MODE_RET_S NO \r\n");
        clear_buf_uart3();
        send_string_uart3(GSM_CHAR_MODE);
    }
}


static void init_sms_to_send()
{
    current_status = GSM_SEND_TEXT_S;

    while(1)
    {
        err ++;
        if(err>MAX_ERROR_COUNT)
        {
            err = 0 ;
            break;
        }
        delay_GSM(1000);
        switch(current_status)
        {
        case GSM_SEND_TEXT_S:
            S_GSM_MSG_MODE_S();
            break ;

        case GSM_MSG_MODE_S:
            S_GSM_CHAR_MODE_S();
            break ;

        case GSM_CHAR_MODE_S:
            S_GSM_CHAR_MODE_RET_S();
            break ;

        case GSM_TEXT_MODE_FINISH:
            break ;
        default:
            clear_buf_uart3();
            break ;
        }
    }
}

void send_gsm_text(unsigned char * buf , unsigned int count)
{
    unsigned int i ;
    char send_sms_cmd_num[SMS_CMD_LEN];
    memset((void*)send_sms_cmd_num,0,SMS_CMD_LEN);

    if(!is_gprs_mode_ok)
        return ;

    init_sms_to_send();
    if(current_status != GSM_TEXT_MODE_FINISH)
        return ;

    PUT((char *)buf);

    PUT("\r\nTEL-\r\n");
    PUT(tpy_send_tel_num);
    PUT("-END\r\n");

    if(strlen(tpy_send_tel_num) != 11)
        send_string_uart3(GSM_SEND_TEXT);
    else
    {
        sprintf(send_sms_cmd_num,"AT+CMGS=\"+86%s\"\x00D\x00A",tpy_send_tel_num);
        PUT(send_sms_cmd_num);
        send_string_uart3(send_sms_cmd_num);
    }
    delay_GSM(100);
    for( i = 0 ;i < count ; i ++)
    {
        for ( i = 0 ; i < strlen((const char*)buf) ; i ++)
        {
            send_data_uart3(buf[i]);
        }
    }

    delay_GSM(100);
    send_data_uart1(0X1A);
    delay_GSM(5000);
}



unsigned char is_gprs_mode_status_ok(void)
{
    return is_gprs_mode_ok;
}

