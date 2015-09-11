

#ifndef HD44780_H
#define HD74480_h


void HD44780_Init();
void HD44780_BL(BitAction state);
void HD44780_string(uint8_t line, uint8_t position, char* str);

#endif
