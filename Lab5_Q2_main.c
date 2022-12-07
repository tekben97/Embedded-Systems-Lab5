#include <msp430.h>

#define CALADC_15V_30C  *((unsigned int *)0x1A1A)                 // Temperature Sensor Calibration-30 C
                                                                  // See device datasheet for TLV table memory mapping
#define CALADC_15V_85C  *((unsigned int *)0x1A1C)                 // Temperature Sensor Calibration-85 C

volatile long temp1;
volatile float IntDegC1;

volatile long temp2;
volatile float IntDegC2;

char result[100];
int count;

void ConfigureAdc_temp1(){
        ADCCTL0 |= ADCSHT_8 | ADCON;                                  // ADC ON,temperature sample period>30us
        ADCCTL1 |= ADCSHP;                                            // s/w trig, single ch/conv, MODOSC
        ADCCTL2 &= ~ADCRES;                                           // clear ADCRES in ADCCTL
        ADCCTL2 |= ADCRES_2;                                          // 12-bit conversion results
        ADCMCTL0 |= ADCSREF_1 | ADCINCH_12;                           // ADC input ch A12 => temp sense
        ADCIE |= ADCIE0;
}

void ConfigureAdc_temp2(){
        ADCCTL0 &= ~ADCENC;
        ADCCTL0 |= ADCSHT_8 | ADCON;
        ADCCTL1 |= ADCSHP|ADCCONSEQ_1;                                            // s/w trig, single ch/conv,MODOSC
        ADCCTL2 &= ~ADCRES;                                           // clear ADCRES in ADCCTL
        ADCCTL2 |= ADCRES_2;                                          // 12-bit conversion results
        ADCMCTL0 |= ADCSREF_1 | ADCINCH_6;                           // ADC input ch A12 => temp sense
        ADCIE |= ADCIE0;
}

void initialize_Adc(){
     ADCCTL0 &= ~ADCIFG;//CLEAR FLAG
     ADCMEM0=0x00000000;
     ADCCTL0=0x0000;
     ADCCTL1=0x0000;
}

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    PM5CTL0 &= ~LOCKLPM5;
    P1DIR &= ~(BIT1);

    // Configure the ADC pin
    P1SEL1 |=  BIT6;
    P1SEL1 |=  BIT7;

    TB0CCTL0 |= CCIE;                                             // TBCCR0 interrupt enabled
    TB0CCR0 = 65535;
    TB0CTL = TBSSEL__ACLK | MC__UP;                               // ACLK, UP mode

    // Configure reference
    PMMCTL0_H = PMMPW_H;                                          // Unlock the PMM registers read 2.2.8 & 2.2.9 form the manual
    PMMCTL2 |= INTREFEN | TSENSOREN | REFVSEL_0;                  // Enable internal 1.5V reference and temperature sensor
    //__delay_cycles(400);

    int m=0;

    if(m==0){
      initialize_Adc();
      ConfigureAdc_temp1();
      ADCCTL0 |= ADCENC + ADCSC +ADCMSC;        // Converter Enable, Sampling/conversion start
      while((ADCCTL0 & ADCIFG) == 0);    // check the Flag, while its low just wait
      _delay_cycles(200000);
      temp1 = ADCMEM0;                    // read the converted data into a variable
      ADCCTL0 &= ~ADCIFG;
      ADCIE &= ~ADCIE0;
      IntDegC1 = (temp1-CALADC_15V_30C)*(85-30)/(CALADC_15V_85C-CALADC_15V_30C)+30;
      //__delay_cycles(400);
      m=1;
    }

    // Configure reference
    PMMCTL0_H = PMMPW_H;                                          // Unlock the PMM registers read 2.2.8 & 2.2.9 form the manual
    PMMCTL2 |= INTREFEN | TSENSOREN | REFVSEL_0;                  // Enable internal 1.5V reference and temperature sensor
    //__delay_cycles(400);

    if(m==1){
      initialize_Adc();
      ConfigureAdc_temp2();
      ADCCTL0 |= ADCENC + ADCSC +ADCMSC;        // Converter Enable, Sampling/conversion start
      while((ADCCTL0 & ADCIFG) == 0);    // check the Flag, while its low just wait
      _delay_cycles(20000000);
      temp2 = ADCMEM0;                    // read the converted data into a variable
      ADCCTL0 &= ~ADCIFG;
      ADCIE &= ~ADCIE0;
      IntDegC2 = (temp2-CALADC_15V_30C)*(85-30)/(CALADC_15V_85C-CALADC_15V_30C)+30;
      //__delay_cycles(400);
      m=0;
    }
}

