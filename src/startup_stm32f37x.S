/* Cortex-M4 stm32f37x.S
*/
    .syntax unified
    .arch armv7-m

    .section .stack
    .align 3
#ifdef __STACK_SIZE
    .equ    Stack_Size, __STACK_SIZE
#else
    .equ    Stack_Size, 0x400
#endif
    .globl    __StackTop
    .globl    __StackLimit
__StackLimit:
    .space    Stack_Size
    .size __StackLimit, . - __StackLimit
__StackTop:
    .size __StackTop, . - __StackTop

    .section .heap
    .align 3
#ifdef __HEAP_SIZE
    .equ    Heap_Size, __HEAP_SIZE
#else
    .equ    Heap_Size, 0xC00
#endif
    .globl    __HeapBase
    .globl    __HeapLimit
__HeapBase:
    .if    Heap_Size
    .space    Heap_Size
    .endif
    .size __HeapBase, . - __HeapBase
__HeapLimit:
    .size __HeapLimit, . - __HeapLimit

    .section .isr_vector
    .align 2
    .globl __isr_vector
__isr_vector:
    .long    __StackTop            /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */
    .long    NMI_Handler           /* NMI Handler */
    .long    HardFault_Handler     /* Hard Fault Handler */
    .long    MemManage_Handler     /* MPU Fault Handler */
    .long    BusFault_Handler      /* Bus Fault Handler */
    .long    UsageFault_Handler    /* Usage Fault Handler */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    SVC_Handler           /* SVCall Handler */
    .long    DebugMon_Handler      /* Debug Monitor Handler */
    .long    0                     /* Reserved */
    .long    PendSV_Handler        /* PendSV Handler */
    .long    SysTick_Handler       /* SysTick Handler */

                // External Interrupts
    .long    WWDG_IRQHandler                   // Window WatchDog
    .long    PVD_IRQHandler                    // PVD through EXTI Line detection
    .long    TAMPER_STAMP_IRQHandler           // Tamper and TimeStamps through the EXTI line
    .long    RTC_WKUP_IRQHandler               // RTC Wakeup through the EXTI line
    .long    FLASH_IRQHandler                  // FLASH
    .long    RCC_IRQHandler                    // RCC
    .long    EXTI0_IRQHandler                  // EXTI Line0
    .long    EXTI1_IRQHandler                  // EXTI Line1
    .long    EXTI2_TS_IRQHandler               // EXTI Line2 and Touch Sense
    .long    EXTI3_IRQHandler                  // EXTI Line3
    .long    EXTI4_IRQHandler                  // EXTI Line4
    .long    DMA1_Channel1_IRQHandler          // DMA1 Channel 1
    .long    DMA1_Channel2_IRQHandler          // DMA1 Channel 2
    .long    DMA1_Channel3_IRQHandler          // DMA1 Channel 3
    .long    DMA1_Channel4_IRQHandler          // DMA1 Channel 4
    .long    DMA1_Channel5_IRQHandler          // DMA1 Channel 5
    .long    DMA1_Channel6_IRQHandler          // DMA1 Channel 6
    .long    DMA1_Channel7_IRQHandler          // DMA1 Channel 7
    .long    ADC1_IRQHandler                   // ADC1
    .long    CAN1_TX_IRQHandler                // CAN1 TX
    .long    CAN1_RX0_IRQHandler               // CAN1 RX0
    .long    CAN1_RX1_IRQHandler               // CAN1 RX1
    .long    CAN1_SCE_IRQHandler               // CAN1 SCE
    .long    EXTI9_5_IRQHandler                // External Line[9:5]s
    .long    TIM15_IRQHandler                  // TIM15
    .long    TIM16_IRQHandler                  // TIM16
    .long    TIM17_IRQHandler                  // TIM17
    .long    TIM18_DAC2_IRQHandler             // TIM18 and DAC2
    .long    TIM2_IRQHandler                   // TIM2
    .long    TIM3_IRQHandler                   // TIM3
    .long    TIM4_IRQHandler                   // TIM4
    .long    I2C1_EV_IRQHandler                // I2C1 Event
    .long    I2C1_ER_IRQHandler                // I2C1 Error
    .long    I2C2_EV_IRQHandler                // I2C2 Event
    .long    I2C2_ER_IRQHandler                // I2C2 Error
    .long    SPI1_IRQHandler                   // SPI1
    .long    SPI2_IRQHandler                   // SPI2
    .long    USART1_IRQHandler                 // USART1
    .long    USART2_IRQHandler                 // USART2
    .long    USART3_IRQHandler                 // USART3
    .long    EXTI15_10_IRQHandler              // External Line[15:10]s
    .long    RTC_Alarm_IRQHandler              // RTC Alarm (A and B) through EXTI Line
    .long    CEC_IRQHandler                    // CEC
    .long    TIM12_IRQHandler                  // TIM12
    .long    TIM13_IRQHandler                  // TIM13
    .long    TIM14_IRQHandler                  // TIM14
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    TIM5_IRQHandler                   // TIM5
    .long    SPI3_IRQHandler                   // SPI3
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    TIM6_DAC1_IRQHandler              // TIM6 and DAC1 Channel1 & channel2
    .long    TIM7_IRQHandler                   // TIM7
    .long    DMA2_Channel1_IRQHandler          // DMA2 Channel 1
    .long    DMA2_Channel2_IRQHandler          // DMA2 Channel 2
    .long    DMA2_Channel3_IRQHandler          // DMA2 Channel 3
    .long    DMA2_Channel4_IRQHandler          // DMA2 Channel 4
    .long    DMA2_Channel5_IRQHandler          // DMA2 Channel 5
    .long    SDADC1_IRQHandler                 // SDADC1
    .long    SDADC2_IRQHandler                 // SDADC2
    .long    SDADC3_IRQHandler                 // SDADC3
    .long    COMP_IRQHandler                   // COMP
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    USB_HP_IRQHandler                 // USB High Priority
    .long    USB_LP_IRQHandler                 // USB Low Priority
    .long    USBWakeUp_IRQHandler               // USB Wakeup
    .long    0                                 // Reserved
    .long    TIM19_IRQHandler                  // TIM19
    .long    0                                 // Reserved
    .long    0                                 // Reserved
    .long    FPU_IRQHandler                    // FPU

    .size    __isr_vector, . - __isr_vector

    .text
    .thumb
    .thumb_func
    .align 2
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

#if 1
/* Here are two copies of loop implemenations. First one favors code size
 * and the second one favors performance. Default uses the first one.
 * Change to "#if 0" to use the second one */
.flash_to_ram_loop:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .flash_to_ram_loop
#else
    subs    r3, r2
    ble    .flash_to_ram_loop_end
.flash_to_ram_loop:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .flash_to_ram_loop
.flash_to_ram_loop_end:
#endif

#ifndef __NO_SYSTEM_INIT
    ldr    r0, =SystemInit
    blx    r0
#endif

    ldr    r0, =_start
    bx    r0
    .pool
    .size Reset_Handler, . - Reset_Handler

/* Our weak _start alternative if we don't use the library _start
 * The zero init section must be cleared, otherwise the librtary is
 * doing that */
    .align 1
    .thumb_func
    .weak _start
    .type _start, %function
_start:

    /* Zero fill the bss segment. */
    ldr   r1, = __bss_start__
    ldr   r2, = __bss_end__
    movs  r3, #0
    b  .fill_zero_bss
.loop_zero_bss:
    str  r3, [r1], #4

.fill_zero_bss:
    cmp  r1, r2
    bcc  .loop_zero_bss

    /* Jump to our main */
    bl main
    b .
    .size    _start, . - _start

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro    def_irq_handler    handler_name
    .align 1
    .thumb_func
    .weak    \handler_name
    .type    \handler_name, %function
\handler_name :
    b    .
    .size    \handler_name, . - \handler_name
    .endm

    def_irq_handler    NMI_Handler
    def_irq_handler    HardFault_Handler
    def_irq_handler    MemManage_Handler
    def_irq_handler    BusFault_Handler
    def_irq_handler    UsageFault_Handler
    def_irq_handler    SVC_Handler
    def_irq_handler    DebugMon_Handler
    def_irq_handler    PendSV_Handler
    def_irq_handler    SysTick_Handler
    def_irq_handler    Default_Handler

    // External Interrupts
    def_irq_handler    WWDG_IRQHandler                   // Window WatchDog
    def_irq_handler    PVD_IRQHandler                    // PVD through EXTI Line detection
    def_irq_handler    TAMPER_STAMP_IRQHandler           // Tamper and TimeStamps through the EXTI line
    def_irq_handler    RTC_WKUP_IRQHandler               // RTC Wakeup through the EXTI line
    def_irq_handler    FLASH_IRQHandler                  // FLASH
    def_irq_handler    RCC_IRQHandler                    // RCC
    def_irq_handler    EXTI0_IRQHandler                  // EXTI Line0
    def_irq_handler    EXTI1_IRQHandler                  // EXTI Line1
    def_irq_handler    EXTI2_TS_IRQHandler               // EXTI Line2 and Touch Sense
    def_irq_handler    EXTI3_IRQHandler                  // EXTI Line3
    def_irq_handler    EXTI4_IRQHandler                  // EXTI Line4
    def_irq_handler    DMA1_Channel1_IRQHandler          // DMA1 Channel 1
    def_irq_handler    DMA1_Channel2_IRQHandler          // DMA1 Channel 2
    def_irq_handler    DMA1_Channel3_IRQHandler          // DMA1 Channel 3
    def_irq_handler    DMA1_Channel4_IRQHandler          // DMA1 Channel 4
    def_irq_handler    DMA1_Channel5_IRQHandler          // DMA1 Channel 5
    def_irq_handler    DMA1_Channel6_IRQHandler          // DMA1 Channel 6
    def_irq_handler    DMA1_Channel7_IRQHandler          // DMA1 Channel 7
    def_irq_handler    ADC1_IRQHandler                   // ADC1
    def_irq_handler    CAN1_TX_IRQHandler                // CAN1 TX
    def_irq_handler    CAN1_RX0_IRQHandler               // CAN1 RX0
    def_irq_handler    CAN1_RX1_IRQHandler               // CAN1 RX1
    def_irq_handler    CAN1_SCE_IRQHandler               // CAN1 SCE
    def_irq_handler    EXTI9_5_IRQHandler                // External Line[9:5]s
    def_irq_handler    TIM15_IRQHandler                  // TIM15
    def_irq_handler    TIM16_IRQHandler                  // TIM16
    def_irq_handler    TIM17_IRQHandler                  // TIM17
    def_irq_handler    TIM18_DAC2_IRQHandler             // TIM18 and DAC2
    def_irq_handler    TIM2_IRQHandler                   // TIM2
    def_irq_handler    TIM3_IRQHandler                   // TIM3
    def_irq_handler    TIM4_IRQHandler                   // TIM4
    def_irq_handler    I2C1_EV_IRQHandler                // I2C1 Event
    def_irq_handler    I2C1_ER_IRQHandler                // I2C1 Error
    def_irq_handler    I2C2_EV_IRQHandler                // I2C2 Event
    def_irq_handler    I2C2_ER_IRQHandler                // I2C2 Error
    def_irq_handler    SPI1_IRQHandler                   // SPI1
    def_irq_handler    SPI2_IRQHandler                   // SPI2
    def_irq_handler    USART1_IRQHandler                 // USART1
    def_irq_handler    USART2_IRQHandler                 // USART2
    def_irq_handler    USART3_IRQHandler                 // USART3
    def_irq_handler    EXTI15_10_IRQHandler              // External Line[15:10]s
    def_irq_handler    RTC_Alarm_IRQHandler              // RTC Alarm (A and B) through EXTI Line
    def_irq_handler    CEC_IRQHandler                    // CEC
    def_irq_handler    TIM12_IRQHandler                  // TIM12
    def_irq_handler    TIM13_IRQHandler                  // TIM13
    def_irq_handler    TIM14_IRQHandler                  // TIM14
    def_irq_handler    TIM5_IRQHandler                   // TIM5
    def_irq_handler    SPI3_IRQHandler                   // SPI3
    def_irq_handler    TIM6_DAC1_IRQHandler              // TIM6 and DAC1 Channel1 & channel2
    def_irq_handler    TIM7_IRQHandler                   // TIM7
    def_irq_handler    DMA2_Channel1_IRQHandler          // DMA2 Channel 1
    def_irq_handler    DMA2_Channel2_IRQHandler          // DMA2 Channel 2
    def_irq_handler    DMA2_Channel3_IRQHandler          // DMA2 Channel 3
    def_irq_handler    DMA2_Channel4_IRQHandler          // DMA2 Channel 4
    def_irq_handler    DMA2_Channel5_IRQHandler          // DMA2 Channel 5
    def_irq_handler    SDADC1_IRQHandler                 // SDADC1
    def_irq_handler    SDADC2_IRQHandler                 // SDADC2
    def_irq_handler    SDADC3_IRQHandler                 // SDADC3
    def_irq_handler    COMP_IRQHandler                   // COMP
    def_irq_handler    USB_HP_IRQHandler                 // USB High Priority
    def_irq_handler    USB_LP_IRQHandler                 // USB Low Priority
    def_irq_handler    USBWakeUp_IRQHandler              // USB Wakeup
    def_irq_handler    TIM19_IRQHandler                  // TIM19
    def_irq_handler    FPU_IRQHandler                    // FPU

    .end
