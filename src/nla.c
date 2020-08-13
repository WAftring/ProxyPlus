#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <Mswsock.h>
#include <stdio.h>

#define wsaerrno WSAGetLastError()
//#include <winhttp.h>
//#pragma comment(lib, "winhttp.lib")
// https://docs.microsoft.com/en-us/previous-versions//aa364726(v=vs.85)?redirectedfrom=MSDN

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
