#ifndef WINHTTPPROXY_H
#define WINHTTPPROXY_H

int SetSystemProxy(const char* proxy, const char* bypass, int enable);
inline int DisableSystemProxy(){ return SetSystemProxy("NULL", "", 0);}

#endif
