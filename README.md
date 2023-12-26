# Online-Voting-System

## Build

### Compiler
Compile `server` and `client`:

```
$ sudo chmod +x auto-build.sh
$ . auto-build.sh
$ make test
```

or you can build step by step

```
$ mkdir build && cd build/
$ cmake ..
$ make
```

### Test
Use shell script to test `server` and `client`:
```
# in Online-Voting-System/build

$ make test
```

Execute the `server` with **port**:

```
# in Online-Voting-System/build

$ ./server/server 8888
```

Execute the `client` with **server IP address** and **port**:
```
# in Online-Voting-System/build

$ ./client/client 127.0.0.1 8888
```

### Format
Format the code in the same coding style:
```
# in Online-Voting-System/build

$ make format
```
