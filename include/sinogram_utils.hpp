#include <sinogram.hpp>


void save_sinogram_image(Sinogram* sinogram);
double** recovger_phatom_by_p_philtration(Sinogram* sinusogram);
void save_recovered_phantom_image(double **data, int w, int h);