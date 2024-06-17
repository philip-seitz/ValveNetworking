# Valve Networking App

Using the [Game Networking Sockets API](https://github.com/ValveSoftware/GameNetworkingSockets) by Valve, this applications acts as a simple example of implementing player movement 
where an actual server (app) controls the players position. 

For that purpose a headless version of this app would run on a server (Linux machine for example). A simple demonstration can be seen below:



https://github.com/philip-seitz/ValveNetworking/assets/93658810/c200e431-e526-4a49-a97c-84b927908d30

Essentially, a headless instance of the server app is running on a Linux machine (server, see top right; the server info can be seen below). 
A client app (left side of the screen) is running on a local Windows machine and connecting to that server. After the connection process the client can control the object,
which is visualized by some simple OpenGL code.

