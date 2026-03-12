#include "phatom_utils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

void print_phantom(Phantom* phantom){
    for (int i = 0 ; i < phantom->getH(); i++){
        for (int j = 0 ; j < phantom->getW(); j++){
            printf("%2.2f ", phantom->get_phantom_data()[i][j]);
        }
        printf("\n");
    }
}

void save_data(Phantom* phantom){
    char filename[64];
    sprintf(filename, "phantoms/data/data_%d.txt", time(0));

    FILE* f = fopen(filename, "w");
    if (!f) return;

    int h = phantom->getH();
    int w = phantom->getW();
    double** data = phantom->get_phantom_data();

    fprintf(f, "%d %d\n", w, h);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            fprintf(f, "%0.1f ", data[i][j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}

void save_image(Phantom* phantom){
    char filename[64];
    sprintf(filename, "phantoms/pics/pic_%d.bmp", time(0));

    FILE* f = fopen(filename, "wb");
    if (!f) return;

    int w = phantom->getW();
    int h = phantom->getH();
    double** data = phantom->get_phantom_data();

    // BMP заголовки
    unsigned char fileHeader[14] = {
        'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0
    };
    unsigned char infoHeader[40] = {
        40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    int rowSize = (w * 3 + 3) & ~3;
    int imageSize = rowSize * h;

    fileHeader[2] = (unsigned char)(54 + imageSize);
    fileHeader[3] = (unsigned char)((54 + imageSize) >> 8);
    fileHeader[4] = (unsigned char)((54 + imageSize) >> 16);
    fileHeader[5] = (unsigned char)((54 + imageSize) >> 24);

    infoHeader[4] = (unsigned char)w;
    infoHeader[5] = (unsigned char)(w >> 8);
    infoHeader[6] = (unsigned char)(w >> 16);
    infoHeader[7] = (unsigned char)(w >> 24);
    infoHeader[8] = (unsigned char)h;
    infoHeader[9] = (unsigned char)(h >> 8);
    infoHeader[10] = (unsigned char)(h >> 16);
    infoHeader[11] = (unsigned char)(h >> 24);
    infoHeader[20] = (unsigned char)imageSize;
    infoHeader[21] = (unsigned char)(imageSize >> 8);
    infoHeader[22] = (unsigned char)(imageSize >> 16);
    infoHeader[23] = (unsigned char)(imageSize >> 24);

    fwrite(fileHeader, 1, 14, f);
    fwrite(infoHeader, 1, 40, f);

    unsigned char* row = new unsigned char[rowSize];
    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            BYTE val = (BYTE)(data[y][x] * 255);
            row[x * 3 + 0] = val;
            row[x * 3 + 1] = val;
            row[x * 3 + 2] = val;
        }
        fwrite(row, 1, rowSize, f);
    }
    delete[] row;

    fclose(f);
}