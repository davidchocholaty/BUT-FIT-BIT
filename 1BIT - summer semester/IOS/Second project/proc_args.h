#ifndef HEADER_PROC_ARGS_H
#define HEADER_PROC_ARGS_H

/***************************************************************************
 * 
 * Soubor: proc_args.h
 * Datum vytvoreni: 2021-04-25
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * Popis: Obsahuje definice kontant pro argumenty
 *        a strukturu reprezentujici argumenty programu
 * 
***************************************************************************/

#include <stdint.h>

/*******************************************************
 * 
 * Struktury
 * 
*******************************************************/

/*
 * Struktura reprezentujici argumenty programu.
 */
typedef struct
{
    unsigned int elves_cnt;
    unsigned int reindeers_cnt;
    unsigned int max_elf_time;
    unsigned int max_reindeer_time;    
} arg_t;

/*******************************************************
 * 
 * Funkce
 * 
*******************************************************/

uint8_t set_args (int argc, char **argv, arg_t *arg);

#endif /* HEADER_PROC_ARGS_H */