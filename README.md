# IRC Server

A lightweight IRC server written in C++ that accepts connections from IRC clients such as [`irssi`](https://irssi.org/). 
This server supports basic client functionality and includes a simple bot that filters out curse words.

## Features

- Accepts and manages multiple IRC client connections (tested with `irssi`)
- Supports basic IRC protocol commands
- Includes a bot that monitors messages and blocks curse words
- Written in C++

## Getting Started

### Requirements

- C++ compiler (e.g., `g++`)
- Linux environment (tested on Ubuntu)
- `make`

### Build

Use the provided Makefile to compile the project:

```bash
make
```

## Run the Server
- To run the IRC server, execute ./irc_server <port> in your terminal, replacing <port> with the desired port number (e.g., 6667). The server will start and listen for client connections on the specified port.

- Connect with irssi
You can connect using the irssi IRC client with the command irssi -c 127.0.0.1 -p 6667.
