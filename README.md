# ST-LINK-GDB-Server

Connect to your Stlink Debugger from anywhere

  

## Proje Hakkında.

  

Bu proje ST-LINK debugger cihazınıza TCP/IP üzerinden ulaşarak debug yapmanıza yarar. ST-LINK Server uygulaması temelinde GNU Debugger Bridge (GDB) server ile terminal üzerinden haberleşerek terminal çıktılarını client uygulamasına gönderir. Client uygulaması GDB server terminalini ekranda görmenize, yeniden aktifleştirmeye veya tamamen kapatmanıza olanak tanır.

  

### Topoloji

![alt text](https://github.com/mcansiz/stlink-GDB-Server/blob/main/topoloji.jpg?raw=true)

Proje 2 kısımdan oluşur:

### 1. ST-LINK Server Uygulaması

Bu uygulama PC üzerinde lokal IP üzerinden belirtilen TCP portu açarak ST-LINK cihazınıza bağlanarak GDB server uygulamasını etkinleştirir ve bağlantıyı bekler. Server uygulaması açıldığı anda 4545 (default) portundan local host Ip adresinde dinleme yapar.  Portu değiştirmek için dinlemeyiş kapatın portu değiştirin dinlemeyi açın.

GDB Server Client tarafından komut ile açılmaktadır.

![alt text](https://github.com/mcansiz/stlink-GDB-Server/blob/main/server.jpg?raw=)

  

### 2. Client Uygulaması

Bu uygulama ST-LINK Server uygulamasına bağlanarak GDB server terminalini ekranda görmenize, yeniden aktifleştirmeye veya tamamen kapatmanıza yarar.

GDB client uygulaması açıldığında Serve ip adresi ve port numarası girilmelidir. GSB server port numarasını değiştirebilirsiniz.

Start GDB Server butonu ile server başlatılır.
Stm32cubeide uygulamanın debug ayarlarında  sırası ile ;
1 . Debugger -> Connect to remote GDB Server 
2. Host name or IP address bölümüne Serve IP adresi yazılır. 
3. Port number bölümüne GDB Server Port yazılır.
4. Debug yapılarak uygulamaya bağlanılır.

![alt text](https://github.com/mcansiz/stlink-GDB-Server/blob/main/client.jpg?raw=true)

###  3. Internet IP adresi ile bağlanmak
 Server PC ip adresi statiğe çevrilmeli, DDNS alınmalı ve server için port yönlendirilmesi yapılmalı.
Alternatif olarak  server ve client bilgisayarlarında RADMIN VPN kullanarak bağlanabilirsiniz.