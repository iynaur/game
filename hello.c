// Simple Xlib application drawing a box in a window.
// gcc hello.c -o output -lX11 -lpthread

#include <unistd.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int angle1 = 0;
const int angle2 = 360 * 64;

int window_length = 800;
int window_width = 600;

int pad_where_y = 560;

Display *display;
Window window;

char *msg = "Hello, World!";
char text[255];
int s;

int pad_length = 50;
int pad_width = 10;

int circle_x = 100;
int circle_y = 100;

int v_x = 2;
int v_y = 2;

int circle_r = 10;

int pad_where_x = 400;

pthread_mutex_t mutex_draw;
pthread_mutex_t mutex_exit;

int exit_threads = 0;

void* timeout_cb() {
    int Ax = circle_x + circle_r/2;
    int Ay = circle_y;
    //int Bx = circle_x + circle_r;
    //int By = circle_y + circle_r/2;
    int Cx = circle_x + circle_r/2;
    int Cy = circle_y + circle_r;
    //int Dx = circle_x;
    //int Dy = circle_y + circle_r/2;
/*    if (Ay <= pad_where_y) {
      circle_x = 100;
      circle_y = 100;
      v_x = 0;
      v_y = 0;
    }
    if (Cx >= pad_where_x) {
      circle_x = 200;
      circle_y = 200;
      v_x = 0;
      v_y = 0;
    }
    if (Cx <= pad_where_x + pad_length) {
      circle_x = 100;
      circle_y = 100;
      v_x = 0;
      v_y = 0;
    }*/
//    if (Cy >= pad_where_y &&
//        Ay <= pad_where_y) { // &&
//        Cx >= pad_where_x &&
//        Cx <= pad_where_x + pad_length) {
//      v_y = -v_y;
//    } else if (Cy >= pad_where_y) {
    if (Cy >= pad_where_y && Ay <= pad_where_y) {
      if (Cx >= pad_where_x && Cx <= pad_where_x + pad_length) {
        v_y = -v_y;
      } else {
        v_x = 0;
        v_y = 0;
      }
    }
    if (v_x > 0) {
      if (circle_x + circle_r < window_length) {
        circle_x += v_x;
      } else {
        v_x = -v_x;
        circle_x += v_x;
      }
    } else if (v_x < 0) {
      if (circle_x > 0) {
        circle_x += v_x;
      } else {
        v_x = -v_x;
        circle_x += v_x;
      }
    }
    if (v_y > 0) {
      if (circle_y + circle_r < window_width) {
        circle_y += v_y;
      } else {
        v_y = -v_y;
        circle_y += v_y;
      }
    } else if (v_y < 0) {
      if (circle_y > 0) {
        circle_y += v_y;
      } else {
        v_y = -v_y;
        circle_y += v_y;
      }
    }

  pthread_mutex_lock(&mutex_draw);
  XClearWindow(display, window);
  XFillRectangle(display, window, DefaultGC(display, s), pad_where_x,
      pad_where_y, pad_length, pad_width);
  XFillArc(display, window, DefaultGC(display, s), circle_x, circle_y,
      circle_r, circle_r, angle1, angle2);
  pthread_mutex_unlock(&mutex_draw);
  //msg = "Hello, Ania!";
  //XDrawString(display, window, DefaultGC(display, s), 50, 50, msg, strlen(msg));
  XFlush(display);
}

static void* g_start_timer(void *args) {
  int msecs = (int)args;

  while(1) {
    usleep(msecs);
    timeout_cb();
    pthread_mutex_lock(&mutex_exit);
    if (exit_threads) {
      pthread_mutex_unlock(&mutex_exit);
      break;
    }
    pthread_mutex_unlock(&mutex_exit);
  }
  pthread_exit(NULL);
}

int main() {
  //Display *display;
  //Window window;
  XEvent event;

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

  pthread_t thread_id;
  int msecs = 10000;
  int rc;
  rc = pthread_create(&thread_id, NULL, g_start_timer, (void *) msecs);
  //pthread_join(thread_id, NULL);

  /* event loop */
  while (1) {
    XNextEvent(display, &event);

    /* exit on key press */
    if (event.type == KeyPress) {
      KeySym key;
      XLookupString(&event, text, 255, &key, None);
      //XClearWindow(display, window);
      if (key == XK_Left) {
        if (pad_where_x > 0) {
          pad_where_x -= 5;
        }
      } else if (key == XK_Right) {
        if (pad_where_x < window_length - pad_length) {
          pad_where_x += 5;
        }
      }
      if (key == XK_q) {
        break;
      }
    }
    pthread_mutex_lock(&mutex_draw);
    XClearWindow(display, window);
    XFillRectangle(display, window, DefaultGC(display, s), pad_where_x,
        pad_where_y, pad_length, pad_width);
    XFillArc(display, window, DefaultGC(display, s), circle_x, circle_y,
        circle_r, circle_r, angle1, angle2);
    //XDrawString(display, window, DefaultGC(display, s), 50, 50, msg,
    //    strlen(msg));
    XFlush(display);
    pthread_mutex_unlock(&mutex_draw);
    

  }

  /* close connection to server */
  XCloseDisplay(display);
  pthread_mutex_lock(&mutex_exit);
  exit_threads = 1;
  pthread_mutex_unlock(&mutex_exit);
  pthread_join(thread_id, NULL);
  //pthread_exit(NULL);
}

