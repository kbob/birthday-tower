# Audio Graph

I used the Teensy Audio System Design Tool to create
the audio graph used in BT_Teensy.

https://www.pjrc.com/teensy/gui/index.html

The design tool only lets you work with built-in modules,
and BT_Teensy uses several custom modules.  So I used
built-in modules as stand-ins, and wrote a Python script
to rewrite the generated C++ source to use the custom modules.

I also got all OCD and positioned the modules very carefully.
I thought about writing a script for that, but ended up doing it manually.

## Use

Export the graph from the design tool and save it in
audiograph.cpp.  Then run modgraph.py to convert it.

*N.B.*, I edited audiograph.cpp for cosmetics.  I reordered
module definitions and added blank lines for grouping.

```
$ python modgraph.py audiograph.cpp
````

Then copy and paste the output into the Arduino sketch.
(Skip the `#include` lines; only `<Audio.h>` is needed.)
