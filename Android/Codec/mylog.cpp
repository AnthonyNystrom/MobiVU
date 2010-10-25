#include "mylog.h"
#include <android/log.h>


CMyLog myLog;

CMyLog::CMyLog(){
	_fpLog=NULL;
}

CMyLog::~CMyLog(){
	if (_fpLog)
		fclose(_fpLog);
}

void CMyLog::EnableLog(){
	_fpLog=fopen("/sdcard/encdec.log","wb");
}

void CMyLog::DisableLog(){
	fclose(_fpLog);
	_fpLog=NULL;
}

void CMyLog::WriteLog(char *pStrCaller, char *pStr){
	if (_fpLog){
		sprintf(_sLog, "%s-%s\n", pStrCaller, pStr);
		
		fputs(pStr,_fpLog);
		__android_log_write(ANDROID_LOG_DEBUG, pStrCaller, pStr);
	}
}
