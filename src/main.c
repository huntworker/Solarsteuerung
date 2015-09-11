/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/

/* Timer:
TIM3:  Button
TIM4:  Encoder
TIM5:  Main (Debug)
TIM14: OneWire

*/


#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#include "stm32f37x_conf.h"
#include "HD44780.h"
#include "DS18B20.h"
#include "encoder.h"
#include "Button.h"

void SystickInit();

typedef struct
{
    uint32_t can_id;
    int16_t diff_col_sp;
    int16_t hyst_col_sp;
    int16_t t_on_sp_lad;
    int16_t t_off_sp_lad;
    int16_t t_on_umw;
    int16_t t_off_umw;
} settings_t;

typedef enum
{
    MENUE_1 = 0,
    MENUE_10,
    MENUE_11,
    MENUE_110,
    MENUE_12,
    MENUE_120,
    MENUE_13,
    MENUE_130,
    MENUE_14,
    MENUE_140,
    MENUE_15,
    MENUE_150,
    MENUE_16,
    MENUE_160,
    MENUE_17,
    MENUE_170,
    MENUE_2,
    MENUE_20,
    MENUE_21,
    MENUE_210
} menue_points_t;

typedef struct
{
    char* line1;
    char* line2;
    menue_points_t menue_up;
    menue_points_t menue_down;
    menue_points_t menue_ok;
    uint8_t use_arrow_keys;
} menue_t;

menue_t menue_points[] =
{
    { "Einstellungen   ", "                ", MENUE_2, MENUE_2, MENUE_11, 0 }, // 1
    { "Einstellungen   ", "zurueck         ", MENUE_17, MENUE_11, MENUE_1, 0 }, // 10
    { "Einstellungen   ", "CAN-ID          ", MENUE_10, MENUE_12, MENUE_110, 0 }, // 11
    { "CAN-ID          ", "                ", MENUE_110, MENUE_110, MENUE_11, 1 }, // 110
    { "Einstellungen   ", "Diff. Kol-Sp    ", MENUE_11, MENUE_13, MENUE_120, 0 }, // 12
    { "Diff. Kol-Sp    ", "                ", MENUE_120, MENUE_120, MENUE_12, 1 }, // 120
    { "Einstellungen   ", "Hyst. Solar     ", MENUE_12, MENUE_14, MENUE_130, 0 }, // 13
    { "Hyst. Solar     ", "                ", MENUE_130, MENUE_130, MENUE_13, 1 }, // 130
    { "Einstellungen   ", "T ON Sp. lad.   ", MENUE_13, MENUE_15, MENUE_140, 0 }, // 14
    { "T ON Sp. lad    ", "                ", MENUE_140, MENUE_140, MENUE_14, 1 }, // 140
    { "Einstellungen   ", "T OFF Sp. lad.  ", MENUE_14, MENUE_16, MENUE_150, 0 }, // 15
    { "T OFF Sp. lad.  ", "                ", MENUE_150, MENUE_150, MENUE_15, 1 }, // 150
    { "Einstellungen   ", "T ON Umw.       ", MENUE_15, MENUE_17, MENUE_160, 0 }, // 16
    { "T ON Wmw.       ", "                ", MENUE_160, MENUE_160, MENUE_16, 1 }, // 160
    { "Einstellungen   ", "T OFF Umw.      ", MENUE_16, MENUE_10, MENUE_170, 0 }, // 17
    { "T OFF Umw.      ", "                ", MENUE_170, MENUE_170, MENUE_17, 1 }, // 170
    { "Status          ", "                ", MENUE_1, MENUE_1, MENUE_21, 0 }, // 2
    { "Status          ", "zurueck         ", MENUE_21, MENUE_21, MENUE_2, 0 }, // 20
    { "Status          ", "Kol SpU SpO  SUL", MENUE_20, MENUE_20, MENUE_210, 0 }, // 21
    { "Kol SpU SpO  SUL", "                ", MENUE_210, MENUE_210, MENUE_21, 1 } // 210
};

volatile uint32_t flags = 0;
#define FLAG_BTN_UP   (1 << 0)
#define FLAG_BTN_DOWN (1 << 1)
#define FLAG_BTN_OK   (1 << 2)
#define FLAG_PUMP_SOL (1 << 3)
#define FLAG_PUMP_SP  (1 << 4)
#define FLAG_PUMP_UMW (1 << 5)
#define FLAG_MENU_UPD (1 << 6)

volatile uint32_t lcd_timeout = 0;

int main(void)
{
    SystickInit();

    // 5V EN
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_SetBits(GPIOA, GPIO_Pin_0); // Enable 5V DCDC converter

    /*#############################################################*/

    HD44780_Init();
    HD44780_BL(ENABLE);

    oneWire_Init();

    encoder_init();

    Button_Init();

    char Buffer[20];

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    RCC_MCOConfig(RCC_MCOSource_SYSCLK);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_ResetBits(GPIOC, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);



    //HD44780_string(0, 0, "Zeile 1");
    //HD44780_string(1, 1, "Zeile 2");


    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Timer
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_Cmd(TIM5, DISABLE);

    // Timer auf 1us einstellen
    TIM_TimeBaseStructure.TIM_Period = 71; // Teilung durch Taktfrequenz -> 1 us
    TIM_TimeBaseStructure.TIM_Prescaler = 4; // Teilung durch 5
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_ARRPreloadConfig(TIM5, ENABLE);

    TIM_OCInitTypeDef TIM_OC_InitStruct;
    TIM_OC_InitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OC_InitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OC_InitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Set;
    TIM_OC_InitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC_InitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OC_InitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC_InitStruct.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OC_InitStruct.TIM_Pulse = 36;
    TIM_OC3Init(TIM5, &TIM_OC_InitStruct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_2);

    TIM_Cmd(TIM5, ENABLE);

    //uint8_t rom_code[8];
    menue_points_t actual_menue = MENUE_210; // Standardmenü
    int32_t temp[3] = {0};

    settings_t settings;
    settings.can_id = 0x40;
    settings.diff_col_sp = 3;
    settings.hyst_col_sp = 1;
    settings.t_off_sp_lad = 65;
    settings.t_off_umw = 75;
    settings.t_on_sp_lad = 70;
    settings.t_on_umw = 80;

    flags |= FLAG_MENU_UPD;

    for(;;)
    {
        /*OneWire_ReadRomCode(rom_code);
        snprintf(Buffer, 16, "ID: %#02"PRIx8"%02"PRIx8"%02"PRIx8"%02"PRIx8"%02"PRIx8"%02"PRIx8"%02"PRIx8"%02"PRIx8"",
                 rom_code[0], rom_code[1], rom_code[2], rom_code[3], rom_code[4], rom_code[5], rom_code[6], rom_code[7]);
        HD44780_string(0, 0, Buffer);*/


        /*snprintf(Buffer, 16, "Temp: %"PRId32",%"PRId32",%"PRId32"", temp[0]/10000, temp[1]/10000, temp[2]/10000);
        //HD44780_string(1, 0, Buffer);*/

        if (ButtonGetState(0))
        {
            if (lcd_timeout < 60)
            {
                flags |= FLAG_BTN_OK;
            }
            lcd_timeout = 0;
        }

        if (lcd_timeout >= 60)
        {
            lcd_timeout = 60;
            HD44780_BL(DISABLE);
        }
        else
            HD44780_BL(ENABLE);

        if (flags & FLAG_MENU_UPD)
        {
            HD44780_string(0, 0, menue_points[actual_menue].line1);
            HD44780_string(1, 0, menue_points[actual_menue].line2);
            flags &= ~FLAG_MENU_UPD;
        }

        switch (actual_menue)
        {
            case MENUE_110:
            {
                // CAN-ID
                snprintf(Buffer, 17, "%08"PRIx32"        ", settings.can_id);
                HD44780_string(1, 0, Buffer);
                if (flags & FLAG_BTN_UP)
                {
                    flags &= ~FLAG_BTN_UP;
                    settings.can_id++;
                }
                else if (flags & FLAG_BTN_DOWN)
                {
                    flags &= ~FLAG_BTN_DOWN;
                    settings.can_id--;
                }
            } break;
            case MENUE_120:
            {
                // Diff Kol. Sp.
                snprintf(Buffer, 17, "%2"PRId16"              ", settings.diff_col_sp);
                HD44780_string(1, 0, Buffer);
                if (flags & FLAG_BTN_UP)
                {
                    flags &= ~FLAG_BTN_UP;
                    settings.diff_col_sp++;
                }
                else if (flags & FLAG_BTN_DOWN)
                {
                    flags &= ~FLAG_BTN_DOWN;
                    settings.diff_col_sp--;
                }
            } break;
            case MENUE_130:
            {
                // Hyst. Solar
                snprintf(Buffer, 17, "%2"PRId16"              ", settings.hyst_col_sp);
                HD44780_string(1, 0, Buffer);
                if (flags & FLAG_BTN_UP)
                {
                    flags &= ~FLAG_BTN_UP;
                    settings.hyst_col_sp++;
                }
                else if (flags & FLAG_BTN_DOWN)
                {
                    flags &= ~FLAG_BTN_DOWN;
                    settings.hyst_col_sp--;
                }
            } break;
            case MENUE_140:
            {
                // T ON Sp.lad
                snprintf(Buffer, 17, "%2"PRId16"              ", settings.t_on_sp_lad);
                HD44780_string(1, 0, Buffer);
                if (flags & FLAG_BTN_UP)
                {
                    flags &= ~FLAG_BTN_UP;
                    settings.t_on_sp_lad++;
                }
                else if (flags & FLAG_BTN_DOWN)
                {
                    flags &= ~FLAG_BTN_DOWN;
                    settings.t_on_sp_lad--;
                }
            } break;
            case MENUE_150:
            {
                // T OFF Sp.lad
                snprintf(Buffer, 17, "%2"PRId16"              ", settings.t_off_sp_lad);
                HD44780_string(1, 0, Buffer);
                if (flags & FLAG_BTN_UP)
                {
                    flags &= ~FLAG_BTN_UP;
                    settings.t_off_sp_lad++;
                }
                else if (flags & FLAG_BTN_DOWN)
                {
                    flags &= ~FLAG_BTN_DOWN;
                    settings.t_off_sp_lad--;
                }
            } break;
            case MENUE_160:
            {
                // T ON Umw.
                snprintf(Buffer, 17, "%2"PRId16"              ", settings.t_on_umw);
                HD44780_string(1, 0, Buffer);
                if (flags & FLAG_BTN_UP)
                {
                    flags &= ~FLAG_BTN_UP;
                    settings.t_on_umw++;
                }
                else if (flags & FLAG_BTN_DOWN)
                {
                    flags &= ~FLAG_BTN_DOWN;
                    settings.t_on_umw--;
                }
            } break;
            case MENUE_170:
            {
                // T OFF Umw.
                snprintf(Buffer, 17, "%2"PRId16"              ", settings.t_off_umw);
                HD44780_string(1, 0, Buffer);
                if (flags & FLAG_BTN_UP)
                {
                    flags &= ~FLAG_BTN_UP;
                    settings.t_off_umw++;
                }
                else if (flags & FLAG_BTN_DOWN)
                {
                    flags &= ~FLAG_BTN_DOWN;
                    settings.t_off_umw--;
                }
            } break;
            case MENUE_210:
            {
                // Status
                uint8_t state = 0;
                if (flags & FLAG_PUMP_SOL)
                    state += 100;
                if (flags & FLAG_PUMP_UMW)
                    state += 10;
                if (flags & FLAG_PUMP_SP)
                    state += 1;
                snprintf(Buffer, 17, "%3"PRId32" %3"PRId32" %3"PRId32"  %03"PRIu8"", temp[0], temp[1], temp[2], state);
                HD44780_string(1, 0, Buffer);
            } break;
            default:
            {

            } break;
        }

        for (int i = 0; i < 3; i++)
        {
            temp[i] = DS18B20_ReadSensor(i+1) / 10000;
        }

        if ((!menue_points[actual_menue].use_arrow_keys) && (flags & (FLAG_BTN_DOWN)))
        {
            flags &= ~FLAG_BTN_DOWN;
            actual_menue = menue_points[actual_menue].menue_down;
            flags |= FLAG_MENU_UPD;
        }
        if ((!menue_points[actual_menue].use_arrow_keys) && (flags & (FLAG_BTN_UP)))
        {
            flags &= ~FLAG_BTN_UP;
            actual_menue = menue_points[actual_menue].menue_up;
            flags |= FLAG_MENU_UPD;
        }
        if (flags & (FLAG_BTN_OK))
        {
            flags &= ~FLAG_BTN_OK;
            actual_menue = menue_points[actual_menue].menue_ok;
            flags |= FLAG_MENU_UPD;
        }

        if (flags & FLAG_PUMP_SOL)
        {
            if ((temp[0] - temp[1]) <= (settings.diff_col_sp - settings.hyst_col_sp))
                flags &= ~FLAG_PUMP_SOL;
        }
        else
        {
            if ((temp[0] - temp[1]) > (settings.diff_col_sp + settings.hyst_col_sp))
                flags |= FLAG_PUMP_SOL;
        }

        if (temp[1] >= settings.t_on_sp_lad)
            flags |= FLAG_PUMP_SP;
        if (temp[1] <= settings.t_off_sp_lad)
            flags &= ~FLAG_PUMP_SP;

        if (temp[1] >= settings.t_on_umw)
            flags |= FLAG_PUMP_UMW;
        if (temp[1] <= settings.t_off_umw)
            flags &= ~FLAG_PUMP_UMW;


        if (flags & FLAG_PUMP_SOL)
            GPIO_SetBits(GPIOC, GPIO_Pin_15);
        else
            GPIO_ResetBits(GPIOC, GPIO_Pin_15);
        if (flags & FLAG_PUMP_SP)
            GPIO_SetBits(GPIOC, GPIO_Pin_14);
        else
            GPIO_ResetBits(GPIOC, GPIO_Pin_14);
        if (flags & FLAG_PUMP_UMW)
            GPIO_SetBits(GPIOC, GPIO_Pin_13);
        else
            GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    }
}

void encoder_count_up()
{
    if (lcd_timeout < 60)
    {
        flags |= FLAG_BTN_UP;
    }
    lcd_timeout = 0;
}

void encoder_count_down()
{
    if (lcd_timeout < 60)
    {
        flags |= FLAG_BTN_DOWN;
    }
    lcd_timeout = 0;
}

void SystickInit()
{
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 10); // tick every 100 ms
}

void SysTick_Handler(void)
{
    // every 100 ms
    static int counter = 9;
    counter--;
    if (!counter)
    {
        // every second
        counter = 9;
        lcd_timeout++;
    }

}

/*void HardFault_Handler()
{
    while(1);
}*/

void Default_Handler()
{
    while(1);
}

/*void NMI_Handler()
{
    while(1);
}

void BusFault_Handler()
{
    while(1);
}*/
