#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <cairo.h>
#include <cairo-xlib.h>

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
    int color[3];
    switch(R.color){
        case 0:
            color[0] = 1;
            color[1] = 1;
            color[2] = 1;
            break;
        case 1:
            color[0] = 1;
            color[1] = 0;
            color[2] = 0;
            break;
        case 2:
            color[0] = 0;
            color[1] = 0;
            color[2] = 1;
            break;
        case 3:
            color[0] = 0;
            color[1] = 1;
            color[2] = 0;
            break;
    }

    cairo_rectangle(cr, R.pos_x, R.pos_y, R.width, R.height);     /* set rectangle */
    cairo_set_source_rgb(cr, color[0], color[1], color[2]);   /* set fill color */
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

void drawFibbTime(cairo_t* cr){
    myTime t=getTime();
    int colors[5];
    getFibbTime(t, &colors);
    rectangle Rs[5];
    Rs[2].pos_x = 10;
    Rs[2].pos_y = 10;
    Rs[2].width = 195;
    Rs[2].height = 195;
    Rs[2].color = colors[2];
    Rs[1].pos_x = 10;
    Rs[1].pos_y = 210;
    Rs[1].width = 295;
    Rs[1].height = 295;
    Rs[1].color = colors[1];
    Rs[3].pos_x = 210;
    Rs[3].pos_y = 10;
    Rs[3].width = 95;
    Rs[3].height = 95;
    Rs[3].color = colors[3];
    Rs[4].pos_x = 210;
    Rs[4].pos_y = 110;
    Rs[4].width = 95;
    Rs[4].height = 95;
    Rs[4].color = colors[4];
    Rs[0].pos_x = 310;
    Rs[0].pos_y = 10;
    Rs[0].width = 495;
    Rs[0].height = 495;
    Rs[0].color = colors[0];
    drawRectangle(cr, Rs[1]);
    drawRectangle(cr, Rs[2]);
    drawRectangle(cr, Rs[3]);
    drawRectangle(cr, Rs[4]);
    drawRectangle(cr, Rs[0]);
    drawReminder(cr, (t.minute % 5));
}

int main() {
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
    drawFibbTime(cr);
    XChangeProperty(d,   /* connection to x server */
            w,    /* window whose property we want to change */
            //prop_root = XInternAtom(disp2, "_XROOTPMAP_ID", False);
            prop_root,   /* property name */
            XA_PIXMAP, /* type of property */
            32,         /* format of prop; can be 8, 16, 32 */
            PropModeReplace,
            (unsigned char*) &pix, /* actual data */
            1         /* number of elements */
            );

	XSetWindowBackgroundPixmap(d, w, pix); // If we do not have compositor :)
    XClearWindow(d,w);
    XFlush(d);
    sleep(30);
    }
    /* cairo_destroy(cr); */
    /* cairo_surface_destroy(surf); */
    /* XFreePixmap(d, pix); */
    XCloseDisplay(d);
    return 0;
}
