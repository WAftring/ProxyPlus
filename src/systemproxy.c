#include "common.h"
#include <winhttp.h>
#include "systemproxy.h"

#pragma comment(lib, "winhttp.lib")

int SetSystemProxy(const  char* proxy, const char* bypass, int enable){

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

	}

	else{
		//TODO This is leading to an INVALID_PARAMETER argument
		SetProxy.dwAccessType = WINHTTP_ACCESS_TYPE_NO_PROXY;
	}

	if(!WinHttpSetDefaultProxyConfiguration(&SetProxy)){
		log_error("Failed to set WinHttp proxy with: %lu", GetLastError());
		retval = -1;
	}
	else{
		WinHttpGetDefaultProxyConfiguration(&SetProxy);
		log_debug("WinHttp proxy set to %s", proxy);
	}

	if(enable){
		free(ProxyBuffer);
		free(BypassBuffer);
	}

	return retval;
}
