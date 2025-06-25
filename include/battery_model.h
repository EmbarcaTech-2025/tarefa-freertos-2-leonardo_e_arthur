#ifndef BATTERY_MODEL_H
#define BATTERY_MODEL_H

#define TEMP_COUNT 7 
#define SOC_COUNT 15

extern double temp_lut[TEMP_COUNT];
extern double soc_lut[SOC_COUNT];
extern double F_R0[TEMP_COUNT][SOC_COUNT];
extern double F_R1[TEMP_COUNT][SOC_COUNT];
extern double F_C1[TEMP_COUNT][SOC_COUNT];

extern double soc_ocv[7];
extern double d_soc_ocv[6];

double interpolate(double x1, double y1, double x2, double y2, double x);
double find_value(double temperature, double soc, 
                   double *temp_vector,
                   double *soc_vector,
                   double values_lut[TEMP_COUNT][SOC_COUNT]);
double polyval(double *coefficients, int num_coeffs, double x);
void battery_model(double i, double V1, double T, double SOC, double Nc, 
           double A[2][2], double b[2][1], double c[1][2], double *Vt);

#endif 