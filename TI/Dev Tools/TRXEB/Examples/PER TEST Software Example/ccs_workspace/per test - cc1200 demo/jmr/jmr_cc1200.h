#ifndef JMR_JMR_CC1200_H_
#define JMR_JMR_CC1200_H_


//Globals
extern void jmr_cc1200_bspInit(void);
void cc1200_spi_init(void);
extern uint8_t jmr_cc1200_sniffInitApp(void);
extern uint8_t jmr_cc1200_freqConfig(uint16_t index);
extern uint8_t jmr_cc1200_masterStartApp(void);
extern void jmr_cc1200_tx_demo(void);

radioChipType_t sniffRadioChipType;

#endif /* JMR_JMR_CC1200_H_ */
