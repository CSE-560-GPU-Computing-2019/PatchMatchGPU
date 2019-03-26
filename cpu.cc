#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <time.h>

#define imgchannels 3

// g -> Grayscale
// c -> Color
int main(){
    int g_width, g_height, g_bpp;
    int c_width, c_height, c_bpp;
    unsigned char *img, *seq_img, *seq_img_device_in_host;
    unsigned char *g_image_cpy;

    // const unsigned char *g_image = stbi_load("Images/Grayscale_512.png", &g_width, &g_height, &g_bpp, imgchannels );
    // const unsigned char *c_image = stbi_load("Images/Color_512.png", &c_width, &c_height, &c_bpp, imgchannels);
    unsigned char *g_image = stbi_load("Images/512/forest_color.jpg", &g_width, &g_height, &g_bpp, imgchannels );

    int i = 0;
    int j = 0;
    unsigned bytePerPixel = imgchannels;
    unsigned char *pixelOffset = g_image + (i + g_height * j) * imgchannels;
    unsigned char r = pixelOffset[0];
    unsigned char g = pixelOffset[1];
    unsigned char b = pixelOffset[2];
    // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
    printf("rgb(%d, %d) = (%d, %d, %d)\n", i, j, r, g, b);

    i = 0;
    j = 1;
    bytePerPixel = imgchannels;
    pixelOffset = g_image + (i + g_height * j) * imgchannels;
    r = pixelOffset[0];
    g = pixelOffset[1];
    b = pixelOffset[2];
    // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
    printf("rgb(%d, %d) = (%d, %d, %d)\n", i, j, r, g, b);

    i = 0;
    j = 2;
    bytePerPixel = imgchannels;
    pixelOffset = g_image + (i + g_height * j) * imgchannels;
    r = pixelOffset[0];
    g = pixelOffset[1];
    b = pixelOffset[2];
    // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
    printf("rgb(%d, %d) = (%d, %d, %d)\n", i, j, r, g, b);

    i = 1;
    j = 0;
    bytePerPixel = imgchannels;
    pixelOffset = g_image + (i + g_height * j) * imgchannels;
    r = pixelOffset[0];
    g = pixelOffset[1];
    b = pixelOffset[2];
    // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
    printf("rgb(%d, %d) = (%d, %d, %d)\n", i, j, r, g, b);

    i = 2;
    j = 0;
    bytePerPixel = imgchannels;
    pixelOffset = g_image + (i + g_height * j) * imgchannels;
    r = pixelOffset[0];
    g = pixelOffset[1];
    b = pixelOffset[2];
    // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
    printf("rgb(%d, %d) = (%d, %d, %d)\n", i, j, r, g, b);


    i = 40;
    j = 40;
    bytePerPixel = imgchannels;
    pixelOffset = g_image + (i + g_height * j) * imgchannels;
    r = pixelOffset[0];
    g = pixelOffset[1];
    b = pixelOffset[2];
    // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
    printf("rgb(%d, %d) = (%d, %d, %d)\n", i, j, r, g, b);

    i = 499;
    j = 499;
    bytePerPixel = imgchannels;
    pixelOffset = g_image + (i + g_height * j) * imgchannels;
    r = pixelOffset[0];
    g = pixelOffset[1];
    b = pixelOffset[2];
    // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
    printf("rgb(%d, %d) = (%d, %d, %d)\n", i, j, r, g, b);

    printf("Image[0] = %d\n", g_image[0]);
    printf("Image[1] = %d\n", g_image[1]);
    printf("Image[2] = %d\n", g_image[2]);
    printf("Image[3] = %d\n", g_image[3]);
    printf("Image[4] = %d\n", g_image[4]);
    printf("Image[4] = %d\n", g_image[5]);
    printf("Image[4] = %d\n", g_image[6]);
    printf("Image[4] = %d\n", g_image[7]);
    printf("Image[4] = %d\n", g_image[8]);
    printf("Image[4] = %d\n", g_image[9]);

    // for(int i=0; i < g_height; i++){
    //     for(int j=0; j < g_width; j++){
    //         unsigned bytePerPixel = imgchannels;
    //         unsigned char *pixelOffset = g_image + (i + g_height * j) * bytePerPixel;
    //         unsigned char r = pixelOffset[0];
    //         unsigned char g = pixelOffset[1];
    //         unsigned char b = pixelOffset[2];
    //         std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
    //     }
    // }

    g_image_cpy = (unsigned char*) malloc(g_width * g_height * sizeof(unsigned char));

    std::cout << "Grayscale : " << g_height << " " << g_width << std::endl;
    // std::cout << "Color     : " << c_height << " " << c_width << std::endl;


    // for(int i=0; i< g_height * g_width * imgchannels; i++){
    //     g_image_cpy[i] = g_image[i];
    // }

    // stbi_write_jpg("Images/512/forest_graysasdf.jpg", g_height, g_width, imgchannels, g_image_cpy, 0);

    return 0;
}
