# ProxyPlus

## Introduction

The goal of this application is to meet some of the needs for a mobile work force.

When we are on select networks, use a proxy. When we are off those networks, disable the proxy.

This fills the missing gap between [insecure WPAD implemenations](https://www.nopsec.com/responder-beyond-wpad/) and static proxy configurations.

## Features

Enabling WinHttp and WinInet proxy depending:

- Network Connection Profile
- (TODO) Adapter being used
- (TODO) Network name
- (TODO) Connectivity to resource
- (TODO) Time of day


## How it works

It uses the Windows API's to manage the proxy settings for the current users.

Please note, since this runs in the context of the current users if you have proxies configured per user this will not address all of them.

(This is planned for a future implementation)

## Todo

- [X] Add a config file parser [(Or found is a better word)](https://github.com/rxi/ini)
- [X] Get WinHttp proxy working
- [X] Get WinInet proxy working
- [X] Get proxy state change with NLA working
- [X] Add logging [(again very nice rxi)](https://github.com/rxi/log.c)
- [ ] Add can reach X state check
- [ ] Add time of day state change watch
- [ ] Add option to run as a service
    
## Change Log

- 08/13 Testing Versions
