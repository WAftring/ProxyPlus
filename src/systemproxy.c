#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include "systemproxy.h"

#pragma comment(lib, "winhttp.lib")

int SetSystemProxy(char* proxy, char* bypass, int enable){
	wchar_t* ProxyBuffer;
	wchar_t* BypassBuffer;
	int retval = 0;
	WINHTTP_PROXY_INFO SetProxy;
	
	SetProxy.dwAccessType = WINHTTP_ACCESS_TYPE_NO_PROXY;

	if(enable){
		// @TODO
		// Here is where our heap corruption is coming from.
		// Lots to do to fix this
		ProxyBuffer = (wchar_t*)malloc(sizeof(proxy));
		BypassBuffer = (wchar_t*)malloc(sizeof(bypass));
		swprintf_s(ProxyBuffer, sizeof(ProxyBuffer), L"%s", proxy);
		mbstowcs_s(NULL, ProxyBuffer, 256, proxy, 256);
		SetProxy.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;	
		SetProxy.lpszProxy = ProxyBuffer;
		mbstowcs_s(NULL, BypassBuffer, 256, bypass, 256);
		SetProxy.lpszProxyBypass = BypassBuffer;
	}

	//@TODO TEMP While testing NLA functionality
//	if(!WinHttpSetDefaultProxyConfiguration(&SetProxy)){
//		//@TODO Add this to a debug line
//		printf("Failed to set Winhttp proxy with: %lu\n", GetLastError());
//		retval = -1;
//	}
//	else{
//		//@TODO add this to a debug line
//		WinHttpGetDefaultProxyConfiguration(&SetProxy);
//		printf("WinHttp Proxy:\n");
//		printf("\tAccess type: %d \n\tProxyString: %ls \n\tProxy ByPass: %ls\n", 
//				SetProxy.dwAccessType, 
//				SetProxy.lpszProxy, 
//				SetProxy.lpszProxyBypass);
//	}

	//@IMPORTANT fix this
	//This is causing heap corruption...
	if(ProxyBuffer)
		free(ProxyBuffer);
	if(BypassBuffer)
		free(BypassBuffer);

	return retval;
}
