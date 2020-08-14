#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include "ini.h"

#pragma comment(lib, "winhttp.lib")

int InitialSetup();

int main(){
	ini_t *config;
	if(InitialSetup() != 0){
		return -1;
	}
	config = ini_load("config.ini");
	if(config == NULL){
		printf("Failed to load config file\n");
		return -1;
	}
	printf("Getting the WinHttp Proxy config: %s\n", ini_get(config,"winhttp","proxy")); 
	WINHTTP_PROXY_INFO WinHttpProxyInfo;
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG WinInetProxyInfo; 
	int MakeChange = 1;
	// Getting the winhttp proxy
	
	WinHttpGetDefaultProxyConfiguration(&WinHttpProxyInfo);
	printf("WinHttp Proxy Config:\n");
	printf("\tAccess type: %d \n\tProxyString: %ls \n\tProxy ByPass: %ls\n", 
			WinHttpProxyInfo.dwAccessType, 
			WinHttpProxyInfo.lpszProxy, 
			WinHttpProxyInfo.lpszProxyBypass);
	WinHttpGetIEProxyConfigForCurrentUser(&WinInetProxyInfo);
	printf("WinInet Proxy Config:\n");
	printf("\tWPAD Enabled: %i \n\tAuto Config URL: %ls\n\tManual Proxy: %ls\n\tProxy Bypass: %ls\n", 
			WinInetProxyInfo.fAutoDetect, 
			WinInetProxyInfo.lpszAutoConfigUrl, 
			WinInetProxyInfo.lpszProxy,
			WinInetProxyInfo.lpszProxyBypass);

	if(MakeChange){
	}
	return 0;
}

int InitialSetup(){
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
