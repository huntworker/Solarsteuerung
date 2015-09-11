#include "stm32f37x_conf.h"

#include "HD44780.h"

static void writeNibble(uint8_t nibble);
static void writeCommand4bit(uint8_t command);
static void writeCommand(uint8_t command);
static void writeData(uint8_t data);

static void delay(uint32_t nCount)
{
    while (nCount--)
    {
        __NOP(); // ~60ns
    }
}

void HD44780_Init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    // Backlight
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    //RS
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    //DATA
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    //EN
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOD, &GPIO_InitStruct);




    delay(500000); // ~15 ms
    writeCommand4bit(0x30);
    delay(100000); // ~4.1 ms
    writeCommand4bit(0x30);
    delay(50000); // ~100 us
    writeCommand4bit(0x30);
    delay(20000);
    writeCommand4bit(0x20);
    delay(20000);
    writeCommand(0x28); // 2 lines, 5x8 font
    delay(20000);
    writeCommand(0x08);
    delay(20000);
    writeCommand(0x01);
    delay(20000);
    writeCommand(0x06);
    delay(20000);
    writeCommand(0x0C);
    delay(20000);

    writeCommand(0x80); // set DDRAM adress 0
    delay(20000);

}

static void writeCommand4bit(uint8_t command)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15); //RS low
    writeNibble(command >> 4);
}

static void writeNibble(uint8_t nibble)
{
    GPIO_SetBits(GPIOD, GPIO_Pin_8); // EN high
    GPIO_WriteBit(GPIOC, GPIO_Pin_6, (nibble & 0x01));
    GPIO_WriteBit(GPIOC, GPIO_Pin_7, (nibble & 0x02));
    GPIO_WriteBit(GPIOC, GPIO_Pin_8, (nibble & 0x04));
    GPIO_WriteBit(GPIOC, GPIO_Pin_9, (nibble & 0x08));
    GPIO_ResetBits(GPIOD, GPIO_Pin_8); // EN low
}

static void writeCommand(uint8_t command)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15); //RS low
    writeNibble(command >> 4);
    writeNibble(command);
}

static void writeData(uint8_t data)
{
    delay(1000);
    GPIO_SetBits(GPIOB, GPIO_Pin_15); //RS high
    writeNibble(data >> 4);
    delay(1000);
    writeNibble(data);
}

void HD44780_BL(BitAction state)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, state);
}

void HD44780_string(uint8_t line, uint8_t position, char* str)
{
    uint8_t address;
    if (line == 0)
        address = position;
    else if (line == 1)
        address = position + 40;
    else
        return;

    delay(20000);
    writeCommand(0x80 | address);
    delay(20000);

    while(*str)
        writeData(*str++);

}
