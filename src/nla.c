#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <Mswsock.h>
#include <stdio.h>

#define wsaerrno WSAGetLastError()
// https://docs.microsoft.com/en-us/previous-versions//aa364726(v=vs.85)?redirectedfrom=MSDN

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

	// Starting WSA
	if(WSAStartup(MAKEWORD(2,2),&WSD) != 0){
		//@TODO Turn this into a fatal error and debug
		printf("WSAStartup failde with: %d\n", wsaerrno);
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
		//@TODO Turn this into a debug line and make fatal
		printf("WSALookupServiceBegin failed with: %d\n", wsaerrno);
		return -1;
	}

	while(1){
		memset(QuerySet, 0, sizeof(*QuerySet));
		BufferSize = sizeof(buff);
		if(WSALookupServiceNext(hNLA,LUP_RETURN_ALL,&BufferSize,QuerySet) != 0){

			// This just means we don't have new data
			if(wsaerrno == WSA_E_NO_MORE){
				//@TODO turn this into a debug line
				printf("No more data\n");
				WSALookupServiceEnd(hNLA);
				break;
			}
			else{
				//@TODO turn this into a debug line
				printf("WSALookupServiceNext failed with: %d\n", wsaerrno);
				return -1;
			}
		}
		// We succeeded in polling NLA and have new data
		else{
			printf("\nNetwork Name: %s\n", QuerySet->lpszServiceInstanceName);
			printf("Comment (if any): %s\n", QuerySet->lpszComment);
			if(QuerySet->lpBlob != NULL){
				do
				{
					pNLA = (PNLA_BLOB) &(QuerySet->lpBlob->pBlobData[Offset]);
					if(pNLA->header.type == NLA_CONNECTIVITY){
						printf("\tNLA Data Type: NLA_CONNECTIVITY\n");
						printf("\t\t%d\n", pNLA->data.connectivity.type);
					}
					Offset = pNLA->header.nextOffset;
				}while(Offset != 0);

				//SetProxy(pNLA->data.connectivity.type);
			}
		}
	}

	return 0;
}

int CheckNLAState(){
	WSADATA WSD;
	char buff[16384];
	HANDLE hNLA;
	DWORD BufferSize;
	WSAQUERYSET* QuerySet = NULL;
	GUID NLANameSpaceGUID = NLA_SERVICE_CLASS_GUID;
	PNLA_BLOB pNLA;
	DWORD Offset = 0;

	if(WSAStartup(MAKEWORD(2,2),&WSD) != 0){
		printf("WSAStartup failde with: %d\n", wsaerrno);
		return -1;
	}

	//Allocating memory for the read
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
		printf("WSALookupServiceBegin failed with: %d\n", wsaerrno);
		return -1;
	}
	else {
	
		while(1){
			memset(QuerySet, 0, sizeof(*QuerySet));
			BufferSize = sizeof(buff);
			if(WSALookupServiceNext(hNLA,LUP_RETURN_ALL,&BufferSize,QuerySet) != 0){

				if(wsaerrno == WSA_E_NO_MORE){
					printf("No more data\n");
					WSALookupServiceEnd(hNLA);
					break;
				}
				else{
					printf("WSALookupServiceNext failed with: %d\n", wsaerrno);
					return -1;
				}
			}

			else{
				printf("\nNetwork Name: %s\n", QuerySet->lpszServiceInstanceName);
				printf("Comment (if any): %s\n", QuerySet->lpszComment);
				if(QuerySet->lpBlob != NULL){
					do
					{
						pNLA = (PNLA_BLOB) &(QuerySet->lpBlob->pBlobData[Offset]);
						if(pNLA->header.type == NLA_CONNECTIVITY){
							printf("\tNLA Data Type: NLA_CONNECTIVITY\n");
							switch(pNLA->data.connectivity.type){
								case NLA_NETWORK_AD_HOC:
									printf("\t\tNetwork Type: AD HOC\n");
									break;
								case NLA_NETWORK_MANAGED:
									printf("\t\tNetwork Type: MANAGED\n");
									break;
								case NLA_NETWORK_UNMANAGED:
									printf("\t\tNetwork Type: UNMANAGED\n");
									break;
								case NLA_NETWORK_UNKNOWN:
									printf("\t\tNetwork Type: UNKNOWN\n");
									break;
							}
						}
						Offset = pNLA->header.nextOffset;
					}while(Offset != 0);
				}
			}
		}				
	}
	return 0;
}
