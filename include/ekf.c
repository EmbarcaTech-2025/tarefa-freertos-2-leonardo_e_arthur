#include "ekf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "battery_model.h"

void matrix_mult(double *result, double *A, int rowsA, int colsA, double *B, int rowsB, int colsB) {
    memset(result, 0, rowsA * colsB * sizeof(double));
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            for (int k = 0; k < colsA; k++) {
                result[i * colsB + j] += A[i * colsA + k] * B[k * colsB + j];
            }
        }
    }
}

void matrix_transpose(double *result, double *A, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[j * rows + i] = A[i * cols + j];
        }
    }
}

void matrix_inv_1x1(double *result, double A) {
    if (A != 0) {
        *result = 1.0 / A;
    }
}

void matrix_eye(double *I, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            I[i * size + j] = 0.0;
        }
        I[i * size + i] = 1.0; 
    }
}


void matrix_subtract(double *result, double *A, double *B, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i * cols + j] = A[i * cols + j] - B[i * cols + j];
        }
    }
}

void matrix_add(double *result, double *A, double *B, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i * cols + j] = A[i * cols + j] + B[i * cols + j];
        }
    }
}

void matrix_scalar_mult(double *result, double *A, double scalar, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i * cols + j] = A[i * cols + j] * scalar;
        }
    }
}

void matrix_scalar_add(double *result, double *A, double scalar, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i * cols + j] = A[i * cols + j] + scalar;
        }
    }
}

void Apriori(double A[2][2], double B[2][1], double i, double X[2][1], double Q[2][2], double P[2][2]) {
    double tempX1[2][1];
    double tempX2[2][1];
    
    matrix_mult(&tempX1[0][0], &A[0][0], 2, 2, &X[0][0], 2, 1);
    matrix_scalar_mult(&tempX2[0][0], &B[0][0], i, 2, 1);
    matrix_add(&X[0][0],&tempX1[0][0],&tempX2[0][0],2,1);
    

    double tempP1[2][2];
    double tempP2[2][2];
    double At[2][2];

    matrix_transpose(&At[0][0], &A[0][0], 2, 2);
    matrix_mult(&tempP1[0][0], &A[0][0], 2, 2, &P[0][0], 2, 2);
    matrix_mult(&tempP2[0][0], &tempP1[0][0], 2, 2, &At[0][0], 2, 2);
    matrix_add(&P[0][0], &tempP2[0][0], &Q[0][0], 2, 2);
}


void Aposteriori(double X[2][1], double P[2][2], double C[1][2], double R, double Error_x, double KG[2][1]) {
    double Ct[2][1];

    matrix_transpose(&Ct[0][0], &C[0][0], 1, 2);

    double PCt[2][1];

    matrix_mult(&PCt[0][0],&P[0][0],2,2,&Ct[0][0],2,1);

    double CP[1][2];

    matrix_mult(&CP[0][0],&C[0][0],1,2,&P[0][0],2,2);
    
    double CPCt[1][1];

    matrix_mult(&CPCt[0][0],&CP[0][0],1,2,&Ct[0][0],2,1);

    double S[1][1];
    matrix_scalar_add(&S[0][0],&CPCt[0][0],R,1,1);

    double S_inv[1][1];
    matrix_inv_1x1(&S_inv[0][0],S[0][0]);


    matrix_scalar_mult(&KG[0][0],&PCt[0][0],S_inv[0][0],2,1);
    
    double tempX1[2][1];
    double tempX2[2][1];

    matrix_scalar_mult(&tempX1[0][0],&KG[0][0],Error_x,2,1);

    matrix_scalar_add(&tempX2[0][0],&X[0][0],0,2,1);

    matrix_add(&X[0][0],&tempX2[0][0],&tempX1[0][0],2,1);

    double eye[2][2];
    
    matrix_eye(&eye[0][0],2);

    double KGC[2][2];
        
    matrix_mult(&KGC[0][0],&KG[0][0],2,1,&C[0][0],1,2);

    double tempP1[2][2];
    double tempP2[2][2];
    matrix_subtract(&tempP1[0][0],&eye[0][0],&KGC[0][0],2,2);

    matrix_scalar_add(&tempP2[0][0],&P[0][0],0,2,1);
        
    matrix_mult(&P[0][0],&tempP1[0][0],2,2,&tempP2[0][0],2,2);


}


void EKF_SOC_OPT(
    double I, double V, double T, double Nc,
    double X[2][1],
    double *Vt,
    double *Vt_Error,
    double P[2][2],  
    double Q[2][2],
    double R,
    double KG[2][1]
) {
    double A[2][2], B[2][1], C[1][2];
    double SOC = X[0][0];

    Nc/=10;

    battery_model(I, X[1][0], T, SOC, Nc, A, B, C, Vt);

    *Vt_Error = V - *Vt;

    Apriori(A,B,I,X,Q,P);   

    if(X[0][0]>=1){
        X[0][0]=1;
    }
    if(X[0][0]<=0){
        X[0][0]=0;
    }
    Aposteriori(X,P,C,R,*Vt_Error,KG);  

    if(X[0][0]>=1){
        X[0][0]=1;
    }
    if(X[0][0]<=0){
        X[0][0]=0;
    }

}