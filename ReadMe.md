# ProxyPlus

## Introduction

The goal of this application is to meet some of the needs for a mobile work force.

When we are on select networks, use a proxy. When we are off those networks, disable the proxy.

This fills the missing gap between [insecure WPAD implemenations](https://www.nopsec.com/responder-beyond-wpad/) and static proxy configurations.

## Features

Enabling WinHttp and WinInet proxy depending:

- Domain connectivity

## Configuration

The application is configured through a single .ini file called `config.ini`.

To leave an option disabled, exclude it from the config.

This file follows the format below:

``` 
[general]
log-level   = "DEBUG"                               ; (String) supported levels are DEBUG and WARN
watch       = "NLA"                                 ; (String) currently only supporting NLA indications
quiet       = TRUE                                  ; (Bool) setting to TRUE will only write the logging to a file. Leaving blank writes to console


[winhttp]
proxy       = "proxy.contoso.com:9000"              ; (String) static proxy
bypass-list = "<local>;"                            ; (String) exclusions for the proxy

[wininet]
wpad            = "TRUE"                            ; (Bool)                        
auto-config-url = "https://proxy.contoso.com:9001"  ; (String) 
proxy           = "https://proxy.contoso.com:9002"  ; (String)
bypass-list     = "<local>;"                        ; (String)
```

## How it works

It uses the Windows API's to manage the proxy settings for the current users.

Please note, since this runs in the context of the current users if you have proxies configured per user this will not address all of them.

## Todo

- [X] Add a config file parser [(Or found is a better word)](https://github.com/rxi/ini)
- [X] Get WinHttp proxy working
- [X] Get WinInet proxy working
- [X] Get proxy state change with NLA working
- [X] Add logging [(again very nice rxi)](https://github.com/rxi/log.c)
    
## Change Log

- 08/13/2020 Testing Versions
- 10/28/2020 v1
