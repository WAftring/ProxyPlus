#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

#pragma comment(lib, "winhttp.lib")
// https://docs.microsoft.com/en-us/previous-versions//aa364726(v=vs.85)?redirectedfrom=MSDN

int ReadAppConfig();


int main(){
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

int ReadAppConfig(){
}
