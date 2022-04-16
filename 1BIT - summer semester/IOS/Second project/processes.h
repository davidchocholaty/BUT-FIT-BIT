#ifndef HEADER_PROCESSES_H
#define HEADER_PROCESSES_H

/***************************************************************************
 * 
 * Soubor: processes.h
 * Datum vytvoreni: 2021-04-25
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * Popis: Obsahuje definici pro pocet skritku
 * 
***************************************************************************/

#include "mem_handler.h"
#include <stdio.h>

/*******************************************************
 * 
 * Funkce
 * 
*******************************************************/

uint8_t create_processes (semaphore_t *sem, shr_vars_t *shr_vars, arg_t *arg, FILE *fp);

#endif /* HEADER_PROCESSES_H */