# ddnsd  
Author: Tim Zimmermann <tim.zimmermann@server24-7.eu>  
  
## Description:   
Ddnsd is a background service to dynamically update  
your IP-Adress in a DNS Zone file. It is compatible  
with most linux / unix based operating systems     

## NOTICE  
You will probably need a secondary nameserver with  
an static IP-Address to which your nameserver will  
send the updated zone, or another dynamic domain  
which is always up to date.

## Requirements:    
- Linux / Unix based operating system  
- Systemd
- g++  
- make  
- curl
- dns server software like bind9 installed on localhost  
  and allowing requests from localhost

## Installation:  
- Please run the installer as root
- make  
- make install    
OR
- Follow the instructions at https://deb.server24-7.eu/add-repo.txt to use the APT Repository
- execute apt-get install ddnsd
  
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
