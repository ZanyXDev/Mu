#ifndef HARDWARE_REGISTERS_H
#define HARDWARE_REGISTERS_H

#include <stdint.h>
#include <stdbool.h>

//interrupt names
#define INT_EMIQ   0x00800000//level 7
#define INT_RTI    0x00400000//level 4
#define INT_SPI1   0x00200000//level 1<->6, configurable, datasheet is contraditory on this one
#define INT_IRQ5   0x00100000//level 5
#define INT_IRQ6   0x00080000//level 6
#define INT_IRQ3   0x00040000//level 3
#define INT_IRQ2   0x00020000//level 2
#define INT_IRQ1   0x00010000//level 1
#define INT_PWM2   0x00002000//level 1<->6, configurable
#define INT_UART2  0x00001000//level 1<->6, configurable
#define INT_INT3   0x00000800//level 4
#define INT_INT2   0x00000400//level 4
#define INT_INT1   0x00000200//level 4
#define INT_INT0   0x00000100//level 4
#define INT_PWM1   0x00000080//level 6
#define INT_KB     0x00000040//level 4
#define INT_TMR2   0x00000020//level 1<->6, configurable
#define INT_RTC    0x00000010//level 4
#define INT_WDT    0x00000008//level 4
#define INT_UART1  0x00000004//level 4
#define INT_TMR1   0x00000002//level 6
#define INT_SPI2   0x00000001//level 4

//reasons a timer is triggered
#define TIMER_REASON_SYSCLK 0x00
#define TIMER_REASON_TIN    0x01
#define TIMER_REASON_CLK32  0x02

//chip names
enum{
   CHIP_BEGIN = 0,
   CHIP_A0_ROM = 0,
   CHIP_A1_USB,
   CHIP_B0_SED,
   CHIP_B1_NIL,
   //CHIP_CX_RAM, //CSC* is owned by CSD during normal operation
   CHIP_DX_RAM,
   CHIP_00_EMU, //used for EMUCS on hardware, used by the emu registers here
   CHIP_REGISTERS,
   CHIP_NONE,
   CHIP_END
};

//types
typedef struct{
   bool     enable;
   uint32_t start;
   uint32_t lineSize;//the size of a single chip select line, multiply by 2 to get the range size for RAM
   uint32_t mask;//the address lines the chip responds to, so 0x10000 on an chip with 16 address lines will return the value at 0x0000

   //attributes
   bool     inBootMode;
   bool     readOnly;
   bool     readOnlyForProtectedMemory;
   bool     supervisorOnlyProtectedMemory;
   uint32_t unprotectedSize;
}chip_t;

//variables
extern chip_t   chips[];
extern int8_t   pllSleepWait;
extern int8_t   pllWakeWait;
extern uint32_t clk32Counter;
extern double   pctlrCpuClockDivider;
extern double   timerCycleCounter[];
extern uint16_t timerStatusReadAcknowledge[];
extern uint8_t  portDInterruptLastValue;
extern uint16_t spi1RxFifo[];
extern uint16_t spi1TxFifo[];
extern uint8_t  spi1RxReadPosition;
extern uint8_t  spi1RxWritePosition;
extern bool     spi1RxOverflowed;
extern uint8_t  spi1TxReadPosition;
extern uint8_t  spi1TxWritePosition;
extern int32_t  pwm1ClocksToNextSample;
extern uint8_t  pwm1Fifo[];
extern uint8_t  pwm1ReadPosition;
extern uint8_t  pwm1WritePosition;

//timing
void beginClk32(void);
void endClk32(void);
void addSysclks(double value);//only call between begin/endClk32

//CPU
bool pllIsOn(void);
bool backlightAmplifierState(void);
bool registersAreXXFFMapped(void);
bool sed1376ClockConnected(void);
void ads7846OverridePenState(bool value);
void refreshTouchState(void);//just refreshes the touchscreen
void refreshInputState(void);//refreshes touchscreen, buttons and docked status
//int32_t interruptAcknowledge(int32_t intLevel);//this is in m68kexternal.h

//memory errors
void setBusErrorTimeOut(uint32_t address, bool isWrite);
void setPrivilegeViolation(uint32_t address, bool isWrite);
void setWriteProtectViolation(uint32_t address);

//memory accessors
uint8_t getHwRegister8(uint32_t address);
uint16_t getHwRegister16(uint32_t address);
uint32_t getHwRegister32(uint32_t address);
void setHwRegister8(uint32_t address, uint8_t value);
void setHwRegister16(uint32_t address, uint16_t value);
void setHwRegister32(uint32_t address, uint32_t value);

//config
void resetHwRegisters(void);
void setRtc(uint16_t days, uint8_t hours, uint8_t minutes, uint8_t seconds);

#endif
