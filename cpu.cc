#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <time.h>

#define imgchannels 1

int main(){
    int width, height, bpp;
    unsigned char *img, *seq_img, *seq_img_device_in_host;
    clock_t start_CPU;

    const unsigned char* image = stbi_load( "Images/Grayscale_512.png", &width, &height, &bpp, imgchannels );
    img = (unsigned char*)malloc(width*height*sizeof(unsigned char));
    seq_img = (unsigned char*)malloc(width*height*sizeof(unsigned char));
    seq_img_device_in_host = (unsigned char*)malloc(width*height*sizeof(unsigned char));

    std::cout << "height " << height << " " << width << std::endl;

    printf("Image[0] = %d\n", image[0]);
    printf("Image[1] = %d\n", image[1]);
    printf("Image[2] = %d\n", image[2]);
    printf("Image[3] = %d\n", image[3]);
    printf("Image[4] = %d\n", image[4]);
    // Design kernel
    // float hostMaskData[maskRows*maskCols];
    // for(int i=0; i< maskCols*maskCols; i++){
        // hostMaskData[i] = 1.0/(maskRows*maskCols);
    // }



    // stbi_write_png("mynew_seq.png", width, height, imgchannels, seq_img, 0);


    return 0;
}
