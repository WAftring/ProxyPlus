#ifndef INTERNETPROXY_H
#define INTERNETPROXY_H

int SetInternetProxy(int WPAD, char* acu, char* proxy, char* bypass, int enable);
inline int DisableInternetProxy(){return SetInternetProxy(0, NULL, NULL, NULL, 0);}

#endif
