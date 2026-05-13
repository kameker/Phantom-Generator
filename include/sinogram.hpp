#include "phantom.hpp"


class Sinogram{
public:
    Sinogram(Phantom* phantom, int x_phys, int y_phys,
        int NPHYS, int NCSYS);
    ~Sinogram();
    void set_detectors();
    void set_phys();
    void set_sinogram();
    double** get_sinogram();
    int getM();
    int getN();
    double get_angles();
private:
    Phantom* phantom;
    int x_phys, y_phys;
    int M,N;
    int NPHYS;
    int NCSYS;
    double** sinogram;
    double* angles;
    double* detector_pos;
};