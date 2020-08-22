#include <windows.h>
#include <winhttp.h>
#include <stdio.h>
#include "systemproxy.h"

#pragma comment(lib, "winhttp.lib")

int SetSystemProxy(char* proxy, char* bypass, int enable){
	int wchar_num = 0;
	wchar_t* ProxyBuffer = NULL;
	wchar_t* BypassBuffer = NULL;
	int retval = 0;

	WINHTTP_PROXY_INFO SetProxy;
	
	SetProxy.dwAccessType = WINHTTP_ACCESS_TYPE_NO_PROXY;

	if(enable){

		wchar_num = MultiByteToWideChar(CP_UTF8, 0, proxy, -1, NULL, 0);
		ProxyBuffer = (wchar_t*)malloc(sizeof(wchar_t)*wchar_num);
		MultiByteToWideChar(CP_UTF8, 0, proxy, -1, ProxyBuffer, wchar_num);
		SetProxy.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;	
		SetProxy.lpszProxy = ProxyBuffer;

		wchar_num = MultiByteToWideChar(CP_UTF8, 0, bypass, -1, NULL, 0);
		BypassBuffer = (wchar_t*)malloc(sizeof(wchar_t)*wchar_num);
		MultiByteToWideChar(CP_UTF8, 0, bypass, -1, BypassBuffer, wchar_num);
		SetProxy.lpszProxyBypass = BypassBuffer;

		free(ProxyBuffer);
		free(BypassBuffer);
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

	return retval;
}
