#include <windows.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "phantom.hpp"
#include "phantom_utils.hpp"
#include "sinogram_utils.hpp"

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600


int main(){
    double delta, sigma;
    int minR, maxR, count;
    const int NPHYS = 360;
    const int NCSYS = 256;
    delta = 0.1;
    sigma = 2;
    minR = 10;
    maxR = 50;
    count = 100;
    Phantom* g_phantom = new Phantom(IMAGE_WIDTH, IMAGE_HEIGHT, delta, sigma, minR, maxR, count);
    Sinogram* sinogram = new Sinogram(g_phantom, 0, 0,NPHYS, NCSYS);
    g_phantom->generate_phantom();
    sinogram->set_sinogram();
    save_data(g_phantom);
    save_phantom_image(g_phantom);
    save_sinogram_image(sinogram);
    double** recovered_phantom_data = recovger_phatom_by_p_philtration(sinogram);
    save_recovered_phantom_image(recovered_phantom_data, IMAGE_WIDTH, IMAGE_HEIGHT);
}