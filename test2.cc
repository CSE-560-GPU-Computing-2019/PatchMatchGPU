#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <time.h>
#include <math.h>

#define imgchannels 3
#define maskCols 5
#define maskRows 5
// #define THRESHOLD 100 // In percentage
#define MAXLEN 1024 // Max length of image paths
#define NUMSAMPLE 500

struct image_holder
{
    int lum;
    int fv;
    unsigned char *image;
};

int THRESHOLD = 0;

unsigned char *getRGBOffset(int i, int j, unsigned char *c_image, int img_height, int img_width)
{
    return c_image + (i + img_height * j) * 3;
}

// To access rgb values of colored image, on the returned value index 0 is R, index 1 if G and 2 is B
unsigned char *getIMGOffset(int i, int j, unsigned char *c_image, int img_height, int img_width)
{
    return c_image + (i + img_height * j);
}

void generatePathNames(char *sizeOfAllImage, char *grayscaleInputName, char *coloredImageName,
                       char *coloredAsGrayscaleImageName, char *grayscaleImagePath,
                       char *coloredImagePath, char *coloredAsGrayscaleImagePath,
                       char *outputImagePath)
{
    char folderName[] = "Images";
    char ch;
    snprintf(grayscaleImagePath, MAXLEN, "%s/%s/%s", folderName, sizeOfAllImage, grayscaleInputName);
    snprintf(coloredImagePath, MAXLEN, "%s/%s/%s", folderName, sizeOfAllImage, coloredImageName);
    snprintf(coloredAsGrayscaleImagePath, MAXLEN, "%s/%s/%s", folderName, sizeOfAllImage, coloredAsGrayscaleImageName);
    snprintf(outputImagePath, MAXLEN, "%s/%s/", folderName, sizeOfAllImage);
    strncat(outputImagePath, grayscaleInputName, strrchr(grayscaleInputName, '.') - grayscaleInputName);
    strcat(outputImagePath, "_colored_test.jpg");

    // printf("Input . start at %s\n", strrchr(grayscaleInputName, '.'));

    printf("Path generated are: \n");
    printf("grayscaleImagePath: %s\n", grayscaleImagePath);
    printf("coloredImagePath: %s\n", coloredImagePath);
    printf("coloredAsGrayscaleImagePath: %s\n", coloredAsGrayscaleImagePath);
    printf("outputImagePath: %s\n", outputImagePath);
    printf("Press any key to continue\n");
    scanf("%c", &ch);
}

void copyGrayscaleToFinal(unsigned char *finalImage, const unsigned char *g_image, int dataSizeX, int dataSizeY)
{
    for (int i = 0; i < dataSizeX; ++i)
    {
        for (int j = 0; j < dataSizeY; ++j)
        {
            // if (finalImage[i + j * dataSizeX] != '\0')
            // continue;
            finalImage[(i + dataSizeX * j) * 3] = g_image[i + j * dataSizeX];
            finalImage[1 + (i + dataSizeX * j) * 3] = g_image[i + j * dataSizeX];
            finalImage[2 + (i + dataSizeX * j) * 3] = g_image[i + j * dataSizeX];
        }
    }
}

int getMean(unsigned char *item, int width, int height)
{
    int mean = 0;
    int sum = 0;
    for (int i = 0; i < width * height; i++)
    {
        sum += item[i];
    }

    mean = sum / (width * height);

    return mean;
}

int luminance_remap(unsigned char *source, unsigned char *target, int width, int height) {
    unsigned char *source_luminance = (unsigned char *) malloc(width * height * sizeof(unsigned char));
    int remapped = 0;

    for (int i=0; i < width * height; i ++) {
        source_luminance[i] = source[i]/100;
    }

    for (int i = 0; i < width * height; i++)
    {
        printf("%d\n", (float) source_luminance[i]);
    }
 
    // mu_a = getMean(source, );
    // mu_b = mean(target(:));
    // sigma_a = std(source_luminance(:));
    // sigma_b = std(target(:));
    // remapped = sigma_b / sigma_a * (source_luminance - mu_a) + mu_b;
    return remapped;
}



int main(int argc, char *argv[])
{

    struct image_holder c_source;
    struct image_holder c_as_g_source;
    struct image_holder g_source;
    struct image_holder target;

    if (argc != 2)
    {
        printf("Usage: ./cpu <THRESHOLD>");
        return 1;
    }

    THRESHOLD = atoi(argv[1]);
    char sizeOfAllImage[] = "256";                           // Must be a square image and all must be of the same size
    char grayscaleInputName[] = "forest_grays.jpg";          // Image to be colored
    char coloredImageName[] = "forest_color.jpg";            // Image from which color will be taken
    char coloredAsGrayscaleImageName[] = "forest_grays.jpg"; // The coloredImage changed to grayscale.

    char grayscaleImagePath[MAXLEN] = {};
    char coloredImagePath[MAXLEN] = {};
    char coloredAsGrayscaleImagePath[MAXLEN] = {};
    char outputImagePath[MAXLEN] = {};

    int c_width, c_height, c_bpp;                // For reading color image
    int c_as_g_width, c_as_g_height, c_as_g_bpp; // For reading grayscale of color iamge. Don't have a way to convert to grayscale in the code.
    int g_width, g_height, g_bpp;                // Image to be colored
    unsigned char *finalImage;                   // To be written

    generatePathNames(sizeOfAllImage, grayscaleInputName, coloredImageName, coloredAsGrayscaleImageName, grayscaleImagePath, coloredImagePath, coloredAsGrayscaleImagePath, outputImagePath);

    unsigned char *c_image = stbi_load(coloredImagePath, &c_width, &c_height, &c_bpp, imgchannels);
    unsigned char *c_as_g_image = stbi_load(coloredAsGrayscaleImagePath, &c_as_g_width, &c_as_g_height, &c_as_g_bpp, 1);
    unsigned char *g_image = stbi_load(grayscaleImagePath, &g_width, &g_height, &g_bpp, 1);
    finalImage = (unsigned char *)malloc(3 * g_width * g_height * sizeof(unsigned char));

    memset(finalImage, '\0', 3 * g_width * g_height * sizeof(unsigned char));
    copyGrayscaleToFinal(finalImage, g_image, g_width, g_height);
    
    c_source.image = c_image;
    c_as_g_source.image = c_as_g_image;
    g_source.image = g_image;

    int mean = getMean(g_source.image, g_width, g_height);
    printf("Mean: %d\n", mean);

    // Luminance remap
    c_source.lum = luminance_remap(c_source.image, g_source.image, c_width, c_height);
    // c_as_g_source.lum = luminance_remap(gsource.image, target.image);
    // g_source.lum = g_source.image;

    // get200SamplesFromCImage(c_image, c_as_g_image, c_width, c_height);

    // printf("asdf\n");

    // patchMatchEachPixelWithSamples(g_image, finalImage, maskCols, maskRows, c_width, c_height);

    // stbi_write_jpg(outputImagePath, g_height, g_width, 3, finalImage, 0);

    free(finalImage);
    return 0;
}