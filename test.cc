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

struct PixelColorLumi{
    int col;
    int row;
    int lum;
    int r;
    int g;
    int b;
};

struct MeanStd{
    float mean;
    float std;
};


int THRESHOLD = 0;
struct PixelColorLumi samples[NUMSAMPLE];
struct MeanStd meanStdSample;

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
float compareGridsEachPixel(const unsigned char *c_as_g_image, const unsigned char *g_image, const unsigned char *c_as_g_image_BASE, const unsigned char *g_image_BASE, int gridSizeX, int gridSizeY, int dataSizeX, int dataSizeY) {
    int sum_g = 0;
    int sum_c_as_g = 0;
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
    }

    return absDiff;
}

// Send offset of image to the beginning or top-left of the starting of the grid.
void compareGridsEachPixelWithSamples(const unsigned char *g_image, const unsigned char *g_image_BASE, int gridSizeX, int gridSizeY, int dataSizeX, int dataSizeY) {
    // For mean, variance and stdDev
    int sumG = 0;
    int meanG = 0;
    int var_g = 0;
    float std_g = 0; 
    int g_Matrix[gridSizeY][gridSizeX];

    for (int i = 0; i < gridSizeY; ++i) {
     for (int j = 0; j < gridSizeX; ++j) {
         g_Matrix[i][j] = 0;
     }
    }

    // Calculate mean
    for (int row = -3; row < gridSizeY/2; ++row) {
        for (int col = -3; col < gridSizeX/2; ++col) {
            if (g_image + col + row * dataSizeX < g_image_BASE)
                continue;
            if (g_image + col + row * dataSizeX > g_image_BASE + dataSizeX * dataSizeY)
                continue;


            g_Matrix[row][col] = g_image[col + row * dataSizeX];
        }
    }


    for (int i = 0; i < maskRows; ++i) {
     for (int j = 0; j < maskCols; ++j) {
            sumG += g_Matrix[i][j];          
     }
    }

    meanG = sumG / (gridSizeX * gridSizeY);

    for (int i = 0; i < maskRows; ++i) {
     for (int j = 0; j < maskCols; ++j) {
            g_Matrix[i][j] -= sumG;

            g_Matrix[i][j] *= g_Matrix[i][j];
     }
    }

    for (int i = 0; i < maskRows; ++i) {
     for (int j = 0; j < maskCols; ++j) {
         var_g += g_Matrix[i][j];
     }
    }

    var_g = var_g/(gridSizeX * gridSizeY);

    std_g = sqrt(var_g);
    meanStdSample.mean = meanG;
    meanStdSample.std = std_g;
    // return std_g;
    // printf("stdDev: %f\n", std_g);
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


void colorImagePixelWithSample(unsigned char *finalImage, int bestIndex) {
    finalImage[0] = samples[bestIndex].r; // Copy R
    finalImage[1] = samples[bestIndex].g; // Copy G
    finalImage[2] = samples[bestIndex].b; //
}

void patchMatchEachPixelWithSamples(const unsigned char *g_image, unsigned char *finalImage, int gridSizeX, int gridSizeY, int dataSizeX, int dataSizeY)
{
    // int widthIter = dataSizeX/gridSizeX;
    // int heightIter = dataSizeY/gridSizeY;
    int c_as_g_index_row = 0;
    int c_as_g_index_col = 0;
    int g_index_row = 0;
    int g_index_col = 0;
    int bestIndex = 0;

    // printf("widthIter %d; heightIter %d\n", widthIter, heightIter);
    // getc(0);

    // for (int i = 0; i < 200; ++i) {
        for (int row_g = 0; row_g < dataSizeY; ++row_g) { // Iterate over g_image
            // g_index_row = row_g * gridSizeX;
            g_index_row = row_g;
            for (int col_g = 0; col_g < dataSizeX; ++col_g) { // Iterate over g_image
                // g_index_col = col_g * gridSizeY;
                g_index_col = col_g;
                // printf("row: %d; col: %d; row_g: %d; col_g: %d, g_index_row: %d, g_index_col: %d, c_as_g_index_row: %d, c_as_g_index_col: %d\n", row, col, row_g, col_g, g_index_row, g_index_col, c_as_g_index_row, c_as_g_index_col);

                // Give the correct offset of c_as_g_image and g_image
                compareGridsEachPixelWithSamples(g_image + g_index_col + (g_index_row * dataSizeX), 
                                      g_image,
                                      gridSizeX, 
                                      gridSizeY,
                                      dataSizeX,
                                      dataSizeY);

                for (int i = 1; i < NUMSAMPLE; ++i) {
                    if ( samples[i].lum < meanStdSample.mean + meanStdSample.std && samples[i].lum > meanStdSample.mean - meanStdSample.std ) {
                        if (abs(samples[i].lum - meanStdSample.mean) < abs(samples[bestIndex].lum - meanStdSample.mean)) {
                            bestIndex = i;
                        }
                    }

                }
                // printf("bestIndex %d\n", bestIndex);

                colorImagePixelWithSample(getRGBOffset(g_index_col, g_index_row, finalImage, dataSizeY, dataSizeX), bestIndex);
                /*if (absDiff < THRESHOLD) {
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
                }*/
            }
        }
    // }
    
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
    strcat(outputImagePath, "_colored_test.jpg");

    // printf("Input . start at %s\n", strrchr(grayscaleInputName, '.'));


    printf("Path generated are: \n");
    printf("grayscaleImagePath: %s\n", grayscaleImagePath);
    printf("coloredImagePath: %s\n", coloredImagePath);
    printf("coloredAsGrayscaleImagePath: %s\n", coloredAsGrayscaleImagePath);
    printf("outputImagePath: %s\n", outputImagePath);
    printf("Press any key to continue\n");
    scanf("%c",&ch);
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

void get200SamplesFromCImage(unsigned char *c_image, unsigned char *c_as_g_image, int width, int height) {
    int randRow = 0;
    int randCol = 0;
    unsigned char *c_image_temp;

    for (int i = 0; i < NUMSAMPLE; ++i) {
        randRow = rand() % (height - 0) + 0;
        randCol = rand() % (width - 0) + 0;
        samples[i].row = randRow;
        samples[i].col = randCol;
        samples[i].lum = c_as_g_image[randCol + randRow * width];
        c_image_temp = getRGBOffset(randCol, randCol, c_image, height, width);
        samples[i].r = c_image_temp[0];
        samples[i].g = c_image_temp[1];
        samples[i].b = c_image_temp[2];
        // printf("R: %d G: %d B: %d\n", c_image_temp[0], c_image_temp[1], c_image_temp[2]);
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
    char sizeOfAllImage[] = "256"; // Must be a square image and all must be of the same size
    char grayscaleInputName[] = "forest_grays.jpg"; // Image to be colored
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
    copyGrayscaleToFinal(finalImage, g_image, g_width, g_height);

    get200SamplesFromCImage(c_image, c_as_g_image, c_width, c_height);

    printf("asdf\n");

    // patchMatchEachPixel(c_image, c_as_g_image, g_image, finalImage, maskCols, maskRows, c_width, c_height);
    patchMatchEachPixelWithSamples(g_image, finalImage, maskCols, maskRows, c_width, c_height);


    stbi_write_jpg(outputImagePath, g_height, g_width, 3, finalImage, 0);

    free(finalImage);
    return 0;
}
