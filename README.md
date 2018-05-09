# ddnsd  
Copyright (c) Tim Zimmermann <tizimmermann01@gmail.com>  
  
## Description:   
Ddnsd is a background service to dynamically update  
your IP-Adress in a DNS Zone file. It is compatible  
with most linux / unix based operating systems     
  
## Requirements:    
- Linux / Unix based operating system  
- Systemd
- g++  
- make  
- curl
- dns server software like bind9 installed on localhost  
  and allowing requests from localhost
- dnsutils

## Installation:  
- Please run the installer as root
- make  
- make install    
  
## Update  
- Please run the updater as root
- make  
- make update  
    
## Uninstall:  
- Please run the uninstaller as root
- make uninstall  
  
## Usage:  
Install the service as shown above and then edit  
/etc/ddns/ddnsd.conf to configure the service.  
