#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <jpeglib.h>
#include "screenshot_linux.h"


void init_buffer(struct jpeg_compress_struct* cinfo) {}
 
boolean empty_buffer(struct jpeg_compress_struct* cinfo) {
    return TRUE;
}
     
void term_buffer(struct jpeg_compress_struct* cinfo) {}
      
int get_jpeg(XImage *img, unsigned char* my_buffer) {

    unsigned long pixel;
    int x, y;
    char* tmp;

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;
    struct jpeg_destination_mgr dmgr;

    dmgr.init_destination    = init_buffer;
    dmgr.empty_output_buffer = empty_buffer;
    dmgr.term_destination    = term_buffer;
    dmgr.next_output_byte    = my_buffer;
    dmgr.free_in_buffer      = img->width * img->height *3;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    cinfo.dest = &dmgr;
 
    cinfo.image_width      = img->width;
    cinfo.image_height     = img->height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;
 
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, 70, 1);
    jpeg_start_compress(&cinfo, 1);
 
    JSAMPROW row_ptr;

    tmp = (char*)malloc(sizeof(char)*3*img->width*img->height);
    for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            pixel = XGetPixel(img,x,y);
            tmp[y*img->width*3+x*3+0] = (pixel>>16);
            tmp[y*img->width*3+x*3+1] = ((pixel&0x00ff00)>>8);
            tmp[y*img->width*3+x*3+2] = (pixel&0x0000ff);
        }
    }

    while (cinfo.next_scanline < cinfo.image_height) {
        row_ptr = (JSAMPROW) &tmp[cinfo.next_scanline*(img->depth>>3)*img->width];
        jpeg_write_scanlines(&cinfo, &row_ptr, 1);
    }
    jpeg_finish_compress(&cinfo);
 
    int compressed_size = cinfo.dest->next_output_byte - my_buffer;

    free(tmp);
    return compressed_size;
}


int screenshot(unsigned char* my_buffer) {

    Display *dpy = XOpenDisplay(0);
    if (dpy == NULL) return 2;
    int size;
    Window root = RootWindow(dpy,DefaultScreen(dpy));
    XWindowAttributes rootattr;
    XGetWindowAttributes(dpy, root, &rootattr);

    XImage *img = XGetImage(dpy,root,0,0, rootattr.width, rootattr.height, XAllPlanes(),ZPixmap);
    XCloseDisplay(dpy);


    if (img == NULL) {
        perror("XGetImage");
        return 3;
    } else {
        size = get_jpeg(img, my_buffer);
        XDestroyImage(img);
    }

    return size;
}

int getX()
{
    Display *dpy = XOpenDisplay(0);
    if (dpy == NULL) return 2;
    Window root = RootWindow(dpy,DefaultScreen(dpy));
    XWindowAttributes rootattr;
    XGetWindowAttributes(dpy, root, &rootattr);
    XCloseDisplay(dpy);
    return rootattr.width;

}
int getY()
{
    Display *dpy = XOpenDisplay(0);
    if (dpy == NULL) return 2;
    Window root = RootWindow(dpy,DefaultScreen(dpy));
    XWindowAttributes rootattr;
    XGetWindowAttributes(dpy, root, &rootattr);
    XCloseDisplay(dpy);
    return rootattr.height;
}
