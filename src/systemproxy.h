#ifndef WINHTTPPROXY_H
#define WINHTTPPROXY_H

int SetSystemProxy(char* proxy, char* bypass, int enable);
inline int DisableSystemProxy(){ return SetSystemProxy(NULL, NULL, 0);}

#endif
