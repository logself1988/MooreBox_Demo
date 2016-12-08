/***************************************
 * @name: mb_platform.c
 * @brief::functions that independent of the hardware layer
***************************************/

#include "mb_platform.h"

typedef enum
{
	TimeStamp,
	BoxIndex,
	BoxPassword
}BoxContentType;

void mb_heartbeat()
{
}

char *ParsePrinter(BoxContentType content, char *RecvRX)
{
	int i=0,j=0;
	char *ParseContent;
	switch (content)
	{
		case TimeStamp:
			strncpy(ParseContent, (RecvRX+5), 14);
			break;
		case BoxIndex:
			strncpy(ParseContent, (RecvRX+20), 8);
			break;
		case BoxPassword:
			strncpy(ParseContent, (RecvRX+29), 11);
			break;
		default:
			break;
	}
	return ParseContent;
}
