// Simple Xlib application drawing a box in a window.
// gcc hello.c -o output -lX11

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  Display *display;
  Window window;
  XEvent event;
  char *msg = "Hello, World!";
  char text[255];
  int s;

  int pad_length = 50;
  int pad_width = 10;

  int window_length = 800;
  int window_width = 600;

  int pad_where_x = 40;

  /* open connection with the server */
  display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }

  s = DefaultScreen(display);

  /* create window */
  window = XCreateSimpleWindow(display, RootWindow(display, s), 0, 0,
      window_length, window_width, 1, BlackPixel(display, s),
      WhitePixel(display, s));

  /* select kind of events we are interested in */
  XSelectInput(display, window, ExposureMask | KeyPressMask);

  /* map (show) the window */
  XMapWindow(display, window);

  /* event loop */
  while (1) {
    XNextEvent(display, &event);

    /* draw or redraw the window */
    if (event.type == Expose) {
      XFillRectangle(display, window, DefaultGC(display, s), pad_where_x, 20,
          pad_length, pad_width);
      XDrawString(display, window, DefaultGC(display, s), 50, 50, msg,
          strlen(msg));
    }
    /* exit on key press */
    if (event.type == KeyPress) {
      KeySym key;
      XLookupString(&event, text, 255, &key, None);
      XClearWindow(display, window);
      if (key == XK_Left) {
        if (pad_where_x > 0) {
          pad_where_x -= 5;
        }
      } else if (key == XK_Right) {
        if (pad_where_x < window_length - pad_length) {
          pad_where_x += 5;
        }
      }
      XFillRectangle(display, window, DefaultGC(display, s), pad_where_x, 20,
          pad_length, pad_width);
      XFlush(display);
      if (key == XK_q) {
        break;
      }
    }
  }

  /* close connection to server */
  XCloseDisplay(display);

  return 0;
}

