# FiboBG

FiboBG is a very simple program which displays the [Fibonacci clock](https://www.theguardian.com/science/alexs-adventures-in-numberland/2015/may/09/fibonacci-clock-can-you-tell-the-time-on-the-worlds-most-stylish-nerd-timepiece). It creates a dynamic background for simple window manager like i3 or dwm. FiboBG was only tested on dwm. The program works well with compositors like compton. 

## Installation

```{bash}
git clone https://github.com/hollorol/fiboBG.git 
cd fiboBG
make
make install
```

## Usage
In xinitrc, run the compiled program in background before window manager! If you have a compositor (only compton tested) you can make a dynamical background more usable with setting your terminal alpha parameter. 

The program was tested in a system which uses compton, dwm (with ewmhtags, fakefullscreen, and pertag patches), st (with alpha patch). 

## Implementation

The program uses only Xlib and Cairo library beside of the standards. It draws the Fibonacci clock with Cairo into a Pixmap, and directly onto the root window (in case of missing compositor). After writing the root window _\_XROOTPMAP\_ID_ property (char* casted Pixmap*) the compositor automatically draws this Pixmap onto the root window (it will be the background). 

Thanks richardgv's comment on this [issue](https://github.com/chjj/compton/issues/225), it helped a lot. Without this comment I would not have known how to draw background when using compositor.

Figuring out all parameters and usage of XChangeProperty was really hard. Here reading the source code of feh helped a lot. 


