# Space Invaders on the 8080

Emulator is a work in progress!! <br>
A space invaders emulator, like the one from arcade machines back in 1970s. The Space Invaders ran on an Intel 8080. This is supposed to emulate that entire machine, rather than the 8080 alone. The buttons and coin inserts are mapped to the keyboard. <br>

GUI using [allegro](https://liballeg.org/)<br>

# Inputs - 
I have mapped the inputs to be -

```
COIN INSERT - Left Key Shift

Player 1 -
    LEFT - A
    RIGHT - D
    FIRE - Space
    START - Num 1
    
Player 2 -
    LEFT - Arrow Left
    RIGHT - Arrow  Right
    FIRE - Arrow Up
    START - Num 2
```

The CPU is fully functional. <br>
The rest of the arcade machine is also finished.

<br>
[CPUDIAG.asm](http://www.emulator101.com/files/cpudiag.asm). <br>
[8080PRE.COM](https://altairclone.com/downloads/cpu_tests/). <br>
[CPUTEST.COM](https://altairclone.com/downloads/cpu_tests/). <br>
[8080EXM.COM](https://altairclone.com/downloads/cpu_tests/), the final boss. <br>

![Game](cemu/emulation/res/invaders_crop.gif)

### TODO
* Remove additional debug functions which I no longer use in it.
* Check if this runs in linux directly or needs a change
