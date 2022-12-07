#include <msp430.h>

#define CALADC_15V_30C  *((unsigned int *)0x1A1A)                 // Temperature Sensor Calibration-30 C
                                                                  // See device datasheet for TLV table memory mapping
#define CALADC_15V_85C  *((unsigned int *)0x1A1C)                 // Temperature Sensor Calibration-85 C

volatile long temp1;
volatile float IntDegF1;
volatile float IntDegC1;

char result[100];
int count;

void ConfigureAdc_temp1(){
            ADCCTL0 |= ADCSHT_8 | ADCON;                                  // ADC ON,temperature sample period>30us
            ADCCTL1 |= ADCSHP;                                            // s/w trig, single ch/conv, MODOSC
            ADCCTL2 &= ~ADCRES;                                           // clear ADCRES in ADCCTL
            ADCCTL2 |= ADCRES_2;                                          // 12-bit conversion results
            ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;                           // ADC input ch A12 => temp sense
            ADCIE |=ADCIE0;
}

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

        int m=1;

      if(m==1){
          //initialize_Adc();
          PMMCTL0_H = PMMPW_H;                                          // Unlock the PMM registers read 2.2.8 & 2.2.9 form the manual
          PMMCTL2 |= INTREFEN | TSENSOREN | REFVSEL_0;                  // Enable internal 1.5V reference and temperature sensor
          ConfigureAdc_temp1();
          ADCCTL0 |= ADCENC + ADCSC +ADCMSC;        // Converter Enable, Sampling/conversion start
          while((ADCCTL0 & ADCIFG) == 0);    // check the Flag, while its low just wait
          //_delay_cycles(200000);
          temp1 = ADCMEM0;                    // read the converted data into a variable
          ADCCTL0 &= ~ADCIFG;
          IntDegC1 = (temp1-CALADC_15V_30C)*(85-30)/(CALADC_15V_85C-CALADC_15V_30C)+30;
          m=0;
    }
}
