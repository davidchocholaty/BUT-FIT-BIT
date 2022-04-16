/***************************************************************************
 * 
 * Soubor: processes.c
 * Datum vytvoreni: 2021-04-25
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * Popis: Zajistuje praci s procesy a jejich synchronizaci
 * 
***************************************************************************/

#include "main.h"
#include "mem_handler.h"
#include "processes.h"
#include "proc_args.h"
#include "write.h"
#include <time.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define ELVES_CNT 3 /* <- Pocet elfu, kteri mohou vzbudit Santu */

/*
 * Generovani nahodneho casu.
 */
unsigned int random_time(unsigned int range_low, unsigned int range_high)
{
    srand(time(0));
    return (rand() % (range_high - range_low + 1) + range_low) * 1000;
}

/*
 * Proces Santa
 */
void santa_proc (semaphore_t *sem,
                 shr_vars_t *shr_vars,
                 arg_t *arg,
                 FILE *fp)
{                
    unsigned int i;    

    /* Vypis, ze Santa jde spat */
    santa_go_sleep(fp, sem, shr_vars->counter);
    
    while (1)
    {    
        sem_wait(sem->santa_mutex);       
        sem_wait(sem->mutex);                

        if (*(shr_vars->reindeers) == arg->reindeers_cnt)
        {
            /* Vsichni sobi jsou zpet */
            break;
        }        
        else if (*(shr_vars->elves) == ELVES_CNT)
        {
            /* Pred dilnou cekaji uz tri elfove, kteri vzbudi Santu */
            santa_help_elves(fp, sem, shr_vars->counter);                              

            for (i = 0; i < ELVES_CNT; i++)            
            {
                sem_post(sem->elf_sem);                                
            }   

            sem_post(sem->mutex);
            
            sem_wait(sem->santa_sem);         
            sem_wait(sem->mutex);

            /*
             * Santa pomohl elfum
             * Vypis, ze Santa jde spat
             */
            santa_go_sleep(fp, sem, shr_vars->counter);            
        }        

        sem_post(sem->mutex);
    }

    /* Vypis, ze Santa zavira dilnu */
    santa_close_workshop(fp, sem, shr_vars->counter);
                                     
    *(shr_vars->closed) = true; 

    for(i = 0; i < arg->reindeers_cnt; i++)
    {
        sem_post(sem->reindeer);
    }        

    sem_post(sem->mutex);

    sem_post(sem->elf_sem);
    sem_post(sem->elf_mutex);   

    sem_wait(sem->santa_sem); 
            
    /*
     * Vsichni sobi jsou zaprazeni
     * Vypis Vanoce zacinaji
     */
    santa_christmas_started(fp, sem, shr_vars->counter);                                        

    exit(0);   
}

/*
 * Proces elf
 */
void elf_proc (semaphore_t *sem,
               shr_vars_t *shr_vars,
               arg_t *arg,
               unsigned int elf_id,
               FILE *fp)
{    
    /* Vypis, ze elf startuje */
    elf_started(fp, sem, shr_vars->counter, elf_id);    
    
    while (1)
    {        
        /* Elf pracuje */
        usleep(random_time(0, arg->max_elf_time));

        sem_wait(sem->elf_mutex);        
        sem_wait(sem->mutex);                        

        /* Vypis, ze elf potrebuje pomoc */
        elf_need_help(fp, sem, shr_vars->counter, elf_id);

        /* Elf chce pomoc, ale zjisti, ze je dilna zavrena */                
        if (*(shr_vars->closed))
        {            
            sem_post(sem->mutex);
            sem_post(sem->elf_mutex);                   
            break;                 
        }
         
        *(shr_vars->elves) = *(shr_vars->elves) + 1;        

        /* Pred dilnou cekaji uz tri elfove */
        if(*(shr_vars->elves) == ELVES_CNT)
        {                            
            sem_post(sem->santa_mutex);        
        }
        else
        {
            sem_post(sem->elf_mutex);
        }

        sem_post(sem->mutex);        
        sem_wait(sem->elf_sem);    

        /*
         * Vsichni elfove, co cekaji pred dilnou
         * ihned odchazeji na dovolenou
         */
        if (*(shr_vars->closed))
        {                        
            sem_post(sem->elf_sem);
            break;            
        }

        sem_wait(sem->mutex);    

        /* Vypis, ze elf dostane pomoc */
        elf_get_help(fp, sem, shr_vars->counter, elf_id);            

        *(shr_vars->elves) = *(shr_vars->elves) - 1;

        /* 
         * Posledni ze 3 elfu dostal pomoc
         * Odchazi z dilny a upozorni dalsiho skritka
         */
        if(*(shr_vars->elves) == 0)
        {                        
            sem_post(sem->santa_sem);
            sem_post(sem->elf_mutex);        
        }

        sem_post(sem->mutex);  
    }

    /* Elf si bere dovolenou */
    elf_taking_holidays(fp, sem, shr_vars->counter, elf_id);
    exit(0);                
}

/*
 * Proces sob
 */
void reindeer_proc (semaphore_t *sem,
                    shr_vars_t *shr_vars,
                    arg_t *arg,
                    unsigned int reindeer_id,
                    FILE *fp)
{  
    /* Vypis, ze sob startuje */   
    reindeer_started(fp, sem, shr_vars->counter, reindeer_id);
    
    /* Sob je na dovolene */
    usleep(random_time(arg->max_elf_time / 2, arg->max_elf_time));

    sem_wait(sem->mutex);              

    /* Vypis, ze sob se vraci domu z dovolene */
    reindeer_return_home(fp, sem, shr_vars->counter, reindeer_id);

    *(shr_vars->reindeers) = *(shr_vars->reindeers) + 1;   

    /* Vsichni sobi jsou zpatky z dovolene */
    if(*(shr_vars->reindeers) == arg->reindeers_cnt)
    {                
        sem_post(sem->santa_mutex);         
    }

    sem_post(sem->mutex);
    sem_wait(sem->reindeer);

    sem_wait(sem->mutex);    

    /* Vypis, ze sob je zaprazen */
    reindeer_get_hitched(fp, sem, shr_vars->counter, reindeer_id);  

    *(shr_vars->reindeers) = *(shr_vars->reindeers) - 1;

    /* Vsichni sobi jsou zaprazeni */
    if(*(shr_vars->reindeers) == 0)
    {               
        sem_post(sem->santa_sem);
    }

    sem_post(sem->mutex); 
       
    exit(0);
}

/*
 * Cekani na potomky
 */
void wait_for_children(unsigned int proc_cnt)
{
    for(unsigned int i = 0; i < proc_cnt; i++) wait(NULL); 
    return; 
}

/*
 * Vytvareni procesu
 */
uint8_t create_processes (semaphore_t *sem,
                          shr_vars_t *shr_vars,
                          arg_t *arg,
                          FILE *fp)
{
    pid_t pid;
    unsigned int elves_cnt = arg->elves_cnt; 
    unsigned int proc_cnt = elves_cnt + arg->reindeers_cnt + 1;
    
    for (unsigned int i = 0; i < proc_cnt; i++)
    {
        pid = fork();

        switch (pid)
        {
        case 0:
            /* Proces potomka */
            if (i == 0)
            {
                /* Proces Santa */
                santa_proc(sem, shr_vars, arg, fp);
            }
            else if (i <= elves_cnt)
            {
                /* Proces elf */
                elf_proc(sem, shr_vars, arg, i, fp);
            }
            else
            {
                /* Proces sob */
                reindeer_proc(sem, shr_vars, arg, i - elves_cnt, fp);
            }          

            break;
        
        case -1:
            /* Nastala chyba pri fork */
            
            kill(-1, SIGKILL); /* Uklizeni procesu */
            return FORK_ERR;

        default:
            /* Kod pro rodice */
            break;
        }      
    }
    
    wait_for_children(proc_cnt);          

    return NO_ERR;
}

/************************ Konec souboru processes.c ***********************/