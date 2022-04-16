/***************************************************************************
 * 
 * Soubor: main.c
 * Datum vytvoreni: 2021-04-17
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * Popis: Santa Claus problem
 * 
***************************************************************************/

#include "main.h"
#include "mem_handler.h"
#include "processes.h"
#include "proc_args.h"
#include "write.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Uvolneni pameti alokovane pomoci malloc */
#define FREE_MEM 1

/* Uvolneni pameti alokovane pomoci malloc a semaforu */
#define FREE_MEM_SEM 2

/* Uvolneni pameti alokovane pomoci malloc, semaforu a sdilene pameti */
#define FREE_MEM_SEM_SHR 3

/*
 * Funkce vytvari ukazatel na soubor proj2.out.
 */
FILE *create_file ()
{
    return fopen("proj2.out", "w");
}

/*
 * Funkce vykonava obsluhu uvolneni pameti pro:
 * - pamet alokovanou pomoci malloc (na heap)
 * - semafory
 * - sdilenou pamet
 */
uint8_t free_handler (unsigned int free_code,
                      arg_t *arg,
                      semaphore_t *sem,
                      shr_vars_t *shr_vars)
{
    uint8_t err = NO_ERR;

    if (free_code == FREE_MEM)
    {
        /* Uvolneni pameti alokovane pomoci malloc */
        free_mem(arg, sem, shr_vars);
    }
    else
    {
        if(free_code == FREE_MEM_SEM)
        {
            /* Uvolneni pameti alokovane pomoci malloc a semaforu */
            free_semaphores(sem);
            free_mem(arg, sem, shr_vars);    
        }
        else if(free_code == FREE_MEM_SEM_SHR) 
        {
            /* Uvolneni pameti alokovane pomoci malloc, semaforu a sdilene pameti */
            free_shr_mem(shr_vars);
            free_semaphores(sem);
            free_mem(arg, sem, shr_vars);        
        }
        else
        {
            /* Chybna hodnota promenne free_code */
            err = FREE_CODE_ERR;
        }
    }

    return err;
}

/*
 * Funkce vykonava obsluhu zachyceni chybovych stavu
 */
uint8_t error_handler (int argc, char **argv)
{
    arg_t *arg = NULL;    
    semaphore_t *sem = NULL;
    shr_vars_t *shr_vars = NULL;
    FILE *fp = NULL;
    uint8_t err = NO_ERR;
    
    /*
     * Zaalokovani pameti pro arg_t
     */
    arg = init_args();   

    if (arg == NULL)
    {
        err = MALLOC_ERR;
    }    

    if(err != NO_ERR)
    {
        free_handler(FREE_MEM, arg, sem, shr_vars);
        print_err_msg(err);
        return err;
    }   

    /*
     * Nastaveni argumentu
     */
    err = set_args(argc, argv, arg);
    
    /*
     * Nespravna hodnota, format nebo pocet parametru
     */
    if(err != NO_ERR)
    {
        free_handler(FREE_MEM, arg, sem, shr_vars);
        print_err_msg(err);
        return err;
    }            

    /*
     * Zaalokovani pameti pro semaphore_t
     */
    sem = init_semaphores();

    if(sem == NULL)
    {
        err = MALLOC_ERR;
    }

    if(err != NO_ERR)
    {        
        free_handler(FREE_MEM, arg, sem, shr_vars);        
        print_err_msg(err);
        return err;
    }


    /*
     * Vytvoreni semaforu
     */
    err = create_semaphores(sem);

    if(err != NO_ERR)
    {
        free_handler(FREE_MEM_SEM, arg, sem, shr_vars);          
        print_err_msg(err);
        return err;
    }

    /*
     * Zaalokovani pameti pro shr_vars_t
     */
    shr_vars = init_shr_vars();

    if(shr_vars == NULL)
    {
        err = MALLOC_ERR;
    }

    if(err != NO_ERR)
    {        
        free_handler(FREE_MEM_SEM, arg, sem, shr_vars);        
        print_err_msg(err);
        return err;
    }

    /*
     * Vytvoreni sdilene pameti
     */
    err = create_shr_mem(shr_vars);

    if(err != NO_ERR)
    {
        free_handler(FREE_MEM_SEM_SHR, arg, sem, shr_vars);        
        print_err_msg(err);
        return err;
    }

    /*
     * Nastaveni sdilenych promennych na pocatecni hodnotu
     */
    *(shr_vars->counter) = 0;
    *(shr_vars->elves) = 0;
    *(shr_vars->reindeers) = 0;
    *(shr_vars->closed) = false;

    /*
     * Vytvoreni souboru
     */
    fp = create_file();

    if(fp == NULL)
    {
        free_handler(FREE_MEM_SEM_SHR, arg, sem, shr_vars);
        print_err_msg(err);
        return err;
    }
    
    /*
     * Nastaveni bufferu pro fp - buffer neni pouzivan
     */
    setvbuf(fp, NULL, _IONBF, 0);

    /*
     * Vytvoreni procesu
     * Funkce create_processes() vola dalsi funkce
     * zajistujici synchronizaci procesu
     */
    err = create_processes(sem, shr_vars, arg, fp);    

    if (err != NO_ERR)
    {        
        fclose(fp);        
        free_handler(FREE_MEM_SEM_SHR, arg, sem, shr_vars);
        print_err_msg(err);
        return err;
    }            

    /*
     * Chyba pri uzavirani souboru
     */
    if (fclose(fp) == EOF)
    {        
        err = CLOSE_FILE_ERR;        
        free_handler(FREE_MEM_SEM_SHR, arg, sem, shr_vars);
        print_err_msg(err);
        return err;
    }    
    
    err = free_handler(FREE_MEM_SEM_SHR, arg, sem, shr_vars);    

    if(err != NO_ERR)
    {
        print_err_msg(err);
    }

    return err;
}

/*
 * Hlavni funkce main.
 */
int main (int argc, char **argv)
{    
    /*
     * V pripadu vyskytu chyby ve funkci error_handler pri prubehu
     * je vracena hodnota EXIT_FAILURE.
     * V opacnem pripadu EXIT_SUCCESS.
     */
    if(error_handler(argc, argv) != NO_ERR)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/************************** Konec souboru main.c **************************/