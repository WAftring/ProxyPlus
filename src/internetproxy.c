#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include "internetproxy.h"

#pragma comment(lib, "wininet.lib")

int SetInternetProxy(int WPAD, char* acu, char* proxy, char* bypass, int enable){
	
	int OptionCount = 1;
	INTERNET_PER_CONN_OPTION_LIST list;
	DWORD dwSize = sizeof(list);

	if(WPAD)
		OptionCount++;
	if(acu)
		OptionCount++;
	if(proxy)
		OptionCount++;

	INTERNET_PER_CONN_OPTION *Option = (INTERNET_PER_CONN_OPTION*)malloc(OptionCount * 
			sizeof(INTERNET_PER_CONN_OPTION));

	Option[0].dwOption = INTERNET_PER_CONN_FLAGS;
	Option[0].Value.dwValue = PROXY_TYPE_DIRECT;

	if(enable){
		
		// Enable WPAD
		if(WPAD)
			Option[0].Value.dwValue |= PROXY_TYPE_AUTO_DETECT;
		// Enable ACU
		if(acu)
			Option[0].Value.dwValue |=  PROXY_TYPE_AUTO_PROXY_URL;
		// Enable Proxy
		if(proxy)
			Option[0].Value.dwValue |= PROXY_TYPE_PROXY;

		// Set Manual Proxy
		Option[1].dwOption = INTERNET_PER_CONN_PROXY_SERVER;
		Option[1].Value.pszValue = TEXT("http://localhost:2000");

		// Set bypass-list
		Option[2].dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
		Option[2].Value.pszValue = TEXT("<local>");

		// Set ACU
		Option[3].dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
		Option[3].Value.pszValue = TEXT("http://localhost:2001");

	}

	list.dwSize = sizeof(list);
	list.pszConnection = NULL;
	list.dwOptionCount = OptionCount;
	list.dwOptionError = 0;
	list.pOptions = Option;
	
	if(!InternetSetOptionA(NULL, INTERNET_OPTION_PER_CONNECTION_OPTION, &list, dwSize)){
		//@TODO Add this to a debug line
		printf("Failed to set proxy: %lu", GetLastError());
		if(Option)
			free(Option);
		return -1;	
	}

	if(Option)
		free(Option);

	return 1;
}
