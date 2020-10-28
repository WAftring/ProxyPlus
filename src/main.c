#include "common.h"
#include <string.h>
#include <share.h>
#include "nla.h"
#include "ini.h"

FILE* InitialSetup();
int ParseLogLevel(const char* LogLevel);

int main(){
	FILE* pFile; 
	ini_t *config;
	int LogInt = LOG_INFO;

	pFile = InitialSetup();

	config = ini_load("config.ini");

	if(config == NULL || pFile == NULL)
		return -1;

	const char* LogStr = ini_get(config, "general", "log-level");
	LogInt = ParseLogLevel(LogStr);

	log_add_fp(pFile, LogInt);

	const char* LogQuiet = ini_get(config, "general", "quiet");
	if(strcmp(LogQuiet, "TRUE") == 0)
		log_set_quiet(true);

	log_info("Logger started");

	//Reading the general config
	const char* StateCheck = ini_get(config, "general", "watch");

	//At some point this will need to be turned into a function
	if(strcmp(StateCheck, "NLA") == 0)
		NLANotify();
	else
		log_fatal("No proxy procedure matched");

	if(config)
		ini_free(config);

	fclose(pFile);

	return 0;
}


FILE* InitialSetup(){
	/* We need to go to the directory that contains the ini since the
	   default is the call of the exe */
	char buffer[MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	FILE* pTemp;
	errno_t err;

	if(GetModuleFileNameA(NULL, buffer, MAX_PATH) == 0){
		printf("Unable to determine path.\nCan't read config.\n");
		return NULL;
	}

	_splitpath(buffer, drive, dir, fname, ext);
	memset(&buffer[0],0,sizeof(buffer));
	_makepath(buffer, drive, dir, NULL, NULL);

	if(SetCurrentDirectory(buffer) == 0){
		printf("Failed to change directory to executable path with error: %d\n", GetLastError());
		return NULL;
	}

	pTemp = _fsopen("ProxyPlus.log", "a+", _SH_DENYNO);

	return pTemp;
}

int ParseLogLevel(const char* LogLevel){

	if(strcmp(LogLevel, "DEBUG") == 0)
		return LOG_DEBUG;
	else
		return LOG_WARN;

}
