# _Rhodes Ramble: Yik-Yak Clone with Network Capabilities._

## introduction

Initial project files provided by Marion Lang (@ml8).

In this project, I wrote a clone of the "popular" [Yik Yak
app](https://yikyak.com/) using sockets and threads, _Rhodes Ramble_.

This involved implementing the specification of the protocol that Rhodes
Ramble clients and servers use to communicate.

This project was broken up into three parts:

* Writing the Rhodes Ramble service protocol implementation.
* Writing the Rhodes Ramble client protocol implementation.
* Modifying the Rhodes Ramble server to be multithreaded.

The objectives of this project were:

* to gain experience with client/server programming using a protocol;
* to practice data parsing and manipulation;
* to use threads to provide concurrency, along with basic synchronization;
* to gain experience debugging programs with several processes and layers;
* to gain experience reading, writing, and understanding code---C in particular;
* and to gain experience reading and understanding technical documentation.

## yik yak and rhodes ramble

### description of yik yak

[Yik Yak](https://en.wikipedia.org/wiki/Yik_Yak) is a semi-anonymous social
media/discussion platform. The initial version of the application was quite
simple; users could do one of three actions:

* Users can create short text posts (called Yaks).
* Users can add short text comments to Yaks.
* Comments and Yaks have an associated karma value that is changed by a user up-
  or down-voting the comment or yak.

The app is semi-anonymous in the sense that users have accounts and persistent
identities with the service, but the user account information is never displayed
to other users.

Instead, each user is assigned a random emoji. This provides a consistent
identity across posts and comments. Users can choose to change their emoji if
they would like to create a new identity.

Users have a karma value that is the sum of the karma of all of their posts and
comments.

One of the reasons that the app is popular is that the app experience is tied to
geolocation and therefore allows for location-based communities to be formed
(e.g., at colleges). Users are only able to see and interact with content that
was posted within 5 miles of their location---though the app allows this
location to be cached so that someone traveling has access to their local Yik
Yak "community."

### rhodes ramble

__Rhodes Ramble__ is a clone of Yik Yak. It is a
client-server system, where user clients post Rambles (Yaks) and comments on
Rambles to a central server.

Rhodes Ramble contains the core features of Yik Yak (semi-anonymous with a
persistent identity, posts, comments, up/down votes, and user karma) but does not
implement location-based services.

## the RR protocol

The Rhodes Ramble app supports a handful of operations:

* __Login:__ This allows a user to register or login. Every time a user logs in,
  their icon is changed and they are granted a new _token_.
* __Post:__ A user creates a ramble.
* __Comment:__ A user creates a comment.
* __Vote:__ A user up/down votes a comment or ramble.
* __Get Posts:__ Retrieve a list of all rambles.
* __Get Comments:__ Retrieve a list of all comments for a particular ramble.
* __Get User Info:__ Get information about a user.

### code organization and system architecture

The Rhodes Ramble app has two processes: the _client_ and the _server_. In the
following architecture diagram, the client components are represented with blue
boxes and the server components are represented with red.

Solid arrows denote interactions via function calls, while dotted arrow
represent interactions over sockets with the Rhodes Ramble protocol.

![Rhodes Ramble Architecture Diagram](doc/rhodes_ramble_arch.png)

The client process was divided into two components: the frontend UI
(`rr_console_client.c` or `rr_curses_client.c`) and the _client library_
(`rr_client_lib.{h,c}`).

The UI component has two responsibilities:

* Establish the connection with the server process
* Delegate all communication with the server to the client library

The client library is responsible for implementing the Rhodes Ramble protocol.
It has a set of `do_` and `get_` functions that are used by the UI to perform
tasks (logging in, voting, commenting, posting) or retrieve data from the server
(user info, posts, comments).

The server process is divided into three components: the main server that
listens for incoming client connections (`rr_server.c`), the Rhodes Ramble
protocol service that parses and executes client requests (`rr_service.{h,c}`),
and the application core that actually carries out requests by modifying or
retrieving the data stored on the server (`rr_core.{h,c}`).

The server component (`rr_server.c`) listens for connections on the port
given by `argv[1]`. When a client connects, the server calls `handle` to
handle the client connection by reading requests and providing responses in a
loop until the client disconnects.

This function calls the `parse_and_dispatch` function of the protocol service
(`rr_service.{h,c}`). The service examines the request that the client made and
determines which action to execute on the application core. It then calls the
appropriate function in the core (defined in `rr_core.h`).

When the application core has performed the action requested, the service writes
the response to the client connection.

### data model

The internal data model of Rhodes Ramble has two entities:

* __User:__ A user of the service. Users have a name and a pin (numeric
  password), a current emoji, a karma value, and a current token. The name and
  pin are fixed for the lifetime of the user. When the user logs in for the
  first time, their name and initial pin are immutable and fixed. The user's
  emoji and token is changed every time the user logs in. The user's karma is
  the sum of all of their rambles (over all time).

* __Post/Comment:__ A ramble/post and a comment are the same entity, internally. Posts
  have an id, an emoji, text, and a parent post. Once a post is created, it is
  immutable. Rambles/posts have a NULL parent, while comments have a parent post
  that they belong to. The parent is a post id.

### protocol definition

When a Rhodes Ramble client connects to the Rhodes Ramble server, the server
waits for and responds to client requests.

A client request consist of a single line of text terminated with a newline
character.

The server responds to a client request with one or more lines of text,
depending on the request type.

## example interaction

```bash
$ ./rr_console_client lily.rhodes.edu 4444
trying to read token from /home/mohidtanveer/.rr_token...
Got cached token 1102520059
Terminal UI for Rhodes Ramble
commands:
                l <name> <pin> register user with given name and pin (this is your password/id)
                g              get all messages
                u <id>         upvote message/comment with given id
                d <id>         downvote message/comment with given id
                p <text>       post message with given text
                c <id> <text>  comment on message with given id
                m <id>         get message comments for message with given id
                i              get your user info
                q              quit
                ?              print this message
> i
icon/karma: ☕/0
> l mohid 1234
got token 1967513926
token cached to /home/mohidtanveer/.rr_token...
> i
icon/karma: ☕/0
> g
 1681692777 > (1) 🍛 hey guys!
 256487231 > (-12) 🐪 whos throwing this weekend???
 424238335 > (48) 📙 rhodes wont ever learn from all these parking issues
 837126389 > (-31) 🚆 weed??
 596516649 > (23) 📹 it's such a beautiful day!
> u 596516649
> g
 1681692777 > (1) 🍛 hey guys!
 256487231 > (-12) 🐪 whos throwing this weekend???
 424238335 > (48) 📙 rhodes wont ever learn from all these parking issues
 837126389 > (-31) 🚆 weed??
 596516649 > (24) 📹 it's such a beautiful day!
> c 596516649 right!?
> m 596516649
 1540383426 > (0) ☕ right!?
> g
 1681692777 > (1) 🍛 hey guys!
 256487231 > (-12) 🐪 whos throwing this weekend???
 424238335 > (48) 📙 rhodes wont ever learn from all these parking issues
 837126389 > (-31) 🚆 weed??
 596516649 > (24) 📹 it's such a beautiful day!
 1540383426 > (0) ☕ right!?
> q
```

---

## overview

```text
3rhodesramble-yikyakclone
|-- README.md  ..................... this document
|-- doc
    `-- rhodes_ramble_arch.png  .... system architecture diagram
`-- src
    |-- conn.h  .................... utility functions for opening/using sockets
    |-- conn.c
    |-- emojis.h  .................. emoji library
    |-- emojis.c
    |-- lib
    |   |-- map.h  ................. the map structure
    |   |-- map.c  ................. the map code
    |   |-- fnv64.h  ............... the hash file
    |   `-- map.o  ................. compiled map code
    |-- Makefile  .................. project Makefile
    |-- posts.h  ................... post/comment data model
    |-- posts.c
    |-- rr_client_lib.h  ........... client implementation of Rhodes Ramble protocol
    |-- rr_client_lib.c  ........... MAIN SOURCE FILE
    |-- rr_console_client.c  ....... console frontend
    |-- rr_core.h  ................. core business logic for the service
    |-- rr_core.c
    |-- rr_curses_client.c  ........ curses frontend
    |-- rr_server.c  ............... server main function / MAIN SOURCE FILE
    |-- rr_service.h  .............. service implementation of Rhodes Ramble protocol
    |-- rr_service.c  .............. MAIN SOURCE FILE
    |-- tests  
    |   |-- posts_test.c  .......... unit tests for posts
    |   |-- rr_core_test.c  ........ unit tests for service
    |   |-- test_utils.h  .......... unit testing utilities
    |   |-- test_utils.c 
    |   `-- users_test.c  .......... unit tests for users
    |-- uio.h  ..................... io utils for sockets
    |-- uio.c 
    |-- users.h  ................... user data model
    |-- users.c
    |-- util.h  .................... utility functions used in the project
    `-- util.c
```
