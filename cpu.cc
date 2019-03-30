#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <time.h>

#define imgchannels 3
#define maskCols 2
#define maskRows 2
// #define THRESHOLD 100 // In percentage
#define MAXLEN 1024 // Max length of image paths

int THRESHOLD = 0;

// To access grayscale value at (i, j) or (x, y) do x + y * img_width

// To access rgb values of colored image, on the returned value index 0 is R, index 1 if G and 2 is B
unsigned char *getRGBOffset(int i, int j, unsigned char *c_image, int img_height, int img_width) {
    return c_image + (i + img_height * j) * 3;
}

// To access rgb values of colored image, on the returned value index 0 is R, index 1 if G and 2 is B
unsigned char *getIMGOffset(int i, int j, unsigned char *c_image, int img_height, int img_width) {
    return c_image + (i + img_height * j);
}

// Send offset of image to the beginning or top-left of the starting of the grid.
int compareGrids(const unsigned char *c_as_g_image, const unsigned char *g_image, int gridSizeX, int gridSizeY, int dataSizeX, int dataSizeY) {
    int sum_c_as_g = 0;
    int sum_g = 0;
    int absDiff = 0;

    for (int row = 0; row < gridSizeY; ++row) {
        for (int col = 0; col < gridSizeX; ++col) {
            sum_c_as_g = c_as_g_image[col + row * dataSizeX];
            sum_g = g_image[col + row * dataSizeX];
            absDiff += abs(sum_c_as_g - sum_g);
        }
        // printf("Value of abs at row %d is %d\n", row, absDiff);
    }

    return absDiff;
}

/**
 * Provide correct offsets of final Image this code assumes that finalImage point to the grid where the color it to be copied to.
 * Same goes for c_image
 */
void colorImagePatch(unsigned char *finalImage, unsigned char *c_image, int gridSizeX, int gridSizeY, int dataSizeX, int dataSizeY) {
    unsigned char *c_image_pixel;
    unsigned char *finalImage_pixel;
    for (int row = 0; row < gridSizeY; ++row) {
        for (int col = 0; col < gridSizeX; ++col) {
            // printf("Row in colorImagePatch is: %d, col is: %d\n", row, col);
            c_image_pixel = getRGBOffset(col, row, c_image, dataSizeY, dataSizeX);
            finalImage_pixel = getRGBOffset(col, row, finalImage, dataSizeY, dataSizeX);
            // finalImage[col + row * gridSizeX] = ;
            finalImage_pixel[0] = c_image_pixel[0]; // Copy R
            finalImage_pixel[1] = c_image_pixel[1]; // Copy G
            finalImage_pixel[2] = c_image_pixel[2]; // Copy B
        }
    }
    // printf("HEREBOID\n");
}

/**
 * Devide the whole image into grids of maskCols by maskRows size. So that the resulting image will have to iterated
 * imageWidth/maskCols * imageHeight/maskRows times. Compare the two grids using sum of the absolute differences of 
 * each value in the grid. If this is less than the threshold defined at the top then copy over the color, and store
 * the sum of absolute difference for grid (i,j). Repeat the above as follows
 * For each grid in c_as_g_image
 *     For each grid in grayscale image do:
 *         Match grid
 *         If Sum of absolute diff < Threshold
 *             If not already colored
 *                 Copy color
 *                 Remember rum of absolute diff for this grid
 *             If colored and new threshold is < old
 *                 Color again with new color and remember threshold
 */
void patchMatch(unsigned char *c_image, const unsigned char *c_as_g_image, const unsigned char *g_image, unsigned char *finalImage, int gridSizeX, int gridSizeY, int dataSizeX, int dataSizeY)
{
    int widthIter = dataSizeX/gridSizeX;
    int heightIter = dataSizeY/gridSizeY;
    int absDiffGrid[heightIter][widthIter];
    int c_as_g_index_row = 0;
    int c_as_g_index_col = 0;
    int g_index_row = 0;
    int g_index_col = 0;
    int absDiff = 0;

    printf("widthIter %d; heightIter %d\n", widthIter, heightIter);
    // getc(0);

    for (int row = 0; row < widthIter; ++row) { // Iterate over c_as_g_image
        c_as_g_index_row = row * gridSizeX;
        for (int col = 0; col < heightIter; ++col) { // Iterate over c_as_g_image
            c_as_g_index_col = col * gridSizeY;
            for (int row_g = 0; row_g < widthIter; ++row_g) { // Iterate over g_image
                g_index_row = row_g * gridSizeX;
                for (int col_g = 0; col_g < heightIter; ++col_g) { // Iterate over g_image
                    g_index_col = col_g * gridSizeY;
                    // printf("row: %d; col: %d; row_g: %d; col_g: %d, g_index_row: %d, g_index_col: %d, c_as_g_index_row: %d, c_as_g_index_col: %d\n", row, col, row_g, col_g, g_index_row, g_index_col, c_as_g_index_row, c_as_g_index_col);

                    // Give the correct offset of c_as_g_image and g_image
                    absDiff = compareGrids(c_as_g_image + c_as_g_index_col + (c_as_g_index_row * dataSizeX), 
                                           g_image + g_index_col + (g_index_row * dataSizeX), 
                                           gridSizeX, 
                                           gridSizeY,
                                           dataSizeX,
                                           dataSizeY);
                    if (absDiff < THRESHOLD) {
                        if (finalImage[g_index_col + g_index_row * gridSizeX] == '\0') {
                            // colorImagePatch(finalImage,
                            //                 c_image,
                            //                 gridSizeX, 
                            //                 gridSizeY,
                            //                 dataSizeX,
                            //                 dataSizeY);
                            // colorImagePatch(finalImage + g_index_col + (g_index_row * dataSizeX),
                            //                 c_image + c_as_g_index_col + (c_as_g_index_row * dataSizeX),
                            //                 gridSizeX,
                            //                 gridSizeY,
                            //                 dataSizeX,
                            //                 dataSizeY);
                            colorImagePatch(getRGBOffset(g_index_col, g_index_row, finalImage, dataSizeY, dataSizeX),
                                            getRGBOffset(c_as_g_index_col, c_as_g_index_row, c_image, dataSizeY, dataSizeX),
                                            gridSizeX,
                                            gridSizeY,
                                            dataSizeX,
                                            dataSizeY);
                            // absDiffGrid[g_index_row][g_index_col] = absDiff;
                            absDiffGrid[row_g][col_g] = absDiff; // row_g and col_g because the above commented line was going out of scope because absDiff is reduced size grid (check at top)
                            // printf("BOIBOI\n");
                        } 
                        else if (absDiff < absDiffGrid[row_g][col_g]){ // If new absDiff < previousAbsDiff then update
                            colorImagePatch(finalImage + g_index_col + (g_index_row * dataSizeX),
                                            c_image + c_as_g_index_col + (c_as_g_index_row * dataSizeX),
                                            gridSizeX,
                                            gridSizeY,
                                            dataSizeX,
                                            dataSizeY);
                            absDiffGrid[row_g][col_g] = absDiff;
                        }
                    }
                }
            }
        }
    }
    
    // printf("\n");
}

void generatePathNames(char *sizeOfAllImage, char *grayscaleInputName, char *coloredImageName, 
                       char *coloredAsGrayscaleImageName, char *grayscaleImagePath,
                       char *coloredImagePath, char *coloredAsGrayscaleImagePath,
                       char * outputImagePath) {
    char folderName[] = "Images";
    char ch;
    snprintf(grayscaleImagePath, MAXLEN, "%s/%s/%s", folderName, sizeOfAllImage, grayscaleInputName);
    snprintf(coloredImagePath, MAXLEN, "%s/%s/%s", folderName, sizeOfAllImage, coloredImageName);
    snprintf(coloredAsGrayscaleImagePath, MAXLEN, "%s/%s/%s", folderName, sizeOfAllImage, coloredAsGrayscaleImageName);
    snprintf(outputImagePath, MAXLEN, "%s/%s/", folderName, sizeOfAllImage);
    strncat(outputImagePath, grayscaleInputName, strrchr(grayscaleInputName, '.') - grayscaleInputName);
    strcat(outputImagePath, "_colored.jpg");

    // printf("Input . start at %s\n", strrchr(grayscaleInputName, '.'));


    printf("Path generated are: \n");
    printf("grayscaleImagePath: %s\n", grayscaleImagePath);
    printf("coloredImagePath: %s\n", coloredImagePath);
    printf("coloredAsGrayscaleImagePath: %s\n", coloredAsGrayscaleImagePath);
    printf("outputImagePath: %s\n", outputImagePath);
    printf("Press any key to continue\n");
    scanf("%c",&ch);
}

// g -> Grayscale
// c -> Color
int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Usage: ./cpu <THRESHOLD>");
        return 1;
    }
    THRESHOLD = atoi(argv[1]);
    char sizeOfAllImage[] = "256"; // Must be a square image and all must be of the same size
    char grayscaleInputName[] = "Grayscale.jpg"; // Image to be colored
    char coloredImageName[] = "forest_color.jpg"; // Image from which color will be taken
    char coloredAsGrayscaleImageName[] = "forest_grays.jpg"; // The coloredImage changed to grayscale.

    char grayscaleImagePath[MAXLEN] = {};
    char coloredImagePath[MAXLEN] = {};
    char coloredAsGrayscaleImagePath[MAXLEN] = {};
    char outputImagePath[MAXLEN] = {};

    int c_width, c_height, c_bpp; // For reading color image
    int c_as_g_width, c_as_g_height, c_as_g_bpp; // For reading grayscale of color iamge. Don't have a way to convert to grayscale in the code.
    int g_width, g_height, g_bpp; // Image to be colored
    unsigned char *finalImage; // To be written
    // unsigned char *img, *seq_img, *seq_img_device_in_host;
    // unsigned char *g_image_cpy;

    generatePathNames(sizeOfAllImage, grayscaleInputName, coloredImageName, coloredAsGrayscaleImageName, grayscaleImagePath, coloredImagePath, coloredAsGrayscaleImagePath, outputImagePath);

    unsigned char *c_image = stbi_load(coloredImagePath, &c_width, &c_height, &c_bpp, imgchannels );
    unsigned char *c_as_g_image = stbi_load(coloredAsGrayscaleImagePath, &c_as_g_width, &c_as_g_height, &c_as_g_bpp, 1 );
    unsigned char *g_image = stbi_load(grayscaleImagePath, &g_width, &g_height, &g_bpp, 1 );
    finalImage = (unsigned char*) malloc(3 * g_width * g_height * sizeof(unsigned char));
    memset(finalImage, '\0', 3 * g_width * g_height * sizeof(unsigned char));

    printf("asdf\n");

    patchMatch(c_image, c_as_g_image, g_image, finalImage, maskCols, maskRows, c_width, c_height);
        /** Testing individual functions
     *  Color Image Patch function is working.
     *  CompareGrid Tested
     *  getRGBOffset Tested
     *  getIMGOffset Tested
     */
        // colorImagePatch(finalImage, c_image, maskCols, maskRows, c_width, c_height); // Color at index 0, 0

        // colorImagePatch(getRGBOffset(200, 100, finalImage, c_height, c_width),
        //                 getRGBOffset(200, 100, c_image, c_height, c_width),
        //                 maskCols, maskRows, c_width, c_height); // Color at index 200, 100

        // colorImagePatch(getRGBOffset(400, 300, finalImage, c_height, c_width),
        //                 getRGBOffset(400, 300, c_image, c_height, c_width),
        //                 maskCols, maskRows, c_width, c_height); // Color at index 200, 100

        // printf("3x3 grid for c_as_g_image:\n");
        // printf("%d, %d, %d\n", c_as_g_image[0], c_as_g_image[1], c_as_g_image[2]);
        // printf("%d, %d, %d\n", c_as_g_image[0 + c_as_g_width * 1], c_as_g_image[1 + c_as_g_width * 1], c_as_g_image[2 + c_as_g_width * 1]);
        // printf("%d, %d, %d\n", c_as_g_image[0 + c_as_g_width * 2], c_as_g_image[1 + c_as_g_width * 2], c_as_g_image[2 + c_as_g_width * 2]);

        // printf("3x3 grid for g_image:\n");
        // printf("%d, %d, %d\n", g_image[0], g_image[1], g_image[2]);
        // printf("%d, %d, %d\n", g_image[0 + c_as_g_width * 1], g_image[1 + c_as_g_width * 1], g_image[2 + c_as_g_width * 1]);
        // printf("%d, %d, %d\n", g_image[0 + c_as_g_width * 2], g_image[1 + c_as_g_width * 2], g_image[2 + c_as_g_width * 2]);

        // printf("Grid abs diff sum at %d, %d = %d\n", 0, 0, compareGrids(c_as_g_image, g_image, maskCols, maskRows, g_width, g_height));

        // printf("3x3 grid for c_as_g_image:\n");
        // printf("%d, %d, %d\n", c_as_g_image[20 + 30 * c_as_g_width ], c_as_g_image[21 + 30 * c_as_g_width], c_as_g_image[22 + 30 * c_as_g_width]);
        // printf("%d, %d, %d\n", c_as_g_image[20 + 31 * c_as_g_width ], c_as_g_image[21 + 31 * c_as_g_width], c_as_g_image[22 + 31 * c_as_g_width]);
        // printf("%d, %d, %d\n", c_as_g_image[20 + 32 * c_as_g_width ], c_as_g_image[21 + 32 * c_as_g_width], c_as_g_image[22 + 32 * c_as_g_width]);

        // printf("3x3 grid for g_image:\n");
        // printf("%d, %d, %d\n", g_image[20 + 30 * c_as_g_width ], g_image[21 + 30 * c_as_g_width], g_image[22 + 30 * c_as_g_width]);
        // printf("%d, %d, %d\n", g_image[20 + 31 * c_as_g_width ], g_image[21 + 31 * c_as_g_width], g_image[22 + 31 * c_as_g_width]);
        // printf("%d, %d, %d\n", g_image[20 + 32 * c_as_g_width ], g_image[21 + 32 * c_as_g_width], g_image[22 + 32 * c_as_g_width]);
        // printf("Grid abs diff sum at %d, %d = %d\n", 20, 30, compareGrids(getIMGOffset(20, 30, c_as_g_image, c_height, c_width),
        //                                                                   getIMGOffset(20, 30, g_image, c_height, c_width),
        //                                                                   maskCols, maskRows, g_width, g_height));

        // i, j is same as x, y : x goes towards the right in the image, y goes below (0, 0) is at the top left in the image.
        // int i = 0;
        // int j = 0;
        // unsigned bytePerPixel = imgchannels;

        // unsigned char *pixelOffset = c_image + (i + c_height * j) * imgchannels;
        // unsigned char r = pixelOffset[0];
        // unsigned char g = pixelOffset[1];
        // unsigned char b = pixelOffset[2];
        // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
        // unsigned char *offset = getRGBOffset(0, 0, c_image, c_height, c_width);
        // printf("rgb(%d, %d) = (%d, %d, %d)\n", 0, 0, offset[0], offset[1], offset[2]);

        // printf("rgb(%d, %d) = (%d)\n", 0, 0, c_as_g_image[0]);

        // offset = getRGBOffset(20, 10, c_image, c_height, c_width);
        // printf("rgb(%d, %d) = (%d, %d, %d)\n", 20, 10, offset[0], offset[1], offset[2]);

        // printf("rgb(%d, %d) = (%d)\n", 20, 10, c_as_g_image[126 + 35 * c_as_g_width]);

        // i = 20;
        // j = 10;
        // bytePerPixel = imgchannels;
        // pixelOffset = c_image + (i + c_height * j) * imgchannels;
        // r = pixelOffset[0];
        // g = pixelOffset[1];
        // b = pixelOffset[2];
        // // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
        // printf("rgb(%d, %d) = (%d, %d, %d)\n", i, j, r, g, b);

        // i = 499;
        // j = 499;
        // bytePerPixel = imgchannels;
        // pixelOffset = c_image + (i + c_height * j) * imgchannels;
        // r = pixelOffset[0];
        // g = pixelOffset[1];
        // b = pixelOffset[2];
        // // std::cout << "RGB       : " << (int)r << ":" << (int)g << ":" << (int)b << std::endl;
        // printf("rgb(%d, %d) = (%d, %d, %d)\n", i, j, r, g, b);

        // printf("Image[0] = %d\n", g_image[0]);
        // printf("Image[1] = %d\n", g_image[1]);
        // printf("Image[2] = %d\n", g_image[2]);
        // printf("Image[3] = %d\n", g_image[3]);
        // printf("Image[4] = %d\n", g_image[4]);
        // printf("Image[4] = %d\n", g_image[5]);
        // printf("Image[4] = %d\n", g_image[6]);
        // printf("Image[4] = %d\n", g_image[7]);
        // printf("Image[4] = %d\n", g_image[8]);
        // printf("Image[4] = %d\n", g_image[9]);

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

        // g_image_cpy = (unsigned char*) malloc(g_width * g_height * sizeof(unsigned char));

        // std::cout << "Grayscale : " << g_height << " " << g_width << std::endl;
        // std::cout << "Color     : " << c_height << " " << c_width << std::endl;

        // for(int i=0; i< g_height * g_width * imgchannels; i++){
        //     g_image_cpy[i] = g_image[i];
        // }

        stbi_write_jpg(outputImagePath, g_height, g_width, 3, finalImage, 0);

    free(finalImage);
    return 0;
}
