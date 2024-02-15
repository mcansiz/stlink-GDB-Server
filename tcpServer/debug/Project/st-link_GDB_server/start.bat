

@echo off
cmd /K "ST-LINK_gdbserver.exe -p 61234 -l 1 -d -s -cp .\st-linkCli\bin -m 0 -k"
