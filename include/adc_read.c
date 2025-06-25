#include "adc_read.h"
#include <math.h>


double adc_to_voltage(int adc_value) {
    if (adc_value < 0 || adc_value > 4096) {
        return -1;
    }
    return ((double)adc_value*2.0 / 4096) * 3.3;
}

double adc_to_temperature(int adc_value) {
    double voltage = ((double)adc_value / 4096.0) * 3.3;

 	double Rt, T;

    if (voltage - R1 != 0) {
        Rt = (VCC * R1) / voltage - R1;
    } else {
        Rt = 10000; 
    }

    if ((A_NTC + B_NTC * log(Rt) + C_NTC * pow(log(Rt), 3)) != 0) {
        T = 1 / (A_NTC + B_NTC * log(Rt) + C_NTC * pow(log(Rt), 3));
    } else {
        T = 273.15; 
    }

    if (isnan(T)) {
        T = 0;
    }

    T = T - 273.15;

    return T;
}


double adc_to_current(int adc_value) {
    double voltage = ((double)adc_value / 4096.0) * 3.3;
    double current = (voltage - 2.5) / 0.066;
}
