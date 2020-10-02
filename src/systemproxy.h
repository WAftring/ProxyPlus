#ifndef WINHTTPPROXY_H
#define WINHTTPPROXY_H

int SetSystemProxy(const char* proxy, const char* bypass, int enable);
inline int DisableSystemProxy(){ return SetSystemProxy("", "", 0);}

#endif
