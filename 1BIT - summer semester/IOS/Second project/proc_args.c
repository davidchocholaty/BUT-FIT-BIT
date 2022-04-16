/***************************************************************************
 * 
 * Soubor: proc_args.c
 * Datum vytvoreni: 2021-04-25
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * Popis: Zajistuje praci s argumenty programu
 * 
***************************************************************************/

#include "main.h"
#include "proc_args.h"
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define ARGS_CNT 5             /* <- 4 vstupni parametry a nazev programu */
#define RANGE_LOW 0            /* <- Spodni hranice  */
#define RANGE_ELFS_CNT 1000    /* <- Hornice hranice poctu elfu */
#define RANGE_REINDEER_CNT 20  /* <- Horni hranice poctu sobu */
#define RANGE_TIME 1000        /* <- Horni hranice casu */
#define BASE 10                /* <- Baze pro funkci strtoul() */

/*
 * Funkce prevadi retezec (datovy typ char *) na celociselnou
 * hodnotu (datovy typ unsigned int)
 */
unsigned int to_unsigned(char *str)
{            
    return (strtoul(str, 0L, BASE));
}

/*
 * Overeni spravneho poctu argumentu
 */
bool valid_args_cnt (int argc)
{
    return (argc == ARGS_CNT);
}

/*
 * Overeni, zda argument je celociselna hodnota
 */
bool is_integer (char *arg)
{
    unsigned int str_len = strlen(arg);

    for (size_t i = 0; i < str_len; i++)
    {
        if (isdigit(arg[i]) == 0)
        {
            return false;
        }        
    }
    
    return true;
}

/*
 * Overeni, zda argumenty obsahuji pouze celociselne hodnoty
 */
bool valid_args_format (int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {        
        if (!is_integer(argv[i]))
        {            
            return false;
        }
    }
    
    return true;
}

/*
 * Overeni, zda jsou argumenty ve spravnem rozsahu
 * Neni nutne testovat, zda je cislo mensi nez RANGE_LOW (hodnota 0)
 * V takovem pripadu by se jednalo o cislo se znamenkem
 * a chyba by byla odchycena ve funkci valid_args_format()
 */
bool valid_args_range (arg_t *arg)
{
    unsigned int arg_val;    
    bool ret_val = true;

    /*
     * Prvni argument: 0 (RANGE_LOW) < NE < 1000 (RANGE_ELFS_CNT)
     */    
    arg_val = arg->elves_cnt;      
    if (arg_val == RANGE_LOW || arg_val >= RANGE_ELFS_CNT)
    {
        ret_val = false;     
    }

    /*
     * Druhy argument: 0 (RANGE_LOW) < NR < 20 (RANGE_REINDEER_CNT)
     */
    arg_val = arg->reindeers_cnt;        
    if (arg_val == RANGE_LOW || arg_val >= RANGE_REINDEER_CNT)
    {
        ret_val = false;
    }
    
    /*
     * Treti a ctvrty argument: 0 (RANGE_LOW) <= TE, RE <= 1000 (RANGE_TIME)
     */
    arg_val = arg->max_elf_time;        
    if (arg_val > RANGE_TIME)
    {
        ret_val = false;
    }

    /*
     * Ctvrty argument
     */        
    arg_val = arg->max_reindeer_time;
    if (arg_val > RANGE_TIME)
    {
        ret_val = false;
    }

    return ret_val;
}

/*
 * Funkce pro otestovani argumentu
 */
uint8_t valid_args (int argc, char **argv)
{
    /*
     * Pocet argumentu.
     */
    if (!valid_args_cnt(argc))
    {
        return WRONG_ARGS_CNT;       
    }

    /*
     * Format argumentu.
     */
    if (!valid_args_format(argc, argv))
    {
        return WRONG_ARGS_FORMAT;  
    }

    return NO_ERR;      
}

/*
 * Funkce nastavuje hodnoty argumentu
 */
uint8_t set_args_values (arg_t *arg, char **argv)
{
    uint8_t err = NO_ERR;

    arg->elves_cnt = to_unsigned(argv[1]);
    if (errno == ERANGE)
    {
        err = WRONG_ARGS_RANGE;    
    }

    arg->reindeers_cnt = to_unsigned(argv[2]);
    if (errno == ERANGE)
    {
        err = WRONG_ARGS_RANGE;
    }

    arg->max_elf_time = to_unsigned(argv[3]);
    if (errno == ERANGE)
    {
        err = WRONG_ARGS_RANGE;
    }

    arg->max_reindeer_time = to_unsigned(argv[4]);
    if (errno == ERANGE)
    {
        err = WRONG_ARGS_RANGE;
    }

    return err;
}

/*
 * Funkce pro nastaveni argumentu
 */
uint8_t set_args (int argc, char **argv, arg_t *arg)
{
    uint8_t err;

    err = valid_args(argc, argv);

    if (err == NO_ERR)
    {
        /*
         * Nastaveni hodnot argumentu
         */        
        err = set_args_values(arg, argv);       
        
        /*
         * Rozsah argumentu
         */
        if (err == NO_ERR && !valid_args_range(arg))
        {
            err = WRONG_ARGS_RANGE;     
        }
    }        

    return err;
}

/************************ Konec souboru proc_args.c ***********************/