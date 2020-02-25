typedef unsigned char uint8_t;
  
 /***GPIO D Ports***/
 #define GPIO_D 0x40020C00
 #define GPIO_D_MODER ((unsigned volatile int *)(GPIO_D))
 #define GPIO_D_OTYPER ((unsigned volatile short *)(GPIO_D+0x04))
 #define GPIO_D_OSPEEDR ((unsigned volatile int *)(GPIO_D+0x08))
 #define GPIO_D_PUPDR ((unsigned volatile int *)(GPIO_D+0x0C)) 
 
 #define GPIO_D_IDR_LOW ((unsigned volatile char *)(GPIO_D+0x10))
 #define GPIO_D_IDR_HIGH ((unsigned volatile char *)(GPIO_D+0x11))
 #define GPIO_D_ODR_LOW ((unsigned volatile char *)(GPIO_D+0x14))
 #define GPIO_D_ODR_HIGH ((unsigned volatile char *)(GPIO_D+0x15))
 
 /***GPIO E Ports***/
 #define GPIO_E 0x40021000
 #define GPIO_E_MODER ((unsigned volatile int *)(GPIO_E))
 #define GPIO_E_OTYPER ((unsigned volatile short *)(GPIO_E+0x04))
 #define GPIO_E_OSPEEDR ((unsigned volatile int *)(GPIO_E+0x08))
 #define GPIO_E_PUPDR ((unsigned volatile int *)(GPIO_E+0x0C))
  
 #define GPIO_E_IDR_LOW ((unsigned volatile char *)(GPIO_E+0x10))
 #define GPIO_E_IDR_HIGH ((unsigned volatile char *)(GPIO_E+0x11))
 #define GPIO_E_ODR_LOW ((unsigned volatile char *)(GPIO_E+0x14))
 #define GPIO_E_ODR_HIGH ((unsigned volatile char *)(GPIO_E+0x15))
 
 /***SysTick Port***/
 #define STK_CTRL ((volatile unsigned int *)(0xE000E010))
 #define STK_LOAD ((volatile unsigned int *)(0xE000E014))
 #define STK_VAL ((volatile unsigned int *)(0xE000E018))
 
  /***Graphic and Ascii display bits***/
 #define B_E 0x40
 #define B_RST 0x20
 #define B_CS2 0x10
 #define B_CS1 8
 #define B_SELECT 4
 #define B_RW 2
 #define B_RS 1

 /***LCD control bits***/
 #define LCD_ON 0x3F
 #define LCD_OFF 0x3E
 #define LCD_SET_ADD 0x40
 #define LCD_SET_PAGE 0xB8
 #define LCD_DISP_START 0xC0
 #define LCD_BUSY 0x80
 
 /***System Control Block Port***/
 #define SCB 0xE000ED00
 #define SCB_ICSR (volatile unsigned int *) (SCB+0x04)
 #define SCB_VTOR (volatile unsigned int *) (SCB+0x08)
 #define SCB_SHPR1 (volatile unsigned int *) (SCB+0x18)
 #define SCB_SHPR2 (volatile unsigned int *) (SCB+0x1C)
 #define SCB_SHPR3 (volatile unsigned int *) (SCB+0x20)
 #define SCB_SHCSR (volatile unsigned int *) (SCB+0x24)
 #define SCB_CPSR (volatile unsigned int *) (SCB+0x28)
 #define SCB_MMFSR (volatile unsigned int *) (SCB+0x28)
 #define SCB_UFSR (volatile unsigned int *) (SCB+0x2A)
 #define SCB_BFSR (volatile unsigned int *) (SCB+0x29)

 #define BIT_NMI_PEND_SET (1<<31) 
 #define BIT_ISR_PENDING (1<<22)
 #define BIT_USG_FAULT_ENA (1<<18)
 #define BIT_BUS_FAULT_ENA (1<<17)
 #define BIT_MEM_FAULT_ENA (1<<16)
 #define BIT_DIV_BY_0 (1<<25)
 #define BIT_DIV_BY_UNALIGNED (1<<24) // TODO
 
 /***System Configuration***/
 #define SYSCFG 0x40013800
 #define SYSCFG_BASE (volatile unsigned int *) (SYSCFG)
 #define SYSCFG_EXTICR1 (volatile unsigned int *) (SYSCFG+8)
 
 /***External Interrupt Ports***/
 #define EXTI 0x40013C00
 #define EXTI_IMR (volatile unsigned int *) (EXTI)
 #define EXTI_EMR (volatile unsigned int *) (EXTI+4)
 #define EXTI_RSTR (volatile unsigned int *) (EXTI+8)
 #define EXTI_FTSR (volatile unsigned int *) (EXTI+0xC)
 #define EXTI_SWEIR (volatile unsigned int *) (EXTI+0x10)
 #define EXTI_PR (volatile unsigned int *) (EXTI+0x14)

 /***Vector interrupts***/
 #define VECTOR_BASE 0x2001C000
 #define EXTI3_IRQVEC ((void (**)(void)) (VECTOR_BASE+0x64))
 #define EXTI2_IRQVEC ((void (**)(void)) (VECTOR_BASE+0x60))
 #define EXTI1_IRQVEC ((void (**)(void)) (VECTOR_BASE+0x5C))
 #define EXTI0_IRQVEC ((void (**)(void)) (VECTOR_BASE+0x58))
 
 #define EXTI3_IRQ_BPOS 0x4000
 #define EXTI2_IRQ_BPOS 0x0400
 #define EXTI1_IRQ_BPOS 0x0040
 #define EXTI0_IRQ_BPOS 0x0004
 
 /******/
 #define NVIC_ISER0 ((unsigned int *) 0xE000E100) 
 #define NVIC_EXTI3_IRQ_BPOS (1<<9)
 #define NVIC_EXTI2_IRQ_BPOS (1<<8)
 #define NVIC_EXTI1_IRQ_BPOS (1<<7)
 #define NVIC_EXTI0_IRQ_BPOS (1<<6)
 
