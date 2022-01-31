# TCP/UDP Client-Server C Application

The purpose of this project is to design a simple Application Layer Protocol (ALP) and implement it by Berkeley sockets programming. ALP should be operational on both TCP and UDP transport layers. Properties of ALP that you will design and implement are as follows.

- ALP is a client-server protocol. There are multiple clients and but a single server.
- ALP is specifically to be called Orhan Veli Poems Servis Protokolu (OVPSP) and the server is Orhan Veli Poems Server (OVPS).
- OVPS keeps a number of Orhan Veli Poems (OVP) in an ordinary LINUX file, from which a OVP could be retrieved by its OVP number. Retrieval is either by OVS number or random. Client may inquire about the largest number in OVPS.
- Any client can anonymously retrieve a OVP. If the supplied number is in the range or request is for a random OVP, OVPS will supply the OVP and it will be displayed in the client screen. Assume all OVP are short enough to fit into a single screen, so no scrolling of screen is necessary. 
