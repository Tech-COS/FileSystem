/*
** EPITECH PROJECT, 2024
** COS
** File description:
** syscall
*/

#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <stdbool.h>
#include <stdint.h>

extern void write_byte(uint16_t data[], uint64_t lba, uint16_t sectorcount);
extern void read_byte(uint16_t data[], uint64_t lba, uint16_t sectorcount);
extern void read_iso_byte(uint16_t data[], uint64_t lba, uint16_t sectorcount);
extern bool detect_drive(uint16_t data[]);
extern void detect_iso(uint16_t data[]);
void sleep(uint64_t time_paused_in_milliseconds);
void wait_for_ready(uint8_t flags, uint8_t flags_not_set);
void read_loop(uint16_t data[], uint16_t sectorcount);
bool detect_drive(uint16_t data[]);

#endif /* !SYSCALL_H_ */
