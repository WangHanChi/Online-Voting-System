# Online-Voting-System

## Build

Compile `server` and `client`:

```
$ make all
```

Use shell script to test `server` and `client`:
```
$ make test
```

Execute the `server` with **port**:
```
$ ./build/server 8888
```

Execute the `client` with **server IP address** and **port**:
```
$ ./build/client 127.0.0.1 8888
```

Format the code in the same coding style:
```
$ make format
```
