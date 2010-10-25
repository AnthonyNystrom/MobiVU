#include "call_mylog.h"
#include "mylog.h"

void InitLog(){
	myLog.EnableLog();
}

void ReleaseLog(){
	myLog.DisableLog();
}
