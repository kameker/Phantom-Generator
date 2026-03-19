#include "phatom.hpp"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>


int offsetX(int w, int r){
    return rand() % std::max(3, 2 * (w / 2 - r)) - (w / 2 - r);
}

int offsetY(int h, int r){
    return rand() % std::max(3, 2 * (h / 2 - r)) - (h / 2 - r);
}

Phantom::Phantom(int w, int h, double delta,
    double sigma, int minR, int maxR, int count){
    this->WIDTH = w;
    this->HEIGHT = h;
    this->delta = delta;
    this->smothingSigma = sigma;
    this->minR = minR;
    this->maxR = maxR;
    this->circles_count = count;
    this->phantom_data = new double*[h];
    for (int i = 0; i < h; i++) {
        this->phantom_data[i] = new double[w];
    }
    this->centerX = WIDTH / 2;
    this->centerY = HEIGHT / 2;
}
Phantom::~Phantom(){
    for (int i = 0; i < this->HEIGHT; i++) {
        delete[] this->phantom_data[i];
    }
    delete[] this->phantom_data;
}
void Phantom::generate_phantom(){
    for (int i = 0; i < this->HEIGHT; i++) {
        for (int j = 0; j < this->WIDTH; j++) {
            this->phantom_data[i][j] = 0.0;
        }
    }
    
    int X, Y, radius;
    for (int i = 0; i < this->circles_count; i++){
        radius = rand_radius();
        X = this->centerX + offsetX(this->WIDTH, radius);
        Y = this->centerY + offsetY(this->HEIGHT, radius);
        fillCircle(X, Y, radius);
    }
    if (this->smothingSigma > 0){
        setGausBlur(this->smothingSigma);
    }
}

int Phantom::rand_radius(){
    return (abs(rand()) + this->minR) % this->maxR;
}

void Phantom::fillCircle(int centerX, int centerY, int radius){
    int rs = radius * radius;
    for (int y = centerY - radius; y <= centerY + radius; y++) {
        for (int x = centerX - radius; x <= centerX + radius; x++) {
            if (y >= 0 && y < this->HEIGHT && x >= 0 && x < this->WIDTH) {
                int dx = x - centerX;
                int dy = y - centerY;
                if (dx * dx + dy * dy <= rs) {
                    this->phantom_data[y][x] = std::min((this->phantom_data[y][x] + delta),(double) 1);
                }
            }
        }
    }
}

void Phantom::reSetData(int w, int h, double delta,
        double sigma, int minR, int maxR, int count){
    this->WIDTH = w;
    this->HEIGHT = h;
    this->delta = delta;
    this->smothingSigma = sigma;
    this->minR = minR;
    this->maxR = maxR;
    this->circles_count = count;
    this->phantom_data = new double*[h];
    for (int i = 0; i < h; i++) {
        this->phantom_data[i] = new double[w];
    }
    this->centerX = WIDTH / 2;
    this->centerY = HEIGHT / 2;
}

void Phantom::setGausBlur(double sigma){
    int kernelSize = (int) (6 * sigma + 1);
    if (kernelSize % 2 == 0) kernelSize++;
    int radius = kernelSize / 2;
    double** kernel = new double*[kernelSize];
    for (int i = 0; i < kernelSize; i++) {
        kernel[i] = new double[kernelSize];
    }
    double sum = 0.0;
        
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            double value = exp(-(x * x + y * y) / (2 * sigma * sigma));
            kernel[y + radius][x + radius] = value;
            sum += value;
        }
    }
        
    for (int y = 0; y < kernelSize; y++) {
        for (int x = 0; x < kernelSize; x++) {
            kernel[y][x] /= sum;
        }
    }

    double** result = new double*[this->HEIGHT];
    for (int i = 0; i < this->HEIGHT; i++) {
        result[i] = new double[this->WIDTH];
    }
    for (int y = 0; y < this->HEIGHT; y++) {
        for (int x = 0; x < this->WIDTH; x++) {
            double blurredValue = 0.0;
            for (int ky = 0; ky < kernelSize; ky++) {
                for (int kx = 0; kx < kernelSize; kx++) {
                    int imgX = x - radius + kx;
                    int imgY = y - radius + ky;
                    if (imgX >= 0 && imgX < this->WIDTH && imgY >= 0 && imgY < this->HEIGHT) {
                        blurredValue += this->phantom_data[imgY][imgX] * kernel[ky][kx];
                    }
                }
            }
            result[y][x] = blurredValue;
        }
    }
    this->phantom_data = result;
}

double* Phantom::get_dataline(){
    return this->phantom_data[300];
}

double** Phantom::get_phantom_data(){
    return this->phantom_data;
}

int Phantom::getH(){
    return this->HEIGHT;
}
int Phantom::getW(){
    return this->WIDTH;
}
