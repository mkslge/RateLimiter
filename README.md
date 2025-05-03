# Scalable UDP Rate Limiter

Limits amount of UDP messages that can be sent to a server via a client, default settings allow clients to send a message up to once every half a second.

The rate limiter works by using a circular buffer to store the most recent clients that interacted with the server and tracking the last time they sent a message
to the server

## Requirements

- **gcc**


## Installation
  First clone the repo, 

  To run the client side code run the command where <server ip_address> is the ip address that you want to send data too
   ```
   ./client <server ip_address>
  ```
  
  To run the server side code run the command 
  ```
  ./server
  ```


