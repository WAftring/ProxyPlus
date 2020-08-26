#include "common.h"
#include <string.h>
#include "nla.h"
#include "ini.h"

FILE* InitialSetup();
void ParseLogLevel(const char* LogLevel);

int main(){
        FILE* pFile; 
	ini_t *config;
        char* LogLevel;
        char* StateCheck;
        
        pFile = InitialSetup();
	if(pFile == NULL){
		return -1;
	}
        else{
            log_add_fp(pFile, LOG_INFO);
            log_set_quiet(true);
            log_info("Logger started");
        }
	config = ini_load("config.ini");

	if(config == NULL){
                log_fatal("Failed to read config file");
		return -1;
	}

        //Reading the general config
        LogLevel = (char*)malloc(sizeof(char) * strlen(ini_get(config, "general", "log-level")));
        ini_sget(config, "general", "log-level", "%s", LogLevel);
        ParseLogLevel(LogLevel);
        StateCheck = (char*)malloc(sizeof(char) * strlen(ini_get(config, "general", "watch")));
        ini_sget(config, "general", "watch", "%s", StateCheck);
        
        //At some point this will need to be turned into a function
        if(strcmp(StateCheck, "NLA") == 0)
            NLANotify();	
        else
            log_fatal("No proxy procedure matched");

        if(StateCheck)
            free(StateCheck);
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

        err = fopen_s(&pTemp, "ProxyPlus.log", "a");

        if(err != 0)
            return NULL;

	return pTemp;
}

void ParseLogLevel(const char* LogLevel){

	//@TODO This doesn't work right now...
    if(strcmp(LogLevel, "DEBUG") == 0)
        log_set_level(LOG_DEBUG);
    else
        log_set_level(LOG_INFO);

}
