#include <assert.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <cairo.h>
#include <cairo-xlib.h>

int width, height;
void draw(cairo_t *cr) {
    int quarter_w = width / 4;
    int quarter_h = height / 4;
    cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
    cairo_rectangle(cr, quarter_w, quarter_h, quarter_w * 2, quarter_h * 2);
    cairo_fill(cr);
}

int main() {
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
    draw(cr);
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

	/* XSetWindowBackgroundPixmap(d, w, pix); // If we do not have compositor :) */
    XClearWindow(d,w);
    XFlush(d);
    /* cairo_destroy(cr); */
    /* cairo_surface_destroy(surf); */
    /* XFreePixmap(d, pix); */
    XCloseDisplay(d);
    return 0;
}
