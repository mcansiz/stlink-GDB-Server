# ST-LINK-GDB-Server
Connect to your Stlink Debugger from anywhere

## Proje Hakkında.

Bu proje ST-LINK debugger cihazınıza TCP/IP üzerinden ulaşarak debug yapmanıza yarar.   ST-LINK  Server uygulaması temelinde GNU Debugger Bridge (GDB) server ile terminal üzerinden haberleşerek terminal çıktılarını client uygulamasına gönderir. Client uygulaması GDB server terminalini ekranda görmenize, yeniden aktifleştirmeye veya tamamen kapatmanıza olanak tanır.

### Topoloji


![alt text](https://github.com/mcansiz/stlink-GDB-Server/blob/main/topoloji.jpg?raw=true)
 
Proje 2 kısımdan oluşur:
### 1.  ST-LINK Server Uygulaması
Bu uygulama PC üzerinde lokal IP üzerinden belirtilen TCP portu açarak ST-LINK cihazınıza bağlanarak GDB server uygulamasını etkinleştirir ve bağlantıyı bekler.

### 2.  Client Uygulaması
Bu uygulama ST-LINK Server uygulamasına bağlanarak GDB server terminalini ekranda görmenize, yeniden aktifleştirmeye veya tamamen kapatmanıza yarar.
