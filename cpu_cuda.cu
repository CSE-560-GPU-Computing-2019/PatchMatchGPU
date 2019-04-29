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
#define IMGSIZE 128
#define THRESHOLD_GPU 200
#define BLOCKSIZESQ 32

int THRESHOLD = 0;

// To access grayscale value at (i, j) or (x, y) do x + y * img_width

// To access rgb values of colored image, on the returned value index 0 is R, index 1 if G and 2 is B
__host__ __device__
unsigned char *getRGBOffset(int i, int j, unsigned char *c_image, int img_height, int img_width) {
    return c_image + (i + img_height * j) * 3;
}

// To access rgb values of colored image, on the returned value index 0 is R, index 1 if G and 2 is B
unsigned char *getIMGOffset(int i, int j, unsigned char *c_image, int img_height, int img_width) {
    return c_image + (i + img_height * j);
}


// Send offset of image to the beginning or top-left of the starting of the grid.
__host__ __device__
int compareGridsEachPixel(const unsigned char *c_as_g_image, const unsigned char *g_image, const unsigned char *c_as_g_image_BASE, const unsigned char *g_image_BASE, int gridSizeX, int gridSizeY, int c_width, int c_height, int g_width, int g_height) {
    int sum_c_as_g = 0;
    int sum_g = 0;
    int absDiff = 0;

    for (int row = -2; row < gridSizeY/2; ++row) {
        for (int col = -2; col < gridSizeX/2; ++col) {
            if (c_as_g_image + col + row * c_width < c_as_g_image_BASE)
                continue;
            if (g_image + col + row * g_width < g_image_BASE)
                continue;
            if (c_as_g_image + col + row * c_width > c_as_g_image_BASE + c_width * c_height)
                continue;
            if (g_image + col + row * g_width > g_image_BASE + g_width * g_height)
                continue;
            sum_c_as_g = c_as_g_image[col + row * c_width];
            sum_g = g_image[col + row * g_width];
            absDiff += abs(sum_c_as_g - sum_g);
        }
        // printf("Value of abs at row %d is %d\n", row, absDiff);
    }
    // printf("Value of absDiff is %d\n", absDiff);
    return absDiff;
}



/**
 * Provide correct offsets of final Image this code assumes that finalImage point to the grid where the color it to be copied to.
 * Same goes for c_image COLORS PIXEL INSTEAD OF GRID
 */
__host__ __device__
void colorImagePatchEachPixel(unsigned char *finalImage, unsigned char *c_image, int gridSizeX, int gridSizeY, int c_width, int c_height, int g_width, int g_height) {
    unsigned char *c_image_pixel;
    unsigned char *finalImage_pixel;

    // if (finalImage + 2 * dataSizeX + 2 > finalImage)
    //     return;
    // if (c_image + 2 * dataSizeX + 2 > strlen(c_image))
    //     return;

    c_image_pixel = getRGBOffset(0, 0, c_image, c_height, c_width);
    finalImage_pixel = getRGBOffset(0, 0, finalImage, g_height, g_width);
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

__global__
void gpuPathMatchEachPixel(unsigned char *c_image, const unsigned char *c_as_g_image, const unsigned char *g_image, unsigned char *finalImage, int gridSizeX, int gridSizeY, int c_width, int c_height, int g_width, int g_height)
{
    __shared__ int absDiffGrid[BLOCKSIZESQ][BLOCKSIZESQ];
    int c_as_g_index_row = 0;
    int c_as_g_index_col = 0;
    int g_index_row = 0;
    int g_index_col = 0;
    int absDiff = 0;

    g_index_row = threadIdx.y + blockIdx.y * blockDim.y;
    g_index_col = threadIdx.x + blockIdx.x * blockDim.x;

    // printf("OPA\n");

    for (int row = 0; row < c_height; ++row) { // Iterate over c_as_g_image
        // c_as_g_index_row = row * gridSizeX;
        c_as_g_index_row = row;
        for (int col = 0; col < c_width; ++col) { // Iterate over c_as_g_image
            // c_as_g_index_col = col * gridSizeY;
            c_as_g_index_col = col;

            absDiff = compareGridsEachPixel(c_as_g_image + c_as_g_index_col + (c_as_g_index_row * c_width), 
                                                    g_image + g_index_col + (g_index_row * g_width), 
                                                    c_as_g_image,
                                                    g_image,
                                                    gridSizeX, 
                                                    gridSizeY,
                                                    c_width, c_height, g_width, g_height);
            // printf("OPA\n");
            
            if (absDiff < THRESHOLD_GPU) {
                // if (finalImage[g_index_col + g_index_row * gridSizeX] == '\0') {
                // printf("g_index_row %d, g_index_col %d\n", g_index_row, g_index_col);
                if (absDiffGrid[threadIdx.y][threadIdx.x] == 0) {
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
                    colorImagePatchEachPixel(getRGBOffset(g_index_col, g_index_row, finalImage, g_width, g_height),
                                    getRGBOffset(c_as_g_index_col, c_as_g_index_row, c_image, c_width, c_height),
                                    gridSizeX,
                                    gridSizeY,
                                    c_width, c_height, g_width, g_height);
                //     // absDiffGrid[g_index_row][g_index_col] = absDiff;
                    absDiffGrid[threadIdx.y][threadIdx.x] = absDiff; // g_index_row and g_index_col because the above commented line was going out of scope because absDiff is reduced size grid (check at top)
                //     // printf("BOIBOI\n");
                } 
                else if (absDiff < absDiffGrid[threadIdx.y][threadIdx.x]){ // If new absDiff < previousAbsDiff then update
                    colorImagePatchEachPixel(getRGBOffset(g_index_col, g_index_row, finalImage, g_width, g_height),
                                    getRGBOffset(c_as_g_index_col, c_as_g_index_row, c_image, c_width, c_height),
                                    gridSizeX,
                                    gridSizeY,
                                    c_width, c_height, g_width, g_height);
                    absDiffGrid[threadIdx.y][threadIdx.x] = absDiff;
                }
            }

        }

    }
}


void generatePathNames(char *sizeOfAllImage, char *grayscaleInputName, char *coloredImageName, 
                       char *coloredAsGrayscaleImageName, char *grayscaleImagePath,
                       char *coloredImagePath, char *coloredAsGrayscaleImagePath,
                       char * outputImagePath,
                       char * outputImagePathGPU) {
    char folderName[] = "Images";
    char input_to_be_colored[] = "input_to_be_colored";
    char input_color_name[] = "input_color";
    char input_grayscale_name[] = "input_grayscale";
    char ch;
    snprintf(grayscaleImagePath, MAXLEN, "%s/%s/%s/%s", folderName, sizeOfAllImage, input_to_be_colored, grayscaleInputName);
    snprintf(coloredImagePath, MAXLEN, "%s/%s/%s/%s", folderName, sizeOfAllImage, input_color_name, coloredImageName);
    snprintf(coloredAsGrayscaleImagePath, MAXLEN, "%s/%s/%s/%s", folderName, sizeOfAllImage, input_grayscale_name, coloredAsGrayscaleImageName);
    snprintf(outputImagePath, MAXLEN, "%s/%s/%s/%s", folderName, sizeOfAllImage, "output", grayscaleInputName);
    snprintf(outputImagePathGPU, MAXLEN, "%s/%s/%s/", folderName, sizeOfAllImage, "output");
    // snprintf(outputImagePathGPU, MAXLEN, "%s/%s/", folderName, sizeOfAllImage);
    strncat(outputImagePathGPU, grayscaleInputName, strrchr(grayscaleInputName, '.') - grayscaleInputName);
    // strncat(outputImagePath, grayscaleInputName, strrchr(grayscaleInputName, '.') - grayscaleInputName);
    // strcat(outputImagePath, "_colored.jpg");
    strcat(outputImagePathGPU, "_GPU.jpg");

    // printf("Input . start at %s\n", strrchr(grayscaleInputName, '.'));


    printf("Path generated are: \n");
    printf("grayscaleImagePath: %s\n", grayscaleImagePath);
    printf("coloredImagePath: %s\n", coloredImagePath);
    printf("coloredAsGrayscaleImagePath: %s\n", coloredAsGrayscaleImagePath);
    // printf("outputImagePath: %s\n", outputImagePath);
    printf("outputImagePathGPU: %s\n", outputImagePathGPU);
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
    char grayscaleInputName[] = "1.jpg";                    // Image to be colored
    char coloredImageName[] = "1.jpg";                     // Image from which color will be taken
    char coloredAsGrayscaleImageName[] = "1.jpg";          // The coloredImage changed to grayscale.
    // char coloredAsGrayscaleImageName[] = "converted_color_";

    char grayscaleImagePath[MAXLEN] = {};
    char coloredImagePath[MAXLEN] = {};
    char coloredAsGrayscaleImagePath[MAXLEN] = {};
    char outputImagePath[MAXLEN] = {};
    char outputImagePathGPU[MAXLEN] = {};

    // Cuda variables
    cudaEvent_t start, stop;
    float elapsedTime;
    unsigned char *d_c_image;
    unsigned char *d_c_as_g_image;
    unsigned char *d_g_image;
    unsigned char *d_finalImage;
    

    int c_width, c_height, c_bpp; // For reading color image
    int c_as_g_width, c_as_g_height, c_as_g_bpp; // For reading grayscale of color iamge. Don't have a way to convert to grayscale in the code.
    int g_width, g_height, g_bpp; // Image to be colored
    unsigned char *finalImage, *c_as_g_image_load; // To be written
    unsigned char *finalImageByGPU;


    generatePathNames(sizeOfAllImage, grayscaleInputName, coloredImageName, coloredAsGrayscaleImageName, grayscaleImagePath, coloredImagePath, coloredAsGrayscaleImagePath, outputImagePath, outputImagePathGPU);

    unsigned char *c_image = stbi_load(coloredImagePath, &c_width, &c_height, &c_bpp, imgchannels );
    unsigned char *c_as_g_image = stbi_load(coloredAsGrayscaleImagePath, &c_as_g_width, &c_as_g_height, &c_as_g_bpp, 1 );
    unsigned char *g_image = stbi_load(grayscaleImagePath, &g_width, &g_height, &g_bpp, 1 );
    finalImage = (unsigned char*) malloc(3 * g_width * g_height * sizeof(unsigned char));
    finalImageByGPU = (unsigned char*) malloc(3 * g_width * g_height * sizeof(unsigned char));
    memset(finalImage, '\0', 3 * g_width * g_height * sizeof(unsigned char));
    memset(finalImageByGPU, '\0', 3 * g_width * g_height * sizeof(unsigned char));
    copyGrayscaleToFinal(finalImage, g_image, g_width, g_height);
    copyGrayscaleToFinal(finalImageByGPU, g_image, g_width, g_height);

    // Convert colored image to grayscale
    // convertToGrayscale(c_as_g_image_load, c_image, c_width, c_height);
    // stbi_write_jpg(coloredAsGrayscaleImagePath, c_height, c_width, 1, c_as_g_image_load, 0);
    // unsigned char *c_as_g_image = stbi_load(coloredAsGrayscaleImagePath, &c_as_g_width, &c_as_g_height, &c_as_g_bpp, 1 );
    // printf("asdf: %d\n", (int)c_as_g_image[0]);
    // printf("asdf\n");

    // clock_t t = clock();
    // patchMatch(c_image, c_as_g_image, g_image, finalImage, maskCols, maskRows, c_width, c_height);
    // patchMatchEachPixel(c_image, c_as_g_image, g_image, finalImage, maskCols, maskRows, c_width, c_height);

    // t = clock() - t;

    // double time_taken = ((double)t) / CLOCKS_PER_SEC;
    // printf("Time Taken: %fms\n", time_taken * 1000);


    // GPU CODE HERE
    // Mallocs
    cudaMalloc(&d_c_image, 3 * c_width * c_height * sizeof(unsigned char));
    cudaMalloc(&d_c_as_g_image, c_width * c_height * sizeof(unsigned char));
    cudaMalloc(&d_g_image, c_width * c_height * sizeof(unsigned char));
    cudaMalloc(&d_finalImage, 3 * c_width * c_height * sizeof(unsigned char));
    cudaMemset(d_finalImage, '\0', 3 * g_width * g_height * sizeof(unsigned char));

    // Memcpy
    cudaMemcpy(d_c_image, c_image, 3 * c_width * c_height * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_c_as_g_image, c_as_g_image, c_width * c_height * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_g_image, g_image, c_width * c_height * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_finalImage, finalImageByGPU, 3 *  c_width * c_height * sizeof(unsigned char), cudaMemcpyHostToDevice);



    int numOfThreadSq = BLOCKSIZESQ;
    dim3 threadsPerBlock(numOfThreadSq, numOfThreadSq);
    dim3 numOfBlocks(g_width/numOfThreadSq, g_height/numOfThreadSq);

    cudaEventCreate(&start);
    cudaEventRecord(start,0);

    gpuPathMatchEachPixel<<<numOfBlocks, threadsPerBlock>>>(d_c_image, d_c_as_g_image, d_g_image, d_finalImage, maskCols, maskRows, c_width, c_height, g_width, g_height);


    cudaEventCreate(&stop);

    cudaEventRecord(stop,0);
    cudaEventSynchronize(stop);

    cudaEventElapsedTime(&elapsedTime, start,stop);
    printf("Elapsed time : %fms\n" ,elapsedTime);
    
    cudaMemcpy(finalImageByGPU, d_finalImage, 3 *  c_width * c_height * sizeof(unsigned char), cudaMemcpyDeviceToHost);

    stbi_write_jpg(outputImagePath, g_height, g_width, 3, finalImage, 0);
    stbi_write_jpg(outputImagePathGPU, g_height, g_width, 3, finalImageByGPU, 0);

    free(finalImage);
    cudaFree(d_c_image);
    cudaFree(d_c_as_g_image);
    cudaFree(d_g_image);
    cudaFree(d_finalImage);
    return 0;
}
