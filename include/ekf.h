#ifndef EKF_H
#define EKF_H


extern double Q[2][2];
extern double R;

void Apriori(double A[2][2], double B[2][1], double i, double X[2][1], double Q[2][2], double P[2][2]);
void Aposteriori(double X[2][1], double P[2][2], double C[1][2], double R, double Error_x, double KG[2][1]);

void matrix_mult(double *result, double *A, int rowsA, int colsA, double *B, int rowsB, int colsB);
void matrix_transpose(double *result, double *A, int rows, int cols);
void matrix_mult(double *result, double *A, int rowsA, int colsA, double *B, int rowsB, int colsB);
void matrix_transpose(double *result, double *A, int rows, int cols);
void matrix_inv_1x1(double *result, double A);
void matrix_eye(double *I, int size);
void matrix_subtract(double *result, double *A, double *B, int rows, int cols);
void matrix_scalar_mult(double *result, double *A, double scalar, int rows, int cols);
void matrix_add(double *result, double *A, double *B, int rows, int cols);
void matrix_scalar_add(double *result, double *A, double scalar, int rows, int cols);


void EKF_SOC_OPT(double I, double V, double T, double Nc, double X[2][1], double *Vt, double *Vt_Error, double P[2][2], double Q[2][2], double R, double KG[2][1]);

#endif