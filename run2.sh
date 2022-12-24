gcc server.c -o y
./y
gcc sniffer.c -o p
timeout 17s ./p
cd NodeServer
node /home/elior/NodeServer/index.js