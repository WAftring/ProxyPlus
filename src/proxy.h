#ifndef PROXY_H
#define PROXY_H

// Pulled from mswsock.h
typedef enum CONNECTIVITY_TYPE {
    NETWORK_AD_HOC    = 0,
    NETWORK_MANAGED   = 1,
    NETWORK_UNMANAGED = 2,
    NETWORK_UNKNOWN   = 3,
} CONNECTIVITY_TYPE;

int SetProxyNLA(int *networkstatearray);

#endif
