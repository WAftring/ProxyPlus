#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#include <winsock2.h>
#include <Mswsock.h>
#include <iphlpapi.h>
#include "common.h"
#include "proxy.h"

#define wsaerrno WSAGetLastError()
// https://docs.microsoft.com/en-us/previous-versions//aa364726(v=vs.85)?redirectedfrom=MSDN

int GetInterfaceCount();

int NLANotify(){
    
	// Thinking how to implement this...
	//
	// We need to Call WSALookupServiceBegin to get our NLA Handle
	// https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsalookupservicebegina
	//
	// We then WSALookupServiceNext
	// https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsalookupservicenexta
	//
	// If there is no change it returns WSA_E_NO_MORE
	// else, it returns 0 and populates the WSAQUERYSET
	//
	// From there we can check our domain connectivity
	
	//@TODO There is a lot more to take a look at here...
	
	WSADATA WSD;
	char buff[16384];
	HANDLE hNLA;
	DWORD BufferSize;
	WSAQUERYSET* QuerySet = NULL;
	GUID NLANameSpaceGUID = NLA_SERVICE_CLASS_GUID;
	PNLA_BLOB pNLA;
	DWORD Offset = 0;
	int *NetworkStateArray;
	int NumAdapters = 0;
	int iterator = 0;
	int StateChanged = 1;
    
	// Starting WSA
	if(WSAStartup(MAKEWORD(2,2),&WSD) != 0){
		//@TODO Turn this into a fatal error and debug
                log_error("Failed to start Winsock with: %d", wsaerrno);
		return -1;
	}
    
	QuerySet = (WSAQUERYSET *)buff;
	memset(QuerySet, 0, sizeof(*QuerySet));
	
	QuerySet->dwSize = sizeof(WSAQUERYSET);
	QuerySet->dwNameSpace = NS_NLA;
	QuerySet->lpServiceClassId = &NLANameSpaceGUID;
	QuerySet->dwNumberOfProtocols = 0;
	QuerySet->lpszServiceInstanceName = NULL;
	QuerySet->lpVersion = NULL;
	QuerySet->lpNSProviderId = NULL;
	QuerySet->lpszContext = NULL;
	QuerySet->lpafpProtocols = NULL;
	QuerySet->lpszQueryString = NULL;
	QuerySet->lpBlob = NULL;
    
	if(WSALookupServiceBegin(QuerySet,LUP_RETURN_ALL,&hNLA) != 0){
                log_fatal("NLA namespace search failed with: %d", wsaerrno);
		return -1;
	}
	
	// This is for the inital adapter number
	NumAdapters = GetInterfaceCount(); 	
	if(NumAdapters < 1)
		return -1;
    
	NetworkStateArray = (int*)calloc(NumAdapters, sizeof(int));	
	
	while(1){
		
		memset(QuerySet, 0, sizeof(*QuerySet));
		BufferSize = sizeof(buff);
		if(WSALookupServiceNext(hNLA,LUP_RETURN_ALL,&BufferSize,QuerySet) != 0){
            
			// This just means we don't have new data
			if(wsaerrno == WSA_E_NO_MORE){
                                log_debug("No more data found. Waiting for state change");
				if(StateChanged){
                                        log_debug("Entering SetProxyNLA");
					SetProxyNLA(NetworkStateArray);
					if(NetworkStateArray)
						free(NetworkStateArray);
				}
                
				StateChanged = 0;
				iterator = 0;
			}
			else if(wsaerrno == WSA_INVALID_HANDLE){
                                    log_debug("WSALookupServiceInvalid handle");
                                    return -1;
                        }
			
			else{
                                log_debug("WSALookupService unexpected error: %d", wsaerrno);
				return -1;
			}
		}
		// We succeeded in polling NLA and have new data
		else{
			if(StateChanged == 0){
				StateChanged = 1;
				NumAdapters = GetInterfaceCount();
				NetworkStateArray = (int*)calloc(0, sizeof(int)*NumAdapters);
			}
            
                        log_debug("Network name: %s", QuerySet->lpszServiceInstanceName);
			if(QuerySet->lpBlob != NULL){
				do
				{
					pNLA = (PNLA_BLOB) &(QuerySet->lpBlob->pBlobData[Offset]);
					if(pNLA->header.type == NLA_CONNECTIVITY){
                                                log_debug("NLA Connectivity type: %d", pNLA->data.connectivity.type);
						NetworkStateArray[iterator] = pNLA->data.connectivity.type;
						iterator++;
					}
					Offset += pNLA->header.nextOffset;
                    
				}while(pNLA->header.nextOffset != 0);
				Offset = 0;
			}
		}
	}
    
	if(NetworkStateArray)
		free(NetworkStateArray);
    
	return 0;
}


int GetInterfaceCount(){
    //This function may be unnecessary?
	PIP_INTERFACE_INFO pInfo = NULL;
	ULONG ulInterfaceBuffer;
	int RetVal = -1;
	
	if(GetInterfaceInfo(
                        NULL,
                        &ulInterfaceBuffer) == ERROR_INSUFFICIENT_BUFFER){
		pInfo = (IP_INTERFACE_INFO*)malloc(ulInterfaceBuffer);	
		if(GetInterfaceInfo(
                            pInfo,
                            &ulInterfaceBuffer) == NO_ERROR){
            
			RetVal = pInfo->NumAdapters;
		}
	}
	else
                log_error("GetInterfaceInfo failed with %lu", GetLastError());
    
	if(pInfo)
		free(pInfo);
    
	return RetVal;
}
