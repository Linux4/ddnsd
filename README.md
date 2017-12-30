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
- dns server software like bind9

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
Note: On first run you have to set the DNS Zone Serial
using the command ddnsd-serial. Help for ddnsd-serial:
"ddnsd-serial -h".   
