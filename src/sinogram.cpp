#include "sinogram.hpp"
#define _USE_MATH_DEFINES
#include <cmath>

Sinogram::Sinogram(Phantom* phantom, int x_phys, int y_phys,
        int NPHYS, int NCSYS){
    this->phantom = phantom;
    this->x_phys = x_phys;
    this->y_phys = y_phys;
    this->M = phantom->getH();
    this->N = phantom->getW();
    this->NPHYS = NPHYS;
    this->NCSYS = NCSYS;
    this->sinogram = new double*[NPHYS];
    for (int i = 0 ; i < NPHYS; i++){
        this->sinogram[i] = new double[NCSYS];
    }
    this->angles = new double[NPHYS];
    this->detector_pos = new double[NCSYS];
    set_detectors();
    set_phys();
}
Sinogram::~Sinogram(){
    delete[] this->angles;
    delete[] this->detector_pos;
    for (int i = 0 ; i < this->NPHYS; i++){
        delete[] this->sinogram[i];
    }
    delete[] this->sinogram;
}

void Sinogram::set_detectors(){
    double CSY_M = M_SQRT2 / 2 * this->N;
    double d_CSY = 2 * CSY_M / (this->NCSYS - 1);
    for (int k = 0; k < this->NCSYS; k++){
        this->detector_pos[k] = -CSY_M + k * d_CSY;
    }
}
void Sinogram::set_phys(){
    double D_A = M_PI / this->NPHYS;
    for (int i = 0; i < this->NPHYS; i++){
        this->angles[i] = i * D_A;
    }
}
void Sinogram::set_sinogram(){
    double** data = this->phantom->get_phantom_data();
    int centerX = this->N / 2;
    int centerY = this->M / 2;

    for (int phi_idx = 0; phi_idx < this->NPHYS; phi_idx++){
        double phi = this->angles[phi_idx];
        double cos_phi = cos(phi);
        double sin_phi = sin(phi);

        for (int det_idx = 0; det_idx < this->NCSYS; det_idx++){
            double s = this->detector_pos[det_idx];
            double sum = 0.0;
            int count = 0;

            for (int y = 0; y < this->M; y++){
                for (int x = 0; x < this->N; x++){
                    double x_c = x - centerX;
                    double y_c = y - centerY;
                    double s_calc = x_c * cos_phi + y_c * sin_phi;
                    
                    if (fabs(s_calc - s) < 0.5){
                        sum += data[y][x];
                        count++;
                    }
                }
            }
            this->sinogram[phi_idx][det_idx] = (count > 0) ? sum / count : 0.0;
        }
    }
}

double** Sinogram::get_sinogram(){
    return this->sinogram;
}