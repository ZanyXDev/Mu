#ifndef MEMORY_ACCESS_H
#define MEMORY_ACCESS_H

#include <stdint.h>

//address space
//new bank size (0x4000)
#define BANK_SCOOT 14
#define NUM_BANKS(areaSize) (((areaSize) >> BANK_SCOOT) + ((areaSize) & ((1 << BANK_SCOOT) - 1) ? 1 : 0))
#define START_BANK(address) ((address) >> BANK_SCOOT)
#define END_BANK(address, size) (START_BANK(address) + NUM_BANKS(size) - 1)
#define BANK_IN_RANGE(bank, address, size) ((bank) >= START_BANK(address) && (bank) <= END_BANK(address, size))
#define BANK_ADDRESS(bank) ((bank) << BANK_SCOOT)
#define TOTAL_MEMORY_BANKS (1 << (32 - BANK_SCOOT))//0x40000 banks for BANK_SCOOT = 14

//chip addresses and sizes
//after boot RAM is at 0x00000000,
//ROM is at 0x10000000
//and the SED1376 is at 0x1FF80000(+ 0x20000 for framebuffer)
#define EMUCS_START_ADDRESS 0xFFFC0000
#define REG_START_ADDRESS 0xFFFFF000
#define SUPERMASSIVE_RAM_SIZE (128 * 0x100000)//128mb RAM
#define RAM_SIZE (16 * 0x100000)//16mb RAM
#define ROM_SIZE (4 * 0x100000)//4mb ROM
#define EMUCS_SIZE 0x20000
#define REG_SIZE 0x1000//is actually 0xE00 without bootloader
#define BOOTLOADER_SIZE 0x200
#define SED1376_MR_BIT 0x20000

//the read/write stuff looks messy here but makes the memory access functions alot cleaner
#if defined(EMU_BIG_ENDIAN)
//memory layout is the same as the Palm m515, just cast to pointer and access, 32 bit accesses are split to prevent unaligned access issues
#define BUFFER_READ_8(segment, accessAddress, mask)  (*(uint8_t*)(segment + ((accessAddress) & (mask))))
#define BUFFER_READ_16(segment, accessAddress, mask) (*(uint16_t*)(segment + ((accessAddress) & (mask))))
#define BUFFER_READ_32(segment, accessAddress, mask) (*(uint16_t*)(segment + ((accessAddress) & (mask))) << 16 | *(uint16_t*)(segment + ((accessAddress) + 2 & (mask))))
#define BUFFER_WRITE_8(segment, accessAddress, mask, value)  (*(uint8_t*)(segment + ((accessAddress) & (mask))) = (value))
#define BUFFER_WRITE_16(segment, accessAddress, mask, value) (*(uint16_t*)(segment + ((accessAddress) & (mask))) = (value))
#define BUFFER_WRITE_32(segment, accessAddress, mask, value) (*(uint16_t*)(segment + ((accessAddress) & (mask))) = (value) >> 16 , *(uint16_t*)(segment + ((accessAddress) + 2 & (mask))) = (value) & 0xFFFF)
#define BUFFER_READ_8_BIG_ENDIAN  BUFFER_READ_8
#define BUFFER_READ_16_BIG_ENDIAN BUFFER_READ_16
#define BUFFER_READ_32_BIG_ENDIAN BUFFER_READ_32
#define BUFFER_WRITE_8_BIG_ENDIAN  BUFFER_WRITE_8
#define BUFFER_WRITE_16_BIG_ENDIAN BUFFER_WRITE_16
#define BUFFER_WRITE_32_BIG_ENDIAN BUFFER_WRITE_32
#else
//memory layout is different from the Palm m515, optimize for opcode fetches(16 bit reads)
#define BUFFER_READ_8(segment, accessAddress, mask)  (*(uint8_t*)(segment + ((accessAddress) & (mask) ^ 1)))
#define BUFFER_READ_16(segment, accessAddress, mask) (*(uint16_t*)(segment + ((accessAddress) & (mask))))
#define BUFFER_READ_32(segment, accessAddress, mask) (*(uint16_t*)(segment + ((accessAddress) & (mask))) << 16 | *(uint16_t*)(segment + ((accessAddress) + 2 & (mask))))
#define BUFFER_WRITE_8(segment, accessAddress, mask, value)  (*(uint8_t*)(segment + ((accessAddress) & (mask) ^ 1)) = (value))
#define BUFFER_WRITE_16(segment, accessAddress, mask, value) (*(uint16_t*)(segment + ((accessAddress) & (mask))) = (value))
#define BUFFER_WRITE_32(segment, accessAddress, mask, value) (*(uint16_t*)(segment + ((accessAddress) & (mask))) = (value) >> 16 , *(uint16_t*)(segment + ((accessAddress) + 2 & (mask))) = (value) & 0xFFFF)
#define BUFFER_READ_8_BIG_ENDIAN(segment, accessAddress, mask)  (segment[(accessAddress) & (mask)])
#define BUFFER_READ_16_BIG_ENDIAN(segment, accessAddress, mask) (segment[(accessAddress) & (mask)] << 8 | segment[(accessAddress) + 1 & (mask)])
#define BUFFER_READ_32_BIG_ENDIAN(segment, accessAddress, mask) (segment[(accessAddress) & (mask)] << 24 | segment[(accessAddress) + 1 & (mask)] << 16 | segment[(accessAddress) + 2 & (mask)] << 8 | segment[(accessAddress) + 3 & (mask)])
#define BUFFER_WRITE_8_BIG_ENDIAN(segment, accessAddress, mask, value)  (segment[(accessAddress) & (mask)] = (value))
#define BUFFER_WRITE_16_BIG_ENDIAN(segment, accessAddress, mask, value) (segment[(accessAddress) & (mask)] = (value) >> 8, segment[(accessAddress) + 1 & (mask)] = (value) & 0xFF)
#define BUFFER_WRITE_32_BIG_ENDIAN(segment, accessAddress, mask, value) (segment[(accessAddress) & (mask)] = (value) >> 24, segment[(accessAddress) + 1 & (mask)] = ((value) >> 16) & 0xFF, segment[(accessAddress) + 2 & (mask)] = ((value) >> 8) & 0xFF, segment[(accessAddress) + 3 & (mask)] = (value) & 0xFF)
#endif

#define SWAP_16(x) ((uint16_t)((((uint16_t)(x) & 0x00FF) << 8) | (((uint16_t)(x) & 0xFF00) >> 8)))
#define SWAP_32(x) ((uint32_t)((((uint32_t)(x) & 0x000000FF) << 24) | (((uint32_t)(x) & 0x0000FF00) <<  8) | (((uint32_t)(x) & 0x00FF0000) >>  8) | (((uint32_t)(x) & 0xFF000000) >> 24)))
#define SWAP_64(x) ((((uint64_t)(x) & UINT64_C(0x00000000000000FF)) << 56) | (((uint64_t)(x) & UINT64_C(0x000000000000FF00)) << 40) | (((uint64_t)(x) & UINT64_C(0x0000000000FF0000)) << 24) | (((uint64_t)(x) & UINT64_C(0x00000000FF000000)) << 8) | (((uint64_t)(x) & UINT64_C(0x000000FF00000000)) >> 8) | (((uint64_t)(x) & UINT64_C(0x0000FF0000000000)) >> 24) | (((uint64_t)(x) & UINT64_C(0x00FF000000000000)) >> 40) | (((uint64_t)(x) & UINT64_C(0xFF00000000000000)) >> 56))

extern uint8_t bankType[];

void setRegisterXXFFAccessMode(void);
void setRegisterFFFFAccessMode(void);
void setSed1376Attached(bool attached);
void resetAddressSpace(void);

#endif
