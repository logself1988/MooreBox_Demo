#ifndef _SIM300_H_
#define _SIM300_H_

//默认设置数据				  
#define SERNUM "0000001"
#define PROTOTOCOL "UDP"
#define IPADDR "lei-gong.vicp.cc"
#define PORTNUM "1001"
#define TELNUM "15922041115"
#define CACHESIZE 512

#define GPRS_SIMPLE 1

typedef enum {
    INIT0,
    CGMM,
    ECHO0,
    SIMCARD,
    RST,
    CREG,
    INIT_FINISH_OK,

    GPRS_JH_S,
    GPRS_DK_S ,
    GPRS_QD_S ,
    GPRS_OPEN_FINISH,       /// GPRS 打开成功了

    TCPIP_IO_MODE,
    TCPIP_BJ_ADDR,
    TCPIP_CONNECT,
    TCP_IP_OK,
    TCP_IP_NO,
    TCP_IP_CONNECTING,

    GPRS_DEF_PDP_S,         /// 定义PDP场景
    GPRS_ACT_PDP_S,         /// 激活场景
    GPRS_ACT_PDP_S_RET,     /// 激活反馈
    GPRS_CMNET_APN_S,       /// 接入模式

    TRANSPARENT_MODE_START,
    TRANSPARENT_MODE_DEF,
    TRANSPARENT_MODE_ACT,
    TRANSPARENT_MODE_IOMODE,
    TRANSPARENT_MODE_TYPE,
    TRANSPARENT_MODE_CONFIG,
    TRANSPARENT_MODE_CONNECT,
    TRANSPARENT_MODE_CONNECT_RET,
    TM_OK,
    TM_NO,

    TCPIP_CLOSE_START,
    TCPIP_CLOSE_DEF,
    TCPIP_CLOSE_ETCPIP,
    TCP_IP_CLOSE_OK,
    TCP_IP_CLOSE_NO,


    GSM_SEND_TEXT_S,
    GSM_MSG_MODE_S,
    GSM_CHAR_MODE_S,
    GSM_TEXT_MODE_FINISH,

    SMS_READ_MESSAGE_START_S,
    SMS_READ_CONTENT_S,
    READ_SMS_FINISH,

    INIT_GPS_FINISH_OK,

    SIM900_LOC_START,
    SIM900_GPRS_S,
    SIM900_APN_S,
    SIM900_PBR1_S,
    SIM900_PBR2_S,
    SIM900_LOC_S,
    SIM900_LOC_G_S,
    SIM900_LOC_FINISH


}SIM_STATUS ;

extern void init_gprs(void);
extern void init_gps_mode(void);
extern unsigned char is_gprs_mode_status_ok(void);

extern void open_gprs_simple(void);//gprs
extern void creat_tcp(void);

extern SIM_STATUS current_status ;
extern void send_gprs_data(char * buf ,unsigned int size);
extern void send_gsm_text(unsigned char * buf , unsigned int count);
extern void read_the_index_sms(void);
extern int check_num_of_sms(void);
extern void start_gprs_mode(void);

extern char g_gprs_conn_ok;	  			 //判断GPRS是否链接到服务器了
extern char ibusy  ;							 //判断是否处于发送数据忙的状态
extern void gprs_heart(char *w,int t);   //心跳包数据


#endif
