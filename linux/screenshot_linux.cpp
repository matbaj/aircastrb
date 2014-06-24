#include <stdio.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <jpeglib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <X11/extensions/XShm.h>
#include "screenshot_linux.h"


void init_buffer(struct jpeg_compress_struct* cinfo) {}
 
boolean empty_buffer(struct jpeg_compress_struct* cinfo) {
    return TRUE;
}
     
void term_buffer(struct jpeg_compress_struct* cinfo) {}
      
int get_jpeg(XImage *img, unsigned char* my_buffer) {

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
    jpeg_set_quality (&cinfo, 90, 1);
    jpeg_start_compress(&cinfo, 1);
 
    JSAMPROW row_ptr;

    tmp = (char*)malloc(sizeof(char)*3*img->width*img->height);
    //memcpy(tmp,img->data,3*img->width*img->height);
    /*for (y = 0; y < img->height; y++) {
        for (x = 0; x < img->width; x++) {
            pixel = XGetPixel(img,x,y);
            tmp[y*img->width*3+x*3+0] = (pixel>>16);
            tmp[y*img->width*3+x*3+1] = ((pixel&0x00ff00)>>8);
            tmp[y*img->width*3+x*3+2] = (pixel&0x0000ff);
        }
    }*/
    int i =0;
    int j=0;
    while(j<4*img->width*img->height)
    {
        tmp[i]=img->data[j+2];
        tmp[i+1]=img->data[j+1];
        tmp[i+2]=img->data[j];
        j+=4;
        i+=3;
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
    
    XImage *image;
    XShmSegmentInfo shminfo;

    Display *d = XOpenDisplay(0);
    if (d == NULL) return 2;
    int size;
    int screen;
    Window root = RootWindow(d,DefaultScreen(d));
    //XWindowAttributes rootattr;
    //XGetWindowAttributes(d, root, &rootattr);

    //XImage *img = XGetImage(dpy,root,0,0, 1366, 768, XAllPlanes(),ZPixmap);
    screen = DefaultScreen(d);
    image = XShmCreateImage(d,DefaultVisual(d,screen),DefaultDepth(d,screen),ZPixmap,NULL,&shminfo,1366,768);
    shminfo.shmid = shmget (IPC_PRIVATE,image->bytes_per_line*image->height,IPC_CREAT|0777);
    shminfo.shmaddr = image->data = (char*)shmat (shminfo.shmid,0,0);
    shminfo.readOnly = False;

    XShmAttach(d,&shminfo);
    //XSync( d, False );
    
    //GC gc;
    //gc = XCreateGC( d, root, 0, NULL );
    //XShmPutImage(d,root,gc,image,0,0,0,0,1366,768, 1); 
    //int CompletionType = XShmGetEventBase (d) + ShmCompletion;
    
    XShmGetImage (d, root,image,0,0, AllPlanes);

    if (image == NULL) {
        perror("XGetImage");
        return 3;
    } else {
        //size = get_jpeg(img, my_buffer);
        //XDestroyImage(img);
        size = get_jpeg(image, my_buffer);
        XDestroyImage (image);
        XShmDetach (d, &shminfo);
        XCloseDisplay(d);
        shmdt (shminfo.shmaddr);
        shmctl (shminfo.shmid, IPC_RMID, 0);
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