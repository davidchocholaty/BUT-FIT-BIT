#ifndef HEADER_WRITE_H
#define HEADER_WRITE_H

/***************************************************************************
 * 
 * Soubor: write.h
 * Datum vytvoreni: 2021-04-25
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * 
***************************************************************************/

#include "mem_handler.h"
#include <stdio.h>

/*******************************************************
 * 
 * Funkce
 * 
*******************************************************/

void print_err_msg (uint8_t err);
void santa_go_sleep (FILE *fp, semaphore_t *sem, unsigned int *counter);
void santa_help_elves (FILE *fp, semaphore_t *sem, unsigned int *counter);
void santa_close_workshop (FILE *fp, semaphore_t *sem, unsigned int *counter);
void santa_christmas_started (FILE *fp, semaphore_t *sem, unsigned int *counter);
void elf_started (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int elf_id);
void elf_need_help (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int elf_id);
void elf_get_help (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int elf_id);
void elf_taking_holidays (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int elf_id);
void reindeer_started (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int reindeer_id);
void reindeer_return_home (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int reindeer_id);
void reindeer_get_hitched(FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int reindeer_id);

#endif /* HEADER_WRITE_H */