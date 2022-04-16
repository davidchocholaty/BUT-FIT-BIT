/***************************************************************************
 * 
 * Soubor: mem_handler.c
 * Datum vytvoreni: 2021-04-25
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * Popis: Zajistuje praci s virtualni pameti pro semafory a sdilenou pamet
 * 
***************************************************************************/

#include "main.h"
#include "mem_handler.h"
#include "proc_args.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

/*
 * Funkce zaalokovava misto na heap pomoci malloc
 * pro strukturu arg_t
 */
arg_t *init_args ()
{
    return (arg_t *) malloc(sizeof(arg_t));
}

/*
 * Funkce zaalokovava misto na heap pomoci malloc
 * pro strukturu semaphore_t
 */
semaphore_t *init_semaphores ()
{
    return (semaphore_t *) malloc(sizeof(semaphore_t));
}

/*
 * Funkce zaalokovava misto na heap pomoci malloc
 * pro strukturu shr_vars_t
 */
shr_vars_t *init_shr_vars ()
{
    return (shr_vars_t *) malloc(sizeof(shr_vars_t));
}

/*
 * Funkce slouzi pro odstraneni semaforu
 */
uint8_t destroy_semaphores (semaphore_t *sem)
{
    uint8_t err = NO_ERR;

    if(sem_destroy(sem->mutex) == -1) err = SEM_FREE_ERR;    
    if(sem_destroy(sem->santa_mutex) == -1) err = SEM_FREE_ERR;    
    if(sem_destroy(sem->santa_sem) == -1) err = SEM_FREE_ERR;    
    if(sem_destroy(sem->elf_mutex) == -1) err = SEM_FREE_ERR;    
    if(sem_destroy(sem->elf_sem) == -1) err = SEM_FREE_ERR; 
    if(sem_destroy(sem->reindeer) == -1) err = SEM_FREE_ERR;    
    if(sem_destroy(sem->write) == -1) err = SEM_FREE_ERR;

    return err;
}

/*
 * Funkce slouzi pro odmapovani semaforu
 */
uint8_t unmap_semaphores (semaphore_t *sem)
{
    uint8_t err = NO_ERR;

    if(munmap(sem->mutex, sizeof(sem_t)) == -1) err = SEM_FREE_ERR;
    if(munmap(sem->santa_mutex, sizeof(sem_t)) == -1) err = SEM_FREE_ERR;    
    if(munmap(sem->santa_sem, sizeof(sem_t)) == -1) err = SEM_FREE_ERR; 
    if(munmap(sem->elf_mutex, sizeof(sem_t)) == -1) err = SEM_FREE_ERR;    
    if(munmap(sem->elf_sem, sizeof(sem_t)) == -1) err = SEM_FREE_ERR;  
    if(munmap(sem->reindeer, sizeof(sem_t)) == -1) err = SEM_FREE_ERR;    
    if(munmap(sem->write, sizeof(sem_t)) == -1) err = SEM_FREE_ERR;

    return err;
}

/*
 * Funkce slouzi pro uvolneni semaforu
 */
uint8_t free_semaphores (semaphore_t *sem)
{
    uint8_t err;

    err = destroy_semaphores(sem);

    if(err == NO_ERR)
    {
        err = unmap_semaphores(sem);
    }
    else
    {
        unmap_semaphores(sem);
    }    

    return err;
}

/*
 * Funkce slouzi pro odmapovani sdilene pameti
 */
uint8_t unmap_shr_mem (shr_vars_t *shr_vars)
{
    uint8_t err = NO_ERR;

    if(munmap(shr_vars->elves, sizeof(unsigned int)) == -1) err = SHR_MEM_FREE_ERR;
    if(munmap(shr_vars->reindeers, sizeof(unsigned int)) == -1) err = SHR_MEM_FREE_ERR;
    if(munmap(shr_vars->counter, sizeof(unsigned int)) == -1) err = SHR_MEM_FREE_ERR;
    if(munmap(shr_vars->closed, sizeof(bool)) == -1) err = SHR_MEM_FREE_ERR;

    return err;
}

/*
 * Funkce slouzi pro uvolneni sdilene pameti
 * Vola funkci unmap_shr_mem() zajistuji
 * odmapovani sdilene pameti
 */
uint8_t free_shr_mem (shr_vars_t *shr_vars)
{
    return unmap_shr_mem(shr_vars);
}

/*
 * Funkce uvolni alokovanou pamet
 */
void free_mem (arg_t *arg, semaphore_t *sem, shr_vars_t *shr_vars)
{
    free(arg);
    arg = NULL;

    free(sem);
    sem = NULL;

    free(shr_vars);
    shr_vars = NULL;

    return;
}

/*
 * Vytvoreni a namapovani pameti
 */
void *create_shr_mem_seg (size_t size)
{
    int protection = PROT_READ | PROT_WRITE;    
    int visibility = MAP_SHARED | MAP_ANONYMOUS;
    
    return mmap(NULL, size, protection, visibility, -1, 0);
}

/*******************************************************
 *
 * Semafory
 * 
*******************************************************/

/*
 * Namapovani semaforu
 */
uint8_t map_semaphores (semaphore_t *sem)
{
    size_t size = sizeof(sem_t);
    uint8_t err = NO_ERR;

    sem->mutex = create_shr_mem_seg(size);    
    if(sem->mutex == MAP_FAILED) err = SEM_ERR;

    sem->santa_mutex = create_shr_mem_seg(size);
    if(sem->santa_mutex == MAP_FAILED) err = SEM_ERR;

    sem->santa_sem = create_shr_mem_seg(size);
    if(sem->santa_sem == MAP_FAILED) err = SEM_ERR;

    sem->elf_mutex = create_shr_mem_seg(size);
    if(sem->elf_mutex == MAP_FAILED) err = SEM_ERR;

    sem->elf_sem = create_shr_mem_seg(size);
    if(sem->elf_sem == MAP_FAILED) err = SEM_ERR;

    sem->reindeer = create_shr_mem_seg(size);
    if(sem->reindeer == MAP_FAILED) err = SEM_ERR;

    sem->write = create_shr_mem_seg(size);
    if(sem->write == MAP_FAILED) err = SEM_ERR;

    return err;
}

/*
 * Nastaveni pocatecni hodnoty pro jednotlive semafory
 */
uint8_t init_mapped_semaphores (semaphore_t *sem)
{
    uint8_t err = NO_ERR;
    
    if (sem_init(sem->mutex, 1, 1) == -1) err = SEM_ERR;      
    if (sem_init(sem->santa_mutex, 1, 0) == -1) err = SEM_ERR;             
    if (sem_init(sem->santa_sem, 1, 0) == -1) err = SEM_ERR;   
    if (sem_init(sem->elf_mutex, 1, 1) == -1) err = SEM_ERR;            
    if (sem_init(sem->elf_sem, 1, 0) == -1) err = SEM_ERR;
    if (sem_init(sem->reindeer, 1, 0) == -1) err = SEM_ERR;    
    if (sem_init(sem->write, 1, 1) == -1) err = SEM_ERR;    

    return err;
}

/*
 * Funkce vytvari semafory
 * - nejprve namapuje pamet pro semafory
 * - pote je nastavi na pocatecni hodnotu
 */
uint8_t create_semaphores (semaphore_t *sem)
{
    uint8_t err;
    
    /* Namapovani semaforu */
    err = map_semaphores(sem);

    if(err == NO_ERR)
    {
        /* Nastaveni semaforu na pocatecni hodnotu */
        err = init_mapped_semaphores(sem);
    }    

    return err;
}

/*******************************************************
 * 
 * Sdilena pamet pro sdilene promenne
 * 
*******************************************************/

/*
 * Funkce namapuje pamet pro sdilene promenne
 */
uint8_t map_shr_mem (shr_vars_t *shr_vars)
{
    size_t size = sizeof(unsigned int);
    uint8_t err = NO_ERR;
    
    shr_vars->elves = create_shr_mem_seg(size);    
    if(shr_vars->elves == MAP_FAILED) err = SHR_MEM_ERR;

    shr_vars->reindeers = create_shr_mem_seg(size);    
    if(shr_vars->reindeers == MAP_FAILED) err = SHR_MEM_ERR;    

    shr_vars->counter = create_shr_mem_seg(size);    
    if(shr_vars->counter == MAP_FAILED) err = SHR_MEM_ERR;   

    shr_vars->closed = create_shr_mem_seg(size);    
    if(shr_vars->closed == MAP_FAILED) err = SHR_MEM_ERR;

    return err;
}

/*
 * Funkce vytvari sdilenou pamet
 */
uint8_t create_shr_mem (shr_vars_t *shr_vars)
{    
    return map_shr_mem(shr_vars);          
}

/*********************** Konec souboru mem_handler.c **********************/