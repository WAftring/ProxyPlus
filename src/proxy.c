//@TODO This will be a wrapper around the internetproxy.c
// and systemproxy.c

#include <windows.h>
#include "ini.h"
#include "proxy.h"
#include "systemproxy.h"
#include "internetproxy.h"


//@TODO This function will be changed
int SetProxyNLA(int *networkstatearray){


	int inDomain = 0;
	ini_t *proxyconfig;	
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

	//@TODO We need to check the array of the number of adapters and the connectivity state they are in	
	for(int i = 0; i <= sizeof(networkstatearray)/sizeof(int); i++){
		if(networkstatearray[i] == NETWORK_MANAGED)
			inDomain = 1;
	}

	if(inDomain){

		proxyconfig = ini_load("config.ini");

		if(proxyconfig == NULL){
			printf("Failed to load config file\n");
			return -1;
		}

		if(ini_get(proxyconfig, "wininet", "WPAD")){
			WPAD = 1;
			Inet = 1;
		}

		// @TODO Something in here is causing heap corruption...
		// config variable loading
		if(ini_get(proxyconfig, "wininet", "auto-config-url")){
			acu = (char*)malloc(sizeof(char) * strlen(ini_get(proxyconfig, "wininet", "auto-config-url")));
			ini_sget(proxyconfig, "wininet", "auto-config-url", "%s", acu);
			Inet = 1;
		}
		if(ini_get(proxyconfig, "wininet", "proxy")){
			inetproxy = (char*)malloc(sizeof(char) * strlen(ini_get(proxyconfig, "wininet", "proxy")));
			ini_sget(proxyconfig, "wininet", "proxy", "%s", inetproxy);
			Inet = 1;
		}
		if(ini_get(proxyconfig, "wininet", "bypass-list")){
			inetbypass = (char*)malloc(sizeof(char) * strlen(ini_get(proxyconfig, "wininet", "bypass-list")));
			ini_sget(proxyconfig, "wininet", "bypass-list", "%s", inetbypass);
		}
		if(ini_get(proxyconfig, "winhttp", "proxy")){
			systemproxy = (char*)malloc(sizeof(char) * strlen(ini_get(proxyconfig, "winhttp", "proxy")));
			ini_sget(proxyconfig, "winhttp", "proxy", "%s", systemproxy);
			WinHttp = 1;
		}
		if(ini_get(proxyconfig, "winhttp", "bypass-list")){
			systembypass = (char*)malloc(sizeof(char) * strlen(ini_get(proxyconfig, "winhttp", "bypass-list")));
			ini_sget(proxyconfig, "winhttp", "bypass-list", "%s", systembypass);
		}

		if(Inet)
			SetInternetProxy(WPAD, acu, inetproxy, inetbypass, 1);	

		if(WinHttp)
			SetSystemProxy(systemproxy, systembypass, 1);	

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
	}

	return 1;
}

