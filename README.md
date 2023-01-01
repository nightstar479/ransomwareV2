# ransomwareV2

Developement repository for C ransomware - Developement cursus

# Goal

this project is part of the Systems Security Development course

The purpose is strictly educational and its use is reserved for demonstration purposes



# How to make it work
Start by editing the `server_ip` variable (line 314 in `client.c` and line 19 in `server.c`) to match your situation, you can also change the `server_port` variable for some reason the default port (9999) doesn't suit your needs.

## Compiling

### Scripted method
```bash
Scripts/compile.sh client && Scripts/compile.sh server
```

### Manual method

```bash
gcc -w client.c -o client -lm -lcrypto
gcc -w server.c -o server -lm -lcrypto
```

## Execution

### Server side
Needs to be executed before the client, the key exchange happens before encryption and won't complete if TCP connection is not fullfilled.

```bash
./server
```

### Client side

```bash
./client [Root Folder] [-e/-d]
```
If you use `-e` it will encrypt and send the key to the remote server
If you use `-d` it will decrypt the folder, you need hower to have `key.bin` and `iv.bin` in the **same folder** as the executable.
