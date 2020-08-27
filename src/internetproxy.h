#ifndef INTERNETPROXY_H
#define INTERNETPROXY_H

int SetInternetProxy(const char* WPAD, const char* acu, const char* proxy, const char* bypass, int enable);
inline int DisableInternetProxy(){return SetInternetProxy(NULL, NULL, NULL, NULL, 0);}

#endif
