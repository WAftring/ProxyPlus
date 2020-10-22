//This is really just a wrapper around
//systemproxy.c and internetproxy.c
#include "common.h"
#include "ini.h"
#include "proxy.h"
#include "systemproxy.h"
#include "internetproxy.h"


int SetProxyNLA(int domain_status){

	ini_t *proxyconfig = NULL;
	int Inet = 0;
	int WinHttp = 0;

	// We need to check the array of the number of adapters and the connectivity state they are in	
	if(domain_status == NETWORK_MANAGED){

		proxyconfig = ini_load("config.ini");

		if(proxyconfig == NULL){
			log_fatal("Failed to load config file");
			return -1;
		}

		// config variable loading
		const char* WPAD = ini_get(proxyconfig, "wininet", "wpad");
		const char* acu = ini_get(proxyconfig, "wininet", "auto-config-url");
		const char* inetproxy = ini_get(proxyconfig, "wininet", "proxy");
		const char* inetbypass = ini_get(proxyconfig, "wininet", "bypass-list");
		const char* systemproxy = ini_get(proxyconfig, "winhttp", "proxy");
		const char* systembypass = ini_get(proxyconfig, "winhttp", "bypass-list");
		
		if(acu || inetproxy || WPAD)
			Inet = 1;
		if(systemproxy)
			WinHttp = 1;

		if(Inet){
			log_info("Setting WinInet proxy");
			SetInternetProxy(WPAD, acu, inetproxy, inetbypass, 1);
		}

		if(WinHttp){
			log_info("Setting WinHttp proxy");
			SetSystemProxy(systemproxy, systembypass, 1);
		}

	}
	else {
		log_info("Disabling WinInet proxy");
		DisableInternetProxy();
		log_info("Disabling WinHttp proxy");
		DisableSystemProxy();
	}

	ini_free(proxyconfig);
	return 1;
}

