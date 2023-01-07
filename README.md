# g733systray
Qt system tray applet for interfacing with g733daemon for a Logitech G733 headset.

It's meant to interface with [g733daemon](https://github.com/danieloneill/g733daemon) and won't do much without it.

To build it:
```
$ mkdir build
$ cd build
$ qmake ..
$ make
```

If g733daemon isn't running (or the headset is off) you'll just see a headset icon in your tray with 0% charge.

The lighting may be toggled by right-clicking the tray icon and selecting the appropriate option. The other one is "Exit", so be steadfast with your attention.

The side button on the headset will trigger an audio prompt to indicate a roughly-accurate state of charge.

