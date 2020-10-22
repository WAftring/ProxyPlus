#include "common.h"
#include <wininet.h>
#include "internetproxy.h"

#pragma comment(lib, "wininet.lib")

int SetInternetProxy(const char* WPAD, const char* acu, const char* proxy, const char* bypass, int enable){

	int CurrentOption = 1;
	int OptionCount = 1;
	INTERNET_PER_CONN_OPTION_LIST list;
	DWORD dwSize = sizeof(list);

	if(WPAD){
		log_info("WPAD defined");
	}
	if(acu){
		log_info("Auto config URL defined");
		OptionCount++;
	}
	if(proxy){
		log_info("Manual proxy defined");
		OptionCount++;
		OptionCount++; //Adding an additional option count for the bypass list
	}

	INTERNET_PER_CONN_OPTION *Option = (INTERNET_PER_CONN_OPTION*)malloc(OptionCount * 
			sizeof(INTERNET_PER_CONN_OPTION));

	Option[0].dwOption = INTERNET_PER_CONN_FLAGS;
	Option[0].Value.dwValue = PROXY_TYPE_DIRECT;
	
	if(enable){
		log_info("Enabling WinInet proxy");	
		// Enable WPAD
		if(WPAD)
			Option[0].Value.dwValue |= PROXY_TYPE_AUTO_DETECT;
		// Enable ACU
		if(acu){
			Option[0].Value.dwValue |=  PROXY_TYPE_AUTO_PROXY_URL;
			// Set ACU
			Option[CurrentOption].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
			Option[CurrentOption].Value.pszValue = (LPSTR)TEXT(acu);
			CurrentOption++;
		}

		// Enable Proxy
		if(proxy){
			Option[0].Value.dwValue |= PROXY_TYPE_PROXY;
			// Set Manual Proxy
			Option[CurrentOption].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
			Option[CurrentOption].Value.pszValue = (LPSTR)TEXT(proxy);
			CurrentOption++;
			// Set bypass-list
			Option[CurrentOption].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
			Option[CurrentOption].Value.pszValue = (LPSTR)TEXT(bypass);
		}
	}

	else
		log_info("Disabling WinInet proxy");

	list.dwSize = sizeof(list);
	list.pszConnection = NULL;
	list.dwOptionCount = OptionCount;
	list.dwOptionError = 0;
	list.pOptions = Option;

	if(!InternetSetOptionA(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwSize)){
		log_error("Failed to set proxy with error: %lu", GetLastError());
		if(Option)
			free(Option);
		return -1;	
	}

	free(Option);
	Option = NULL;
	return 1;
}
