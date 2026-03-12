#ifndef GPHS_K
#define GPHS_K
class Phantom {
public:
    Phantom(int w, int h, double delta, 
        double sigma, int minR, int maxR, int count);
    ~Phantom();
    void generate_phantom();
    int rand_radius();
    void setGausBlur(double sigma);
    void fillCircle(int X, int Y, int radius);
    void reSetData(int w, int h, double delta,
        double sigma, int minR, int maxR, int count);
    void compute_dataline();
    double** get_phantom_data();
    int getH();
    int getW();
    double* get_dataline();
private:
    double** phantom_data;
    double* dataline;
    double delta;
    double smothingSigma;
    int circles_count;
    int radius;
    int WIDTH, HEIGHT;
    int minR, maxR;
    int centerX, centerY;
};
#endif