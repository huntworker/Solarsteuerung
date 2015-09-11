void oneWire_Init();
void OneWire_ReadRomCode(uint8_t* rom_code);
void DS18B20_StartMeasurement(uint8_t* rom_code);
int32_t DS18B20_GetTemp(uint8_t* rom_code);
int32_t DS18B20_ReadSensor(uint8_t sensor);
void DS18B20_SetResolution(uint8_t* rom_code, uint8_t bit);
