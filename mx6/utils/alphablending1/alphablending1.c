#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/mxcfb.h>
#include <linux/ipu.h>

#define MAX_ALPHA 255

int main (int argc, char *argv[])
{
    struct fb_var_screeninfo fb0_var;
    struct fb_fix_screeninfo fb0_fix;
    int fd_fb0;

    struct mxcfb_gbl_alpha g_alpha;
    struct mxcfb_loc_alpha l_alpha;

    // Open Framebuffer and gets its address
    if ((fd_fb0 = open("/dev/fb1", O_RDWR, 0)) < 0) {
        printf("Unable to open /dev/fb1\n");
        goto done;
    }

    if ( ioctl(fd_fb0, FBIOGET_FSCREENINFO, &fb0_fix) < 0) {
        printf("Get FB fix info failed!\n");
        close(fd_fb0);
        goto done;
    }

    if ( ioctl(fd_fb0, FBIOGET_VSCREENINFO, &fb0_var) < 0) {
        printf("Get FB var info failed!\n");
        close(fd_fb0);
        goto done;
    }

    printf("\nFB information \n");
    printf("xres = %d\n",  fb0_var.xres);
    printf("xres_virtual = %d\n",  fb0_var.xres_virtual);
    printf("yres = %d\n",  fb0_var.yres);
    printf("yres_virtual = %d\n",  fb0_var.yres_virtual);
    printf("bits_per_pixel = %d\n",  fb0_var.bits_per_pixel);
    printf("pixclock = %d\n",  fb0_var.pixclock);
    printf("height = %d\n",  fb0_var.height);
    printf("width = %d\n",  fb0_var.width);
    printf("Pixel format : RGBX_%d%d%d%d\n",fb0_var.red.length,
                                                 fb0_var.green.length,
                                                 fb0_var.blue.length,
                                                 fb0_var.transp.length);
    printf(" Begin of bitfields(Byte ordering):-\n");
    printf("  Red    : %d\n",fb0_var.red.offset);
    printf("  Blue   : %d\n",fb0_var.blue.offset);
    printf("  Green  : %d\n",fb0_var.green.offset);
    printf("  Transp : %d\n",fb0_var.transp.offset);

 /*   if (argc < 2)
    {
        printf(" Put alpha value as argument [0 - 255] - 0: transparent \n");
        exit(-1);
    }
*/
    /* Disable global alpha */
    g_alpha.alpha = MAX_ALPHA;
    g_alpha.enable = 0;

    if (ioctl(fd_fb0, MXCFB_SET_GBL_ALPHA, &g_alpha) < 0) {
        printf("Set global alpha failed\n");
        close(fd_fb0);
        goto done;
    }

    /* Enable local alpha*/
    l_alpha.enable = 1;
    l_alpha.alpha_in_pixel = 1;
    l_alpha.alpha_phy_addr0 = 0;
    l_alpha.alpha_phy_addr1 = 0;

    if (ioctl(fd_fb0, MXCFB_SET_LOC_ALPHA,&l_alpha) < 0) {
        printf("Set local alpha failed\n");
        close(fd_fb0);
        goto done;
    }

    close(fd_fb0);
    return 0;

done:
    return -1;
}
