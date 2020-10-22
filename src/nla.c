#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#include <winsock2.h>
#include <Mswsock.h>
#include <iphlpapi.h>
#include "common.h"
#include "proxy.h"

#define wsaerrno WSAGetLastError()
#define MAX_ADAPTER 12

typedef struct {
	char adapter_guid[40];
	int domain_connectivity;
	int active;
} NetAdapter;


typedef struct dynamic_vector {
	NetAdapter* data;
	size_t limit;
	size_t current;
} nvector;

void UpdateVector(nvector* vNetAdapter, NetAdapter* newAdapter, int bAdd);
void UpdateAdapterVector(nvector* vNetAdapter, char* adapter_guid, const int* domain_connectivity, int bAdded);
int GetSignificantStatus(nvector* vNetAdapter);
int NLANotify(){

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

	WSADATA WSD;
	char buff[16384];
	HANDLE hNLA;
	DWORD BufferSize;
	WSAQUERYSET* QuerySet = NULL;
	GUID NLANameSpaceGUID = NLA_SERVICE_CLASS_GUID;
	PNLA_BLOB pNLA;
	DWORD Offset = 0;
	DWORD lpcbBytesReturned = 0;
	nvector vNetAdapter;
	int iterator = 0;
	int domainTemp = 0;
	int StateChanged = 1;

	// Starting WSA
	if(WSAStartup(MAKEWORD(2,2),&WSD) != 0){
		log_fatal("Failed to start Winsock with: %d", wsaerrno);
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

	vNetAdapter.data = (NetAdapter*)calloc(1, sizeof(NetAdapter));
	vNetAdapter.limit = 1;
	vNetAdapter.current = 0;

	while(1){

		memset(QuerySet, 0, sizeof(*QuerySet));
		BufferSize = sizeof(buff);
		// If we detect a change it will tell us
		// If something was added or removed
		// This means I need to know what is left...
		if(WSALookupServiceNext(hNLA,LUP_RETURN_BLOB,&BufferSize,QuerySet) != 0){

			// This just means we don't have new data
			if(wsaerrno == WSA_E_NO_MORE){
				if(StateChanged && vNetAdapter.current != 0){
					log_debug("Entering SetProxyNLA");
					SetProxyNLA(GetSignificantStatus(&vNetAdapter));
				}

				// For single-threaded applications, a typical method to use the WSANSPIoctl function
				// is as follows. Call the WSANSPIoctl function with the dwControlCode parameter
				// set to SIO_NSP_NOTIFY_CHANGE with no completion routine
				// (the lpCompletion parameter set to NULL) after every WSALookupServiceNext
				// function call to make sure the query data is still valid. If the data becomes invalid,
				// call the WSALookupServiceEndfunction to close the query handle.
				// Call the WSALookupServiceBegin function to retrieve a new query handle and begin the query again.
				//
				// This will block until a network state change occurs
				// It will start processing before the domain connectivity can be determined...

				log_info("No more data found. Waiting for state change");
				if(WSANSPIoctl(
				  hNLA,
				  SIO_NSP_NOTIFY_CHANGE,
				  NULL,0,NULL,0,
				  &lpcbBytesReturned,
				  NULL
				) != NO_ERROR){
					log_error("Failed to set the WSANSPIoctl: %d", wsaerrno);
				}
				log_info("NLA indicated state change");
				if(lpcbBytesReturned)
					free(&lpcbBytesReturned);
				// We need this to give the NLA time to contact a DC
				log_debug("Starting sleep for domain connectivity");
				Sleep(3000); 

				StateChanged = 0;
				iterator = 0;
			}
			else if(wsaerrno == WSA_INVALID_HANDLE){
				log_fatal("WSALookupServiceInvalid handle");
				return -1;
			}

			else{
				log_fatal("WSALookupService unexpected error: %d", wsaerrno);
				return -1;
			}
		}
		// We succeeded in polling NLA and have new data
		// Additionally based in the WSAQUERYSET we need
		// to check the dwOutputFlag for if the item was
		// added or removed
		else{
			if(StateChanged == 0){
				StateChanged = 1;
			}

			if(QuerySet->lpBlob != NULL){
				do
				{
					log_debug("Query Result: %lu", QuerySet->dwOutputFlags);
					pNLA = (PNLA_BLOB) &(QuerySet->lpBlob->pBlobData[Offset]);
					if(pNLA->header.type == NLA_INTERFACE){
						log_info("NLA Adapter Name: %s", pNLA->data.interfaceData.adapterName);
						UpdateAdapterVector(&vNetAdapter,
											pNLA->data.interfaceData.adapterName,
											&domainTemp,
											QuerySet->dwOutputFlags);
						}

					else if(pNLA->header.type == NLA_CONNECTIVITY){
						log_info("NLA Connectivity type: %d", pNLA->data.connectivity.type);
						domainTemp = pNLA->data.connectivity.type;
					}
					Offset += pNLA->header.nextOffset;

				}while(pNLA->header.nextOffset != 0);
				Offset = 0;
			}
		}
	}

	if(vNetAdapter.data)
		free(vNetAdapter.data);
	return 0;
}

void UpdateAdapterVector(nvector* vNetAdapter, char* adapter_guid, const int* domain_connectivity, int bAdded){

	int bFound = 0;
	int location = 0;
	NetAdapter tempAdapter;

	memcpy_s(tempAdapter.adapter_guid, sizeof(tempAdapter.adapter_guid), adapter_guid, strlen(adapter_guid));
	tempAdapter.domain_connectivity = *domain_connectivity;
	tempAdapter.active = 0;

	if(bAdded == RESULT_IS_ADDED || bAdded == RESULT_IS_CHANGED) //Adapters can be added, removed or deleted
		tempAdapter.active = 1;


	for (int i = 0; i < vNetAdapter->current; i++) {
		if(strcmp(vNetAdapter->data[i].adapter_guid,adapter_guid) == 0){
			bFound = 1;
			location = i;
			break;
		}
	}

	if(bFound){
		vNetAdapter->data[location] = tempAdapter;
	}
	else
		UpdateVector(vNetAdapter, &tempAdapter, bAdded);

}

void UpdateVector(nvector* vNetAdapter, NetAdapter* newAdapter, int bAdd){

	if(bAdd){
		vNetAdapter->data[vNetAdapter->current] = *newAdapter;
		vNetAdapter->current++;
	}

	if(vNetAdapter->current == vNetAdapter->limit)
		vNetAdapter->data = (NetAdapter*)realloc(vNetAdapter->data, (vNetAdapter->limit + 3) * sizeof(NetAdapter));

}

int GetSignificantStatus(nvector* vNetAdapter){
	int sig_status = 0;

	for (int i = 0; i < vNetAdapter->current; i++)
		if (vNetAdapter->data[i].domain_connectivity == NETWORK_MANAGED && vNetAdapter->data[i].active == 1)
			sig_status = NETWORK_MANAGED;

	return sig_status;
}
