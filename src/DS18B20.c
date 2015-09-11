
#include "stm32f37x_conf.h"

#include "DS18B20.h"

static uint32_t ow_akt_delay;

static void OW_Delay_us(uint32_t delay);
BitAction OneWire_ReadBit(void);
void OneWire_WriteBitHi(void);
void OneWire_WriteBitLo(void);
uint8_t OneWire_ReadByte(void);
void OneWire_WriteByte(uint8_t wert);
void OW_DataLo(void);
void OW_DataHi(void);
uint32_t OW_Mess_Lo_us(uint32_t delay);
BitAction OW_DataRead(void);

static GPIO_TypeDef* actualGPIO = GPIOB;
static uint16_t actual_pin = GPIO_Pin_14;


void oneWire_Init()
{
    //GPIO
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOF, &GPIO_InitStruct);

    // Timer
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

    TIM_Cmd(TIM14, DISABLE);

    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);

    // Timer auf 1us einstellen
    TIM_TimeBaseStructure.TIM_Period = 71; // Teilung durch Taktfrequenz -> 1 us
    TIM_TimeBaseStructure.TIM_Prescaler = 4; // Teilung durch 5
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

    TIM_ARRPreloadConfig(TIM14, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;

    // Update Interrupt enable
    TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE);

    // NVIC konfig
    NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    OW_DataHi();

    /*TIM_OCInitTypeDef TIM_OC_InitStruct;
    TIM_OC_InitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OC_InitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OC_InitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Set;
    TIM_OC_InitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC_InitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OC_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC_InitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OC_InitStruct.TIM_Pulse = 36;
    TIM_OC1Init(TIM14, &TIM_OC_InitStruct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_0);*/

    //TIM_Cmd(TIM14, ENABLE);
}

void TIM14_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET)
    {
        // wenn Interrupt aufgetreten
        TIM_ClearITPendingBit(TIM14, TIM_IT_Update);

        ow_akt_delay++;
    }
}

static void OW_Delay_us(uint32_t delay)
{
    // Timer auf 0 setzen
    ow_akt_delay=0;
    // Timer starten
    TIM_Cmd(TIM14, ENABLE);
    // warten bis Timer abgelaufen
    while(ow_akt_delay<delay);
    // Timer stoppen
    TIM_Cmd(TIM14, DISABLE);
}

BitAction OneWire_ReadBit(void)
{
    BitAction ret_wert;

    OW_DataLo();
    OW_Delay_us(1);
    OW_DataHi();
    OW_Delay_us(2);
    ret_wert = OW_DataRead();
    OW_Delay_us(10);

    return(ret_wert);
}

void OneWire_WriteBitHi(void)
{
    OW_DataLo();
    OW_Delay_us(1);
    OW_DataHi();
    OW_Delay_us(16);
}

void OneWire_WriteBitLo(void)
{
    OW_DataLo();
    OW_Delay_us(16);
    OW_DataHi();
    OW_Delay_us(1); // recovery
}

uint8_t OneWire_ReadByte(void)
{
    uint8_t ret_wert=0;
    uint8_t bit_maske=0x01,n;

    // 8bit per OneWire lesen
    for(n=0;n<8;n++)
    {
        if(OneWire_ReadBit() == Bit_SET)
            ret_wert |= bit_maske;

        bit_maske=bit_maske << 1;
    }

    return(ret_wert);
}

void OneWire_WriteByte(uint8_t wert)
{
    uint8_t bit_maske=0x01,n;

    // 8bit per OneWire senden
    for(n=0;n<8;n++)
    {
        if((wert&bit_maske)==0)
            OneWire_WriteBitLo();
        else
            OneWire_WriteBitHi();

        bit_maske = bit_maske << 1;
    }
}

uint8_t OneWire_ResetSequenz(void)
{
    ErrorStatus ret_wert = 1;
    uint32_t lo_time = 0;
    BitAction hi_pegel;

    // Initialisierung-Sequenz
    OW_Delay_us(20);
    OW_DataLo();
    OW_Delay_us(112); //  480<T<640
    OW_DataHi();
    OW_Delay_us(4);  // 15<T<60
    lo_time = OW_Mess_Lo_us(60); // messen vom presence puls 60<T<240
    OW_Delay_us(10);
    hi_pegel = OW_DataRead(); // messen ob am ende wieder hi
    OW_Delay_us(24);

    // Auswertung ob ok
    if((lo_time > 4) && (hi_pegel == Bit_SET))
        // der presence Puls vom Slave muss > 30 us sein
        ret_wert = 0;

    return(ret_wert);
}

void OW_DataLo(void)
{
    GPIO_ResetBits(actualGPIO, actual_pin);
}

void OW_DataHi(void)
{
    GPIO_SetBits(actualGPIO, actual_pin);
}

uint32_t OW_Mess_Lo_us(uint32_t delay)
{
    uint32_t ret_wert=0;
    uint32_t start_pos=0, stop_pos=0;

    // Timer auf 0 setzen
    ow_akt_delay=0;
    // Timer starten
    TIM_Cmd(TIM14, ENABLE);
    // warten bis Timer abgelaufen
    do
    {
        // Pegel am Data-Pin messen
        if(OW_DataRead()==Bit_RESET)
        {
            if(start_pos==0)
                start_pos = ow_akt_delay;
        }
        else if(start_pos > 0)
            if(stop_pos==0)
                stop_pos=ow_akt_delay;
    } while(ow_akt_delay < delay);
    // Timer stoppen
    TIM_Cmd(TIM14, DISABLE);

    // Lo-Impuls ausrechnen
    if(stop_pos>start_pos)
        ret_wert = stop_pos - start_pos;

    return(ret_wert);
}

BitAction OW_DataRead(void)
{
    BitAction ret_wert;

    ret_wert = GPIO_ReadInputDataBit(actualGPIO, actual_pin);

    return(ret_wert);
}

void OneWire_ReadRomCode(uint8_t* rom_code)
{
    uint32_t n;

    if(OneWire_ResetSequenz() == 0)
    {
        // befehl senden (auslesen einer Rom-ID)
        OneWire_WriteByte(0x33);
        // Rom-Code auslesen
        for(n=0; n<8; n++)
        {
            rom_code[n] = OneWire_ReadByte();
            //romCode |= ((uint64_t) temp) << (n*8);
        }
        // reset seq
        OneWire_ResetSequenz();
    }
}

void DS18B20_StartMeasurement(uint8_t* rom_code)
{
    OneWire_WriteByte(0x55); // MATCH_ROM_CMD

    for(uint8_t n=0;n<8;n++)
    {
        OneWire_WriteByte(rom_code[n]);
    }

    OneWire_WriteByte(0x44); //Messung starten
}

int32_t DS18B20_GetTemp(uint8_t* rom_code)
{
    uint8_t temp_array[9];
    int32_t ret_val = 0;

    BitAction wait_bit;
    uint32_t timeout = 0;
    do
    {
        wait_bit = OneWire_ReadBit();
        timeout++;
    } while ((wait_bit == Bit_RESET) && (timeout < 50000));

    if (wait_bit != Bit_RESET)
    {
        // Messung fertig
        if (!OneWire_ResetSequenz())
        {
            // Gerät reagiert
            OneWire_WriteByte(0x55); // MATCH_ROM_CMD

            for(uint8_t n=0;n<8;n++)
            {
                OneWire_WriteByte(rom_code[n]);
            }

            OneWire_WriteByte(0xBE); // Read temp

            for (uint8_t n = 0; n < 9; n++)
                temp_array[n] = OneWire_ReadByte();


            uint16_t raw_temp;
            if ((temp_array[4] & 0x60) == 0x60)
            {
                // 12 Bit res
                raw_temp = (temp_array[1] & 0x07) << 8;
                raw_temp |= temp_array[0];
                if ((temp_array[1] & 0x80) == 0x00)
                {
                    ret_val = raw_temp * 625;
                }
                else
                {
                    raw_temp ^= 0x07FF;
                    raw_temp++;
                    ret_val = raw_temp * (-625);
                }
            }
            else if ((temp_array[4] & 0x60) == 0x40)
            {
                // 11 Bit res
                raw_temp = (temp_array[1] & 0x07) << 7;
                raw_temp |= (temp_array[0] >> 1);
                if ((temp_array[1] & 0x80) == 0x00)
                {
                    ret_val = raw_temp * 1250;
                }
                else
                {
                    raw_temp ^= 0x03FF;
                    raw_temp++;
                    ret_val = raw_temp * (-1250);
                }
            }
            else if ((temp_array[4] & 0x60) == 0x20)
            {
                // 10 Bit res
                raw_temp = (temp_array[1] & 0x07) << 6;
                raw_temp |= (temp_array[0] >> 2);
                if ((temp_array[1] & 0x80) == 0x00)
                {
                    ret_val = raw_temp * 2500;
                }
                else
                {
                    raw_temp ^= 0x01FF;
                    raw_temp++;
                    ret_val = raw_temp * (-2500);
                }
            }
            else if ((temp_array[4] & 0x60) == 0x00)
            {
                // 09 Bit res
                raw_temp = (temp_array[1] & 0x07) << 5;
                raw_temp |= (temp_array[0] >> 3);
                if ((temp_array[1] & 0x80) == 0x00)
                {
                    ret_val = raw_temp * 5000;
                }
                else
                {
                    raw_temp ^= 0x00FF;
                    raw_temp++;
                    ret_val = raw_temp * (-5000);
                }
            }
        }
    }

    return ret_val;
}

void DS18B20_SetResolution(uint8_t* rom_code, uint8_t bit)
{
    uint8_t conf_reg;

    switch (bit)
    {
        default:
        case 9:
        {
            conf_reg = 0x00;
        } break;
        case 10:
        {
            conf_reg = 0x20;
        } break;
        case 11:
        {
            conf_reg = 0x40;
        } break;
        case 12:
        {
            conf_reg = 0x60;
        } break;
    }

    OneWire_ResetSequenz();
    OneWire_WriteByte(0x55); // match ROM

    for (uint8_t i = 0; i < 8; i++)
        OneWire_WriteByte(rom_code[i]);

    OneWire_WriteByte(0x4E); // Write Scratchpad

    OneWire_WriteByte(0x7F); // Alarm Register High
    OneWire_WriteByte(0x80); // Alarm Register High
    OneWire_WriteByte(conf_reg); // Alarm Register High

    OneWire_ResetSequenz();
}

int32_t DS18B20_ReadSensor(uint8_t sensor)
{
    switch (sensor)
    {
        case 1:
        default:
            {
                actualGPIO = GPIOB;
                actual_pin = GPIO_Pin_14;
            } break;
        case 2:
            {
                actualGPIO = GPIOF;
                actual_pin = GPIO_Pin_7;
            } break;
        case 3:
            {
                actualGPIO = GPIOF;
                actual_pin = GPIO_Pin_6;
            } break;
    }

    uint8_t rom_code[8];
    OneWire_ReadRomCode(rom_code);
    DS18B20_SetResolution(rom_code, 9);
    DS18B20_StartMeasurement(rom_code);
    return DS18B20_GetTemp(rom_code);
}
