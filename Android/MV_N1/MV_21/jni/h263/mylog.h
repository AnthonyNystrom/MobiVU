#ifndef _MYLOG
#define _MYLOG

#include <stdio.h>

class CMyLog{
	FILE *_fpLog;
	char _sLog[255];
public:
	CMyLog();
	~CMyLog();
	void EnableLog();
	void DisableLog();
	void WriteLog(char *pStrCaller, char *pStr);
};
extern CMyLog myLog;

#endif _MYLOG
