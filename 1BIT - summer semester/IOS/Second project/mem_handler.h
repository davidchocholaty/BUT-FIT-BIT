#ifndef HEADER_MEM_HANDLER_H
#define HEADER_MEM_HANDLER_H

/***************************************************************************
 * 
 * Soubor: mem_handler.h
 * Datum vytvoreni: 2021-04-25
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * Popis: Obsahuje strukturu reprezentujici semafory
 *        a strukturu reprezentujici sdilene promenne
 * 
***************************************************************************/

#include <semaphore.h>
#include <stdbool.h>
#include "proc_args.h"

/*******************************************************
 * 
 * Struktury
 * 
*******************************************************/

/*
 * Struktura reprezentujici semafory
 * zajistujici synchronizaci procesu.
 */
typedef struct 
{
    sem_t *mutex;
    sem_t *santa_mutex;
    sem_t *santa_sem;
    sem_t *elf_mutex;
    sem_t *elf_sem;
    sem_t *reindeer;
    sem_t *write;
} semaphore_t;

/*
 * Struktura reprezentujici sdilene promenne.
 */
typedef struct
{
    unsigned int *elves; //cnt
    unsigned int *reindeers; //cnt
    unsigned int *counter;
    bool *closed;    
} shr_vars_t;

/*******************************************************
 * 
 * Funkce
 * 
*******************************************************/

arg_t *init_args ();
semaphore_t *init_semaphores ();
shr_vars_t *init_shr_vars ();
uint8_t free_shr_mem (shr_vars_t *shr_vars);
uint8_t free_semaphores (semaphore_t *sem);
void free_mem (arg_t *arg, semaphore_t *sem, shr_vars_t *shr_vars);
uint8_t create_semaphores (semaphore_t *sem);
uint8_t create_shr_mem (shr_vars_t *shr_vars);

#endif /* HEADER_MEM_HANDLER_H */