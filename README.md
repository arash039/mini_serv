# Exam rank 06: Mini Server in C

This repository contains a simple server written in C. The server uses the `select()` system call to handle multiple clients concurrently. It's a basic implementation of a chat server where clients can send messages to each other.

## Code Explanation

The server is implemented in a single C file, `mini_serv.c`. Here's a brief overview of how it works:

- The server starts by checking the command-line arguments to get the port number on which it should listen.

- It then initializes a buffer for incoming messages, an array of client structures, and a server socket.

- The server socket is bound to the specified port on the loopback address (127.0.0.1).

- The server then enters a main loop where it uses the `select()` system call to wait for activity on any of the sockets it's monitoring (the server socket and any client sockets).

- If the server socket is ready for reading, it means a new client is connecting. The server accepts the new client, adds it to the set of monitored sockets, and sends a message to all other clients announcing the new client's arrival.

- If a client socket is ready for reading, it means the client has sent a message. The server reads the message and forwards it to all other clients.

- If a client socket is ready for reading but no data is available, it means the client has disconnected. The server removes the client from the set of monitored sockets and sends a message to all other clients announcing the client's departure.

## Usage

To run the server, compile the `mini_serv.c` file and run the resulting binary with a single argument, the port number on which the server should listen.

```bash
gcc -o mini_serv mini_serv.c
./mini_serv 8080
```

To connect to the server, you can use any TCP client, such as netcat (`nc`). Here's an example of how to connect to the server running on `localhost` port `8080`:

```bash
nc localhost 8080
```
And for testing a message with multiple `\n` characters:
```bash
echo -e "hello\nthis is a test message\nthat has multiple new line" | nc localhost 8080
```

Once connected, you can type messages into the client's standard input. These messages will be sent to the server, which will then forward them to all other connected clients.
