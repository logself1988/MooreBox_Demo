/*****************************************
 * @file  mb_platform.h
 * @brief This file contains the functions independent of the hardware
 * **************************************/
#ifndef MB_PLATFORM_H_H_H
#define MB_PLATFORM_H_H_H

#ifdef __cplusplus
	extern "C"{
#endif

typedef struct
{
	uint8_t  mb_action;
	uint32_t mb_id;
	uint64_t mb_password;
	uint16_t mb_index;

}mb_MsgSendTypeDef;

typedef struct
{
	uint8_t  mb_action;
	uint32_t mb_id;

}mb_CmdRecvTypeDef;


extern void mb_heartbeat(void);

extern int MsgSend(char *pMsg, uint32_t MsgLen, const char *ip, uint16_t port);




#ifdef __cplusplus
}
#endif

#endif
