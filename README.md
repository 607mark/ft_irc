# ft_irc

An IRC (Internet Relay Chat) server implementation in C++20, built as part of the 42 school curriculum.
## Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [Architecture](#architecture)
- [Technology Stack](#technology-stack)
- [Dependencies](#dependencies)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
- [Supported Commands](#supported-commands)
- [Configuration](#configuration)
- [Testing](#testing)
- [Limitations](#limitations)

## Project Overview

`ft_irc` is a lightweight IRC server that implements the core functionality of the IRC protocol. It allows multiple clients to connect simultaneously, authenticate with a password, join channels, and exchange messages. The server uses non-blocking I/O with `poll()` for handling multiple connections efficiently.

## Features

- **Multi-client support**: Handle multiple simultaneous client connections
- **Password authentication**: Server requires a password for client connections
- **Channel management**: Create, join, and leave channels
- **Channel modes**: Support for invite-only, topic restrictions, key-protected channels, and user limits
- **Operator privileges**: Channel operators can manage channel settings and users
- **Private messaging**: Direct messages between users
- **Invite system**: Invite users to channels (required for invite-only channels)
- **Logging**: Configurable logging with multiple severity levels and categories

## Architecture

The project is organized into the following main components:

```
src/
├── main.cpp              # Entry point
├── handleInput.cpp       # Command parsing and routing
├── irc.hpp               # Common includes and declarations
├── server/
│   ├── Server.cpp        # Main server logic (poll loop, client management)
│   └── Server.hpp
├── client/
│   ├── Client.cpp        # Client state and message queue
│   └── Client.hpp
├── channel/
│   ├── Channel.cpp       # Channel state and user management
│   └── Channel.hpp
├── commands/
│   ├── commands.hpp      # Command handler declarations
│   ├── invite.cpp        # INVITE command
│   ├── join.cpp          # JOIN command
│   ├── kick.cpp          # KICK command
│   ├── mode.cpp          # MODE command
│   ├── nick.cpp          # NICK command
│   ├── part.cpp          # PART command
│   ├── pass.cpp          # PASS command
│   ├── ping.cpp          # PING/PONG command
│   ├── privmsg.cpp       # PRIVMSG command
│   ├── topic.cpp         # TOPIC command
│   ├── user.cpp          # USER command
│   ├── who.cpp           # WHO command
│   └── whois.cpp         # WHOIS command
├── logger/
│   ├── Logger.cpp        # Logging utility
│   └── Logger.hpp
├── parser/
│   └── parser.cpp        # Input parsing utilities
└── utils/
    ├── responsePrefix.cpp
    ├── utils.cpp
    ├── validateChannelName.cpp
    └── validateClientName.cpp
```

### Main Components

#### Server
The `Server` class manages:
- Socket creation and binding
- Client connections via `poll()`
- Client data handling and message routing
- Client disconnection and cleanup

#### Client
The `Client` class stores:
- Connection file descriptor
- Nickname, username, real name, and hostname
- Registration state (password, nick, user)
- Message buffer for incomplete messages
- Outgoing message queue

#### Channel
The `Channel` class manages:
- Channel name and topic
- User list and operator list
- Invite list
- Channel modes (invite-only, topic restriction, key, user limit)

#### Logger
The `Logger` class provides:
- Multiple log levels (DEBUG, INFO, SUCCESS, WARNING, ERROR)
- Log categories (SERVER, CLIENT, CHANNEL, NETWORK, AUTH, COMMAND)
- Colored console output
- Timestamps

## Technology Stack

The IRC server is built on **BSD sockets** (POSIX sockets), which is the standard Unix/Linux networking API for TCP/IP communication.

### Key Networking Components Used

| Function | Purpose |
|----------|---------|
| `socket()` | Creates the server socket (`AF_INET`, `SOCK_STREAM`) |
| `bind()` | Binds the socket to a port |
| `listen()` | Marks the socket as passive for accepting connections |
| `accept()` | Accepts incoming client connections |
| `poll()` | Monitors multiple file descriptors for I/O events |
| `send()` / `recv()` | Sends and receives data over the socket |

### Protocol Stack

```
┌─────────────────────────────────────┐
│          IRC Protocol               │  ← Application Layer (this project)
├─────────────────────────────────────┤
│          TCP (SOCK_STREAM)          │  ← Transport Layer (reliable delivery)
├─────────────────────────────────────┤
│          IPv4 (AF_INET)             │  ← Network Layer (IP addressing)
├─────────────────────────────────────┤
│          BSD Sockets API            │  ← Interface to the kernel
├─────────────────────────────────────┤
│          Linux Kernel               │  ← Handles actual network I/O
└─────────────────────────────────────┘
```

### Technology Summary

| Component | Technology |
|-----------|------------|
| **Language** | C++20 |
| **Networking API** | BSD/POSIX Sockets |
| **Transport Protocol** | TCP |
| **Network Protocol** | IPv4 |
| **I/O Multiplexing** | `poll()` |
| **Application Protocol** | IRC (RFC 1459 / RFC 2812) |

## Dependencies

- **C++17 compiler** (g++ or clang++)
- **Make**
- **POSIX-compliant system** (Linux, macOS)
- **Google Test** (optional, for running tests)

## Build Instructions

### Build the Server

```bash
make
```

This will compile the server and create the `ircserv` executable.

### Other Make Targets

```bash
make clean      # Remove object files
make fclean     # Remove object files and executable
make re         # Rebuild everything
make test       # Build and run tests (requires Google Test)
```

## Usage

### Starting the Server

```bash
./ircserv <port> <password>
```

- `<port>`: Port number to listen on (1-65535)
- `<password>`: Password required for client authentication

**Example:**
```bash
./ircserv 6667 mypassword
```

### Connecting with an IRC Client

You can connect using any IRC client (e.g., irssi, WeeChat, HexChat):

```bash
# Using irssi
irssi -c localhost -p 6667 -w mypassword
```

### Connecting with Netcat

For testing purposes, you can use `nc`:

```bash
nc localhost 6667
```

Then send IRC commands manually:
```
PASS mypassword
NICK mynick
USER myuser 0 * :My Real Name
JOIN #channel
PRIVMSG #channel :Hello, world!
```

## Supported Commands

| Command | Syntax | Description |
|---------|--------|-------------|
| `PASS` | `PASS <password>` | Authenticate with the server password |
| `NICK` | `NICK <nickname>` | Set or change nickname |
| `USER` | `USER <username> <mode> <unused> :<realname>` | Set username and real name |
| `JOIN` | `JOIN <channel> [key]` | Join a channel |
| `PART` | `PART <channel> [message]` | Leave a channel |
| `PRIVMSG` | `PRIVMSG <target> :<message>` | Send a message to a user or channel |
| `KICK` | `KICK <channel> <user> [reason]` | Remove a user from a channel (operators only) |
| `INVITE` | `INVITE <nick> <channel>` | Invite a user to a channel |
| `TOPIC` | `TOPIC <channel> [new_topic]` | View or set channel topic |
| `MODE` | `MODE <channel> <modes> [params]` | Set channel modes |
| `PING` | `PING <token>` | Keep connection alive |
| `WHO` | `WHO <channel>` | List users in a channel |
| `WHOIS` | `WHOIS <nick>` | Get information about a user |

### Channel Modes

| Mode | Parameter | Description |
|------|-----------|-------------|
| `+i` | None | Invite-only channel |
| `+t` | None | Topic restricted to operators |
| `+k` | `<key>` | Channel key (password) |
| `+o` | `<nick>` | Give/take operator privileges |
| `+l` | `<limit>` | Set user limit |

**Example:**
```
MODE #channel +i           # Set channel to invite-only
MODE #channel +k secret    # Set channel key to "secret"
MODE #channel +o user      # Give operator status to user
MODE #channel +l 10        # Limit channel to 10 users
```

## Configuration

The server does not use a configuration file. All settings are provided via command-line arguments:

- **Port**: First argument (required)
- **Password**: Second argument (required)

## Testing

The project includes unit tests using Google Test. To run the tests:

```bash
make test
```

### Test Coverage

- `channelname_test.cc`: Channel name validation tests
- `command_test.cc`: Command parsing tests
- `logger_test.cc`: Logger functionality tests
- `password_test.cc`: Password validation tests
- `port_test.cc`: Port validation tests
- `username_test.cc`: Username validation tests

## Limitations

### Incomplete Features

1. **No TLS/SSL support**: All connections are unencrypted
2. **No server-to-server linking**: Only standalone server operation
3. **No persistent storage**: All data is lost on server restart
