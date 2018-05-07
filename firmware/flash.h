#ifndef FLASH_H
#define FLASH_H
#include "types.h"

void flash_write_page(u8 *dest, u8 *src, int count);
void flash_write_complete(void);
void flash_write_failed(void);

//Internal
void flash_idle();
void flash_unlock();
int flash_writing();
#endif
