# ddnsd  
Copyright (c) Tim Zimmermann <tizimmermann01@gmail.com>  
  
## Description:   
Ddnsd is a background service to dynamically update  
your IP-Adress in a DNS Zone file. It is compatible  
with most linux / unix based operating systems     
  
## Requirements:    
- Linux / Unix based operating system  
- g++  
- make  
- curl
- dns server software like bind9 installed on localhost  
  and allowing requests from localhost
- dnsutils

## Installation:  
- make  
- make install    
  
## Update  
- make  
- make update  
    
## Uninstall:  
- make uninstall  
  
## Usage:  
Install the service as shown above and then edit  
/etc/ddns/ddnsd.conf to configure the service.  
