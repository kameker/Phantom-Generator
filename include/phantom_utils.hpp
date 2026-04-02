#include "phantom.hpp"
#include <stdio.h>


void print_phantom(Phantom* phantom);
void save_data(Phantom* phantom);
void save_image(Phantom* phantom);

void gen_sinusogram(Phantom* phantom, const int NPHI, const int DC);
