#ifndef WINHTTPPROXY_H
#define WINHTTPPROXY_H

#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

bool SetWinHttpProxy(char* proxy, char* bypass, bool enable);

bool SetWinHttpProxy(char* proxy, char* bypass, bool enable){
	wchar_t* ProxyBuffer;
	wchar_t* BypassBuffer;
	bool retval = false;
	WINHTTP_PROXY_INFO SetProxy;

	if(enable){
		mbstowcs_s(NULL, ProxyBuffer, 256, ConfigProxy, 256);
		SetProxy.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;	
		SetProxy.lpszProxy = ProxyBuffer;
		mbstowcs_s(NULL, BypassBuffer, 256, ConfigBypassList, 256);
		SetProxy.lpszProxyBypass = BypassBuffer;

		if(WinHttpSetDefaultProxyConfiguration(&SetProxy)){
			WinHttpGetDefaultProxyConfiguration(&SetProxy);
			//@TODO add this to a debug line
			printf("WinHttp Proxy:\n");
			printf("\tAccess type: %d \n\tProxyString: %ls \n\tProxy ByPass: %ls\n", 
					SetProxy.dwAccessType, 
					SetProxy.lpszProxy, 
					SetProxy.lpszProxyBypass);
			retval = true;
		}
		else
			printf("Failed to set Winhttp proxy with: %lu\n", GetLastError());

		free(ProxyBuffer);
		free(BypassBuffer);
	}
	else {
		SetProxy.dwAccessType = WINHTTP_ACCESS_TYPE_NO_PROXY;
		//@TODO Add this to a debug line
		if(WinHttpSetDefaultProxyConfiguration(&SetProxy))
			retval = true;
		else
			printf("Failed to reset Winhttp proxy with: %lu\n", GetLastError());
	}
	return retval;
}

#endif
