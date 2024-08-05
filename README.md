# Birthday Tower

The Birthday Tower is an interactive toy with light and sound,
intended for young children.

It is a 2.2 meter tall wooden tower with 3D printed top and base.
It has a strip of LEDs running its length, and it has one big red
illuminated arcade button in a separate controller box.

It's called Birthday Tower because it will be a gift for two 
family members who're about to celebrate their
fifth and third birthdays. (Summer 2024)

The electronics are:
 * Teensy 4.0
 * Teensy Audio Adapter
 * Adafruit NeoPXL8 Friend
 * 2 meters of 100/meter WS2812 RGB LEDs
 * a 5 volt, 10 amp power brick
 * a bunch of wiring and connectors


# Repository Organization

### Audio_Graph

  script to process Teensy Audio Library module graphs

### BT_Teensy

  the birthday tower firmware running on the Teensy

### Interactive_Diversion_1

  the original 2013 project that inspired this.
  It ran on an Arduino Uno.

### Ping_Pong

  an audio test sketch that demonstrates a ping-pong
  delay

### Reverb_Sketch

  an unfinished sketch that uses Mutable Instruments modules

### Reverb_Test

  a test sketch that implements the Mutable Instruments
  modules and the parameter real-time tuning system.  I used
  it to tune the sounds of the bass drum, FM drum, and reverb
  modules.

### Tweaker

  the GUI tool that edits audio parameters in real time
  for Reverb_Test and (soon) BT_Teensy