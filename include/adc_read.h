#ifndef ADC_READ_H
#define ADC_READ_H

#define R1 101000 
#define VCC 5     
#define A_NTC 3.70011733e-04      
#define B_NTC 2.53164380e-04      
#define C_NTC 2.88489489e-08      

double adc_to_voltage(int adc_value);

double adc_to_temperature(int adc_value);

double adc_to_current(int adc_value);

#endif
