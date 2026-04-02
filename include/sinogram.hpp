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