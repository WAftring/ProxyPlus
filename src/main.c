#include <windows.h>
#include <stdio.h>
#include "nla.h"
#include "internetproxy.h"
#include "systemproxy.h"
#include "ini.h"

//Turning this into a service
//https://www.codeproject.com/Articles/499465/Simple-Windows-Service-in-Cplusplus

int InitialSetup();
int MonitorStateChange();
int main(){

	ini_t *config;
	char* ConfigProxy;
	char* ConfigBypassList;
	int WPAD = 0;
	int Inet = 0;
	int WinHttp = 0;
	char* acu;
	char* inetproxy;
	char* inetbypass;
	char* systemproxy;
	char* systembypass;

	if(InitialSetup() != 0){
		return -1;
	}

	config = ini_load("config.ini");

	if(config == NULL){
		printf("Failed to load config file\n");
		return -1;
	}
	
	if(ini_get(config, "wininet", "WPAD")){
		WPAD = 1;
		Inet = 1;
	}

	// config variable loading
	if(ini_get(config, "wininet", "auto-config-url")){
		acu = malloc(sizeof(ini_get(config, "wininet", "auto-config-url")));
		ini_sget(config, "wininet", "auto-config-url", "%s", acu);
		Inet = 1;
	}
	if(ini_get(config, "wininet", "proxy")){
		inetproxy = (char*)malloc(sizeof(ini_get(config, "wininet", "proxy")));
		ini_sget(config, "wininet", "proxy", "%s", inetproxy);
		Inet = 1;
	}
	if(ini_get(config, "wininet", "bypass-list")){
		inetbypass = calloc(0, sizeof(ini_get(config, "wininet", "bypass-list")));
		ini_sget(config, "wininet", "bypass-list", "%s", inetbypass);
	}
	if(ini_get(config, "winhttp", "proxy")){
		systemproxy = (char*)malloc(sizeof(ini_get(config, "winhttp", "proxy")));
		ini_sget(config, "winhttp", "proxy", "%s", systemproxy);
		WinHttp = 1;
	}
	if(ini_get(config, "winhttp", "bypass-list")){
		systembypass = (char*)malloc(sizeof(ini_get(config, "winhttp", "bypass-list")));
		ini_sget(config, "winhttp", "bypass-list", "%s", systembypass);
	}

	MonitorNLAState();	
//	if(Inet)
//		SetInternetProxy(WPAD, acu, inetproxy, inetbypass, 1);
//
//	if(WinHttp)
//		SetSystemProxy(systemproxy, systembypass, 1);

	if(acu)
		free(acu);
	if(inetproxy)
		free(inetproxy);
	if(inetbypass)
		free(inetbypass);
	if(systemproxy)
		free(systemproxy);
	if(systembypass)
		free(systembypass);
	if(config)
		ini_free(config);

//	DisableSystemProxy();
//	DisableInternetProxy();

	return 0;
}

int MonitorStateChange(int condition){
	
}

int InitialSetup(){
	/* We need to go to the directory that contains the ini since the
	   default is the call of the exe */
	char buffer[MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	if(GetModuleFileNameA(NULL, buffer, MAX_PATH) == 0){
		printf("Unable to determine path.\nCan't read config.\n");
		return -1;
	}

	_splitpath(buffer, drive, dir, fname, ext);
	memset(&buffer[0],0,sizeof(buffer));
	_makepath(buffer, drive, dir, NULL, NULL);

	if(SetCurrentDirectory(buffer) == 0){
		printf("Failed to change directory to executable path with error: %d\n", GetLastError());
		return -1;
	}
	
	return 0;
}
