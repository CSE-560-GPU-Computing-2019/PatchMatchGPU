#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <iostream>
#include "stb_image.h"
#include "stb_image_write.h"
#include <stdio.h>
#include <time.h>

#define imgchannels 3
#define maskCols 5
#define maskRows 5
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
            if (col + (row * dataSizeX) > dataSizeX * dataSizeY) 
                continue;
            sum_c_as_g = c_as_g_image[col + row * dataSizeX];
            sum_g = g_image[col + row * dataSizeX];
            absDiff += abs(sum_c_as_g - sum_g);
        }
        // printf("Value of abs at row %d is %d\n", row, absDiff);
    }

    return absDiff;
}

// Send offset of image to the beginning or top-left of the starting of the grid.
int compareGridsEachPixel(const unsigned char *c_as_g_image, const unsigned char *g_image, const unsigned char *c_as_g_image_BASE, const unsigned char *g_image_BASE, int gridSizeX, int gridSizeY, int dataSizeX, int dataSizeY) {
    int sum_c_as_g = 0;
    int sum_g = 0;
    int absDiff = 0;

    for (int row = -2; row < gridSizeY/2; ++row) {
        for (int col = -2; col < gridSizeX/2; ++col) {
            if (c_as_g_image + col + row * dataSizeX < c_as_g_image_BASE)
                continue;
            if (g_image + col + row * dataSizeX < g_image_BASE)
                continue;
            if (c_as_g_image + col + row * dataSizeX > c_as_g_image_BASE + dataSizeX * dataSizeY)
                continue;
            if (g_image + col + row * dataSizeX > g_image_BASE + dataSizeX * dataSizeY)
                continue;
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
 * Provide correct offsets of final Image this code assumes that finalImage point to the grid where the color it to be copied to.
 * Same goes for c_image COLORS PIXEL INSTEAD OF GRID
 */
void colorImagePatchEachPixel(unsigned char *finalImage, unsigned char *c_image, int gridSizeX, int gridSizeY, int dataSizeX, int dataSizeY) {
    unsigned char *c_image_pixel;
    unsigned char *finalImage_pixel;

    // if (finalImage + 2 * dataSizeX + 2 > finalImage)
    //     return;
    // if (c_image + 2 * dataSizeX + 2 > strlen(c_image))
    //     return;

    c_image_pixel = getRGBOffset(0, 0, c_image, dataSizeY, dataSizeX);
    finalImage_pixel = getRGBOffset(0, 0, finalImage, dataSizeY, dataSizeX);
    // finalImage[col + row * gridSizeX] = ;
    finalImage_pixel[0] = c_image_pixel[0]; // Copy R
    finalImage_pixel[1] = c_image_pixel[1]; // Copy G
    finalImage_pixel[2] = c_image_pixel[2]; // Copy B
    // for (int row = 0; row < gridSizeY; ++row) {
    //     for (int col = 0; col < gridSizeX; ++col) {
    //         // printf("Row in colorImagePatch is: %d, col is: %d\n", row, col);
    //         c_image_pixel = getRGBOffset(col, row, c_image, dataSizeY, dataSizeX);
    //         finalImage_pixel = getRGBOffset(col, row, finalImage, dataSizeY, dataSizeX);
    //         // finalImage[col + row * gridSizeX] = ;
    //         finalImage_pixel[0] = c_image_pixel[0]; // Copy R
    //         finalImage_pixel[1] = c_image_pixel[1]; // Copy G
    //         finalImage_pixel[2] = c_image_pixel[2]; // Copy B
    //     }
    // }
    // printf("HEREBOID\n");
}


void patchMatchEachPixel(unsigned char *c_image, const unsigned char *c_as_g_image, const unsigned char *g_image, unsigned char *finalImage, int gridSizeX, int gridSizeY, int dataSizeX, int dataSizeY)
{
    // int widthIter = dataSizeX/gridSizeX;
    // int heightIter = dataSizeY/gridSizeY;
    int absDiffGrid[dataSizeY][dataSizeX];
    int c_as_g_index_row = 0;
    int c_as_g_index_col = 0;
    int g_index_row = 0;
    int g_index_col = 0;
    int absDiff = 0;

    // printf("widthIter %d; heightIter %d\n", widthIter, heightIter);
    // getc(0);

    for (int row = 0; row < dataSizeY; ++row) { // Iterate over c_as_g_image
        // c_as_g_index_row = row * gridSizeX;
        c_as_g_index_row = row;
        for (int col = 0; col < dataSizeX; ++col) { // Iterate over c_as_g_image
            // c_as_g_index_col = col * gridSizeY;
            c_as_g_index_col = col;
            for (int row_g = 0; row_g < dataSizeY; ++row_g) { // Iterate over g_image
                // g_index_row = row_g * gridSizeX;
                g_index_row = row_g;
                for (int col_g = 0; col_g < dataSizeX; ++col_g) { // Iterate over g_image
                    // g_index_col = col_g * gridSizeY;
                    g_index_col = col_g;
                    // printf("row: %d; col: %d; row_g: %d; col_g: %d, g_index_row: %d, g_index_col: %d, c_as_g_index_row: %d, c_as_g_index_col: %d\n", row, col, row_g, col_g, g_index_row, g_index_col, c_as_g_index_row, c_as_g_index_col);

                    // Give the correct offset of c_as_g_image and g_image
                    absDiff = compareGridsEachPixel(c_as_g_image + c_as_g_index_col + (c_as_g_index_row * dataSizeX), 
                                                    g_image + g_index_col + (g_index_row * dataSizeX), 
                                                    c_as_g_image,
                                                    g_image,
                                                    gridSizeX, 
                                                    gridSizeY,
                                                    dataSizeX,
                                                    dataSizeY);
                    if (absDiff < THRESHOLD) {
                        // if (finalImage[g_index_col + g_index_row * gridSizeX] == '\0') {
                        if (absDiffGrid[row_g][col_g] == 0) {
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
                            colorImagePatchEachPixel(getRGBOffset(g_index_col, g_index_row, finalImage, dataSizeY, dataSizeX),
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
                            colorImagePatchEachPixel(getRGBOffset(g_index_col, g_index_row, finalImage, dataSizeY, dataSizeX),
                                            getRGBOffset(c_as_g_index_col, c_as_g_index_row, c_image, dataSizeY, dataSizeX),
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

    // memset(absDiffGrid, 9999, heightIter * widthIter);
    for (int i = 0; i < widthIter; ++i) {
        for (int j = 0; j < heightIter; ++j) {
            absDiffGrid[j][i] = 0;
        }
    }

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
                        // if (finalImage[g_index_col + g_index_row * gridSizeX] == '\0') {
                        if (absDiffGrid[row_g][col_g] == 0) {
                        //     // colorImagePatch(finalImage,
                        //     //                 c_image,
                        //     //                 gridSizeX, 
                        //     //                 gridSizeY,
                        //     //                 dataSizeX,
                        //     //                 dataSizeY);
                        //     // colorImagePatch(finalImage + g_index_col + (g_index_row * dataSizeX),
                        //     //                 c_image + c_as_g_index_col + (c_as_g_index_row * dataSizeX),
                        //     //                 gridSizeX,
                        //     //                 gridSizeY,
                        //     //                 dataSizeX,
                        //     //                 dataSizeY);
                            colorImagePatch(getRGBOffset(g_index_col, g_index_row, finalImage, dataSizeY, dataSizeX),
                                            getRGBOffset(c_as_g_index_col, c_as_g_index_row, c_image, dataSizeY, dataSizeX),
                                            gridSizeX,
                                            gridSizeY,
                                            dataSizeX,
                                            dataSizeY);
                        //     // absDiffGrid[g_index_row][g_index_col] = absDiff;
                            absDiffGrid[row_g][col_g] = absDiff; // row_g and col_g because the above commented line was going out of scope because absDiff is reduced size grid (check at top)
                        //     // printf("BOIBOI\n");
                        // } 
                        // else if (absDiff < absDiffGrid[row_g][col_g]){ // If new absDiff < previousAbsDiff then update
                        //     colorImagePatch(finalImage + g_index_col + (g_index_row * dataSizeX),
                        //                     c_image + c_as_g_index_col + (c_as_g_index_row * dataSizeX),
                        //                     gridSizeX,
                        //                     gridSizeY,
                        //                     dataSizeX,
                        //                     dataSizeY);
                        //     absDiffGrid[row_g][col_g] = absDiff;
                        }
                        else if (absDiff < absDiffGrid[row_g][col_g]){ // If new absDiff < previousAbsDiff then update
                        // // if (finalImage[g_index_col + g_index_row * gridSizeX] == '\0'){
                            colorImagePatch(getRGBOffset(g_index_col, g_index_row, finalImage, dataSizeY, dataSizeX),
                                            getRGBOffset(c_as_g_index_col, c_as_g_index_row, c_image, dataSizeY, dataSizeX),
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
    // printf("Press any key to continue\n");
    // scanf("%c",&ch);
}

void copyGrayscaleToFinal(unsigned char * finalImage, const unsigned char *g_image, int dataSizeX, int dataSizeY) {
    for (int i = 0; i < dataSizeX; ++i) {
        for (int j = 0; j < dataSizeY; ++j) {
            // if (finalImage[i + j * dataSizeX] != '\0')
                // continue;
            finalImage[(i + dataSizeX * j) * 3] = g_image[i + j * dataSizeX];
            finalImage[1 + (i + dataSizeX * j) * 3] = g_image[i + j * dataSizeX];
            finalImage[2 + (i + dataSizeX * j) * 3] = g_image[i + j * dataSizeX];
        }
    }
}

void convertToGrayscale(unsigned char *finalImage, const unsigned char *g_image, int dataSizeX, int dataSizeY)
{
    for (int i = 0; i < dataSizeX; ++i)
    {
        for (int j = 0; j < dataSizeY; ++j)
        {
            // printf("%d, %d, %d\n", g_image[(i + dataSizeX * j) * 3], g_image[1 + (i + dataSizeX * j) * 3], g_image[2 + (i + dataSizeX * j) * 3]);
            int avg = (g_image[(i + dataSizeX * j) * 3] + g_image[1 + (i + dataSizeX * j) * 3] + g_image[2 + (i + dataSizeX * j) * 3]) / 3;
            // printf("Avg: %d\n", avg);
            finalImage[i + dataSizeX * j] = avg;
            finalImage[1 + i + dataSizeX * j] = avg;
            finalImage[2 + i + dataSizeX * j] = avg;
        }
    }
}

// g -> Grayscale
// c -> Color
int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Usage: ./cpu <THRESHOLD>");
        return 1;
    }
    THRESHOLD = atoi(argv[1]);
    char sizeOfAllImage[] = "128"; // Must be a square image and all must be of the same size
    char grayscaleInputName[] = "test/gray.jpg";                    // Image to be colored
    char coloredImageName[] = "test/color.jpg";                     // Image from which color will be taken
    char coloredAsGrayscaleImageName[] = "test/color_grays.jpg";          // The coloredImage changed to grayscale.
    // char coloredAsGrayscaleImageName[] = "converted_color_";

    char grayscaleImagePath[MAXLEN] = {};
    char coloredImagePath[MAXLEN] = {};
    char coloredAsGrayscaleImagePath[MAXLEN] = {};
    char outputImagePath[MAXLEN] = {};


    int c_width, c_height, c_bpp; // For reading color image
    int c_as_g_width, c_as_g_height, c_as_g_bpp; // For reading grayscale of color iamge. Don't have a way to convert to grayscale in the code.
    int g_width, g_height, g_bpp; // Image to be colored
    unsigned char *finalImage, *c_as_g_image_load; // To be written

    generatePathNames(sizeOfAllImage, grayscaleInputName, coloredImageName, coloredAsGrayscaleImageName, grayscaleImagePath, coloredImagePath, coloredAsGrayscaleImagePath, outputImagePath);

    unsigned char *c_image = stbi_load(coloredImagePath, &c_width, &c_height, &c_bpp, imgchannels );
    unsigned char *c_as_g_image = stbi_load(coloredAsGrayscaleImagePath, &c_as_g_width, &c_as_g_height, &c_as_g_bpp, 1 );
    unsigned char *g_image = stbi_load(grayscaleImagePath, &g_width, &g_height, &g_bpp, 1 );
    finalImage = (unsigned char*) malloc(3 * g_width * g_height * sizeof(unsigned char));
    memset(finalImage, '\0', 3 * g_width * g_height * sizeof(unsigned char));
    copyGrayscaleToFinal(finalImage, g_image, g_width, g_height);

    // Convert colored image to grayscale
    // convertToGrayscale(c_as_g_image_load, c_image, c_width, c_height);
    // stbi_write_jpg(coloredAsGrayscaleImagePath, c_height, c_width, 1, c_as_g_image_load, 0);
    // unsigned char *c_as_g_image = stbi_load(coloredAsGrayscaleImagePath, &c_as_g_width, &c_as_g_height, &c_as_g_bpp, 1 );
    // printf("asdf: %d\n", (int)c_as_g_image[0]);
    // printf("asdf\n");

    clock_t t = clock();
    // patchMatch(c_image, c_as_g_image, g_image, finalImage, maskCols, maskRows, c_width, c_height);
    patchMatchEachPixel(c_image, c_as_g_image, g_image, finalImage, maskCols, maskRows, c_width, c_height);

    t = clock() - t;

    double time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Time Taken: %f\n", time_taken);


    stbi_write_jpg(outputImagePath, g_height, g_width, 3, finalImage, 0);

    free(finalImage);
    return 0;
}
