# nibblesd

Nibbles Daemon. Game server for nibbles networked clients.
This is a C socket programming exercise. It is not intended to use this for any purpose.

## Installation

Build and install
```
$ make
$ sudo make install
```

The command above will place the binary in _/usr/bin_. \
Alternatively you can also install it as a user in another path (i.e: _$HOME/.local/bin_)
```
$ make install DESTDIR=$HOME/.local/bin
```