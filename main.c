#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include "config.h"
#define VERSION "0.100"

typedef struct _rectangle {
    int pos_x;
    int pos_y;
    int width;
    int height;
    int color;

} rectangle;


typedef struct _myTime {
    unsigned int hour;
    unsigned int minute;
}  myTime;

myTime getTime(void){
    time_t raw_time;
    time(&raw_time);
    struct tm *currTime;
    currTime = localtime(&raw_time);
    myTime t;
    t.hour = currTime->tm_hour;
    t.minute = currTime->tm_min;
    return t;
}

void print_current_time(void){
    myTime t=getTime();
    printf("%02d:%02d\n", t.hour, t.minute);
}

void getFibbTime(myTime t, int* res){
    int minute = t.minute;
    minute = (minute / 5) * 5;
    int hour = t.hour % 12;
    if(hour == 0){
        hour = 12;
    }
    int fibBlocks[5] = {5,3,2,1,1};
    int mins[5] = {0,0,0,0,0};
    int hours[5] = {0,0,0,0,0};
    int blockElement = 0;
    while(minute != 0){
        if(fibBlocks[blockElement] * 5 > minute){
            blockElement++;
            continue;
        }
        mins[blockElement] = 1;
        minute -=  fibBlocks[blockElement] * 5;
        blockElement++;
        
    }
    blockElement = 0;
    while(hour != 0){
        if(fibBlocks[blockElement] > hour){
            blockElement++;
            continue;
        }
        hours[blockElement] = 1;
        hour -=  fibBlocks[blockElement];
        blockElement++;
    }

    for(int i=0; i < 5; ++i){
        *(res + i) = mins[i] + 2 * hours[i];
    }

    if((*(res + 3) == 3) && (*(res + 4) ==0)){
      *(res + 3) = 2;
      *(res + 4) = 1;
    }

}

void drawRectangle(cairo_t* cr, rectangle R){
    color rectColor = colors[R.color]; // colors is defined in config.h
    cairo_rectangle(cr, R.pos_x, R.pos_y, R.width, R.height);     /* set rectangle */
    cairo_set_source_rgb(cr, rectColor.red, rectColor.green, rectColor.blue);   /* set fill color */
    cairo_fill(cr);                            /* fill rectangle */
}

void drawReminder(cairo_t* cr, int minRem){
    float x_pos[4]={10, 810, 810, 10};
    float y_pos[4]={10, 10, 510, 510};
    int previous;

    if(minRem == 0){
        previous = 4;
    } else {
        previous = minRem - 1;
    }

    for(int i = 0; i < 4; ++i){
        cairo_arc(cr, x_pos[i], y_pos[i],10, 0, 2 * M_PI);
        cairo_set_source_rgb(cr, 0.5,  0.5, 0.5);   /* set fill color */
        cairo_fill(cr);
    }

    cairo_arc(cr, x_pos[previous], y_pos[previous], 10, 0, 2 * M_PI);
    cairo_set_source_rgb(cr, 0.5,  0.5, 0.5);   /* set fill color */
    cairo_fill(cr);
    if(minRem != 0){
        cairo_arc(cr, x_pos[minRem - 1], y_pos[minRem - 1], 7, 0, 2 * M_PI);
        cairo_set_source_rgb(cr, 0,  0, 0);   /* set fill color */
        cairo_fill(cr);
    }
}


void drawFibbTime(cairo_t* cr, int width, int height){

    int bh = ((int) ((height - TOP_OFFSET) * BOX_HEIGHT_RATE) / 5) * 5;
    int unit = bh / 5;
    int bw = bh / 5 * 8;
    
    int startpos_y = STARTPOS_Y + TOP_OFFSET + ((height - TOP_OFFSET - bh)/2);
    int startpos_x = (width - 8 * unit) / 2;
    
    myTime t=getTime();
    int colors[5];
    getFibbTime(t, &colors);
    rectangle Rs[5];

    // TOP-LEFT BOX ( size 2 )
    Rs[2].pos_x = startpos_x;
    Rs[2].pos_y = startpos_y;
    Rs[2].width = 2 * unit - GAP_SIZE;
    Rs[2].height = Rs[2].width;
    Rs[2].color = colors[2];

    // BOTTOM-LEFT BOX ( size 3 )
    Rs[1].pos_x = Rs[2].pos_x;
    Rs[1].pos_y = Rs[2].pos_y + (2 * unit);
    Rs[1].width = 3 * unit - GAP_SIZE;
    Rs[1].height = Rs[1].width;
    Rs[1].color = colors[1];

    // TOP SMALL BOX ( size 1 )
    Rs[3].pos_x = Rs[2].pos_x + (2 * unit);
    Rs[3].pos_y = Rs[2].pos_y;
    Rs[3].width = 1 * unit - GAP_SIZE;
    Rs[3].height = Rs[3].width;
    Rs[3].color = colors[3];

    // BOTTOM SMALL BOX ( size 1 )
    Rs[4].pos_x = Rs[3].pos_x;
    Rs[4].pos_y = Rs[3].pos_y + (1 * unit);
    Rs[4].width = Rs[3].width;
    Rs[4].height = Rs[3].width;
    Rs[4].color = colors[4];

    // LARGE BOX ( size 5 )
    Rs[0].pos_x = Rs[3].pos_x + (1 * unit);
    Rs[0].pos_y = Rs[2].pos_y;
    Rs[0].width = 5 * unit - GAP_SIZE;
    Rs[0].height = Rs[0].width;
    Rs[0].color = colors[0];

    drawRectangle(cr, Rs[1]);
    drawRectangle(cr, Rs[2]);
    drawRectangle(cr, Rs[3]);
    drawRectangle(cr, Rs[4]);
    drawRectangle(cr, Rs[0]);
    drawReminder(cr, (t.minute % 5));
}

int main(int argc, char** argv) {

    if(argc > 1){
        if(strcmp(argv[1],"-v") == 0){
            printf("%s\n", VERSION);
            exit(0);
        }
    }

    int width;
    int height;
    Display *d = XOpenDisplay(NULL);
    assert(d);

    int s = DefaultScreen(d);
    Window w = RootWindow(d, s);
    XSync(d, False);
    width = DisplayWidth(d, s);
    height = DisplayHeight(d, s);
    Pixmap pix;
    int depth = DefaultDepth(d, s);
    pix = XCreatePixmap(d, w, width, height, depth);
    Atom prop_root;

	prop_root = XInternAtom(d, "_XROOTPMAP_ID", False);

    cairo_surface_t *surf = cairo_xlib_surface_create(d, pix,
                                  DefaultVisual(d, s),
                                  width, height);
    cairo_t *cr = cairo_create(surf);
    while(1){
        drawFibbTime(cr, width, height);
        XChangeProperty(d, w, prop_root, XA_PIXMAP, 32, PropModeReplace,
            (unsigned char*) &pix, 1);

        XSetWindowBackgroundPixmap(d, w, pix); // If we do not have compositor :)
        XClearWindow(d,w);
        XFlush(d);
        sleep(REFRESH_TIME);
    }
    cairo_destroy(cr);
    cairo_surface_destroy(surf);
    XFreePixmap(d, pix);
    XCloseDisplay(d);
    return 0;
}
