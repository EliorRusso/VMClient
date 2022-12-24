const net = require('net');
const fs = require('fs');
 
const client = net.createConnection({
  port: 8000,
  host: '10.100.102.30'

}, () => {
  // 'connect' listener
  console.log('connected to server!');
  fs.readFile('/home/elior/NodeServer/log.txt', (err, data) => {
    if (err) throw err;
    client.write(data);
  });
});
client.on('data', (data) => {
  console.log(data.toString());
  client.end();
});
client.on('end', () => {
  console.log('disconnected from server');
});