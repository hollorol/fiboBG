#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xinerama.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include "config.h"

#define VERSION "0.100"

volatile sig_atomic_t running = 1;

void handle_signal(int signum) {
    (void)signum; // Acknowledge parameter to prevent unused warning
    running = 0;
}

// --- Struct Definitions ---
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
} myTime;

// --- Time Functions ---
myTime getTime(void) {
    time_t raw_time;
    time(&raw_time);
    struct tm *currTime;
    currTime = localtime(&raw_time);
    myTime t;
    t.hour = currTime->tm_hour;
    t.minute = currTime->tm_min;
    return t;
}

void print_current_time(void) {
    myTime t = getTime();
    printf("%02d:%02d\n", t.hour, t.minute);
}

// --- Fibonacci Logic ---
void getFibbTime(myTime t, int *res) {
    int minute = t.minute;
    minute = (minute / 5) * 5;
    int hour = t.hour % 12;
    if (hour == 0) {
        hour = 12;
    }
    int fibBlocks[5] = {5, 3, 2, 1, 1};
    int mins[5] = {0, 0, 0, 0, 0};
    int hours[5] = {0, 0, 0, 0, 0};
    int blockElement = 0;

    // Calculate minutes representation
    while (minute != 0) {
        if (blockElement >= 5) break; // Safety break
        if (fibBlocks[blockElement] * 5 > minute) {
            blockElement++;
            continue;
        }
        mins[blockElement] = 1;
        minute -= fibBlocks[blockElement] * 5;
        blockElement++;
    }

    // Calculate hours representation
    blockElement = 0;
    while (hour != 0) {
         if (blockElement >= 5) break; // Safety break
        if (fibBlocks[blockElement] > hour) {
            blockElement++;
            continue;
        }
        hours[blockElement] = 1;
        hour -= fibBlocks[blockElement];
        blockElement++;
    }

    // Combine results (0=off, 1=min, 2=hour, 3=both)
    for (int i = 0; i < 5; ++i) {
        *(res + i) = mins[i] + 2 * hours[i];
    }

    // Handle ambiguity of the two '1' blocks
    if ((*(res + 3) == 3) && (*(res + 4) == 0)) {
        *(res + 3) = 2; // Assign first '1' block to hour
        *(res + 4) = 1; // Assign second '1' block to minute
    }
}

// --- Drawing Functions ---
void drawRectangle(cairo_t *cr, rectangle R) {
    if (R.width <= 0 || R.height <= 0) return; // Don't draw invalid rectangles
    color rectColor = colors[R.color]; // colors is defined in config.h
    cairo_rectangle(cr, R.pos_x, R.pos_y, R.width, R.height); /* set rectangle */
    cairo_set_source_rgb(cr, rectColor.red, rectColor.green, rectColor.blue); /* set fill color */
    cairo_fill(cr); /* fill rectangle */
}

// Modified to draw the clock centered within a specific target area
void drawFibbTimeOnArea(cairo_t *cr, int target_x, int target_y, int target_width, int target_height) {

    // Calculate available drawing area within the target rectangle, considering offsets from config.h
    int available_width = target_width - LEFT_OFFSET - RIGHT_OFFSET;
    int available_height = target_height - TOP_OFFSET - BOTTOM_OFFSET;

    if (available_width <= 0 || available_height <= 0) return; // Area too small

    // Calculate unit size based on available height and desired ratio
    int bh = ((int)(available_height * BOX_HEIGHT_RATE) / 5) * 5;
    int unit = bh / 5;
    if (unit <= 0) return; // Unit size too small

    // Calculate total size of the Fibonacci layout (8 units wide, 5 units high)
    int total_layout_width = 8 * unit;
    int total_layout_height = 5 * unit;

    // Check if layout fits within available space
    if (total_layout_width > available_width || total_layout_height > available_height) {
       // Optionally scale down or just don't draw if it doesn't fit?
       // For simplicity, let's just prevent drawing very small layouts if unit is tiny above.
       // A more robust solution might scale 'unit' based on width too.
    }

    // Calculate starting position to center the layout within the available area
    int startpos_x = target_x + LEFT_OFFSET + (available_width - total_layout_width) / 2;
    int startpos_y = target_y + TOP_OFFSET + (available_height - total_layout_height) / 2;

    // Get time and calculate colors
    myTime t = getTime();
    int block_colors[5]; // Renamed from 'colors' to avoid conflict with config.h 'colors' array
    getFibbTime(t, (int *)&block_colors);
    rectangle Rs[5];

    // Define rectangle geometry relative to startpos_x/y and unit
    // TOP-LEFT BOX ( size 2 ) - Index 2
    Rs[2].pos_x = startpos_x;
    Rs[2].pos_y = startpos_y;
    Rs[2].width = 2 * unit - GAP_SIZE;
    Rs[2].height = Rs[2].width;
    Rs[2].color = block_colors[2];

    // BOTTOM-LEFT BOX ( size 3 ) - Index 1
    Rs[1].pos_x = Rs[2].pos_x;
    Rs[1].pos_y = Rs[2].pos_y + (2 * unit);
    Rs[1].width = 3 * unit - GAP_SIZE;
    Rs[1].height = Rs[1].width;
    Rs[1].color = block_colors[1];

    // TOP SMALL BOX ( size 1 ) - Index 3
    Rs[3].pos_x = Rs[2].pos_x + (2 * unit);
    Rs[3].pos_y = Rs[2].pos_y;
    Rs[3].width = 1 * unit - GAP_SIZE;
    Rs[3].height = Rs[3].width;
    Rs[3].color = block_colors[3];

    // BOTTOM SMALL BOX ( size 1 ) - Index 4
    Rs[4].pos_x = Rs[3].pos_x;
    Rs[4].pos_y = Rs[3].pos_y + (1 * unit);
    Rs[4].width = Rs[3].width;
    Rs[4].height = Rs[3].width;
    Rs[4].color = block_colors[4];

    // LARGE BOX ( size 5 ) - Index 0
    Rs[0].pos_x = Rs[3].pos_x + (1 * unit);
    Rs[0].pos_y = Rs[2].pos_y;
    Rs[0].width = 5 * unit - GAP_SIZE;
    Rs[0].height = Rs[0].width;
    Rs[0].color = block_colors[0];

    // Draw the rectangles
    // Order might matter visually if gaps are negative, draw largest last? Or smallest?
    // Current order seems fine for positive gaps.
    drawRectangle(cr, Rs[1]);
    drawRectangle(cr, Rs[2]);
    drawRectangle(cr, Rs[3]);
    drawRectangle(cr, Rs[4]);
    drawRectangle(cr, Rs[0]);
}

// --- Main Function ---
int main(int argc, char **argv) {

    // --- Argument Parsing ---
    if (argc > 1) {
        if (strcmp(argv[1], "-v") == 0) {
            printf("fiboBG Version: %s\n", VERSION);
            exit(0);
        }
        // Add other arguments here if needed
    }

    // --- X11 Setup ---
    Display *d = XOpenDisplay(NULL);
    if (!d) {
         fprintf(stderr, "Error: Cannot open display.\n");
         return 1;
    }

    int screen_num = DefaultScreen(d);
    Window root_window = RootWindow(d, screen_num);
    int root_width = DisplayWidth(d, screen_num);   // Full virtual screen width
    int root_height = DisplayHeight(d, screen_num); // Full virtual screen height
    int root_depth = DefaultDepth(d, screen_num);

    Pixmap pix = XCreatePixmap(d, root_window, root_width, root_height, root_depth);
    Atom prop_root_pixmap = XInternAtom(d, "_XROOTPMAP_ID", False);

    // --- Cairo Setup ---
    cairo_surface_t *surf = cairo_xlib_surface_create(d, pix,
                                                    DefaultVisual(d, screen_num),
                                                    root_width, root_height);
    cairo_t *cr = cairo_create(surf);


    // --- Xinerama Setup ---
    XineramaScreenInfo *xinerama_screens = NULL;
    int num_screens = 0;
    int xinerama_major, xinerama_minor;
    Bool xinerama_available = XineramaQueryExtension(d, &xinerama_major, &xinerama_minor);

    if (xinerama_available && XineramaIsActive(d)) {
        xinerama_screens = XineramaQueryScreens(d, &num_screens);
        if (!xinerama_screens || num_screens <= 0) {
            fprintf(stderr, "Warning: Xinerama active but failed to query screens or none found. Falling back to single screen mode.\n");
            num_screens = 0; // Force fallback
            if (xinerama_screens) XFree(xinerama_screens); // Free if allocated but num_screens was 0
            xinerama_screens = NULL;
        } else {
             printf("Info: Xinerama detected %d screen(s).\n", num_screens);
        }
    } else {
        fprintf(stderr, "Info: Xinerama not available or not active. Running in single screen mode.\n");
        num_screens = 0; // Ensure fallback
    }


    // --- Register Signal Handlers ---
    signal(SIGINT, handle_signal);  // Catch Ctrl+C
    signal(SIGTERM, handle_signal); // Catch standard termination signal

    // --- Main Loop ---
    printf("Fibonacci Clock: Starting background update loop...\n");
    while (running) {
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        cairo_paint(cr);

        if (num_screens > 0 && xinerama_screens) {
            for (int i = 0; i < num_screens; ++i) {
                drawFibbTimeOnArea(cr,
                                   xinerama_screens[i].x_org,
                                   xinerama_screens[i].y_org,
                                   xinerama_screens[i].width,
                                   xinerama_screens[i].height);
            }
        } else {
            drawFibbTimeOnArea(cr, 0, 0, root_width, root_height);
        }

        XChangeProperty(d, root_window, prop_root_pixmap, XA_PIXMAP, 32, PropModeReplace,
                        (unsigned char *)&pix, 1);

        XSetWindowBackgroundPixmap(d, root_window, pix);
        XClearWindow(d, root_window);

        XFlush(d);

        sleep(REFRESH_TIME); 
    }

    // --- Cleanup ---
    printf("\nFibonacci Clock: Shutting down gracefully...\n");
    cairo_destroy(cr);
    cairo_surface_destroy(surf);
    XFreePixmap(d, pix);
    if (xinerama_screens) { // Free Xinerama screen info if allocated
        XFree(xinerama_screens);
    }
    XCloseDisplay(d);

    return 0;
}
