#ifndef WINHTTPPROXY_H
#define WINHTTPPROXY_H

int SetSystemProxy(char* proxy, char* bypass, int enable);
inline int DisableSystemProxy(){ return SetSystemProxy("", "", 0);}

#endif
