#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <stdbool.h>
#include "ini.h"

#pragma comment(lib, "wininet.lib")
//Turning this into a service
//https://www.codeproject.com/Articles/499465/Simple-Windows-Service-in-Cplusplus
bool InitialSetup();

int main(){

	ini_t *config;
	char* ConfigProxy;
	char* ConfigBypassList;
	INTERNET_PER_CONN_OPTION OptionList;
	DWORD dwBufferLength = sizeof(OptionList);
	
	OptionList.dwSize = sizeof(list);
	OptionList.pszConnection = NULL;
	OptionList.dwOptionCount = 4;
	OptionList.pOptions = new INTERNET_PER_CONN_OPTION[3];

	if(NULL == OptionList.pOptions){
		printf("Stuff failed...\n");
		return -1;
	}
	//Options we want
	//INTERNET_PER_CONN_PROXY_SERVER
	//INTERNET_PER_CONN_PROXY_BYPASS
	//INTERNET_PER_CONN_FLAGS
	//INTERNET_PER_CONN_AUTOCONFIG_URL

	if(InitialSetup() != 0){
		return -1;
	}

	config = ini_load("config.ini");

	if(config == NULL){
		printf("Failed to load config file\n");
		return -1;
	}
	
	if(!InternetQueryOptionA(NULL, 
			INTERNET_OPTION_PER_CONNECTION_OPTION,
			&OptionList,
			&dwBufferLength)){
		printf("Getting inet proxy failed with: %lu\n", GetLastError());
		return -1;
	}

	return 0;
}

bool InitialSetup(){
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
