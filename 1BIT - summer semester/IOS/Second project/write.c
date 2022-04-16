/***************************************************************************
 * 
 * Soubor: write.c
 * Datum vytvoreni: 2021-04-25
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * Popis: Zajistuje vystup programu pro vypis do souboru a chybova hlaseni
 * 
***************************************************************************/

#include "main.h"
#include "mem_handler.h"
#include <stdint.h>
#include <stdio.h>

/*
 * Funkce slouzi pro vypis chyboveho hlaseni
 * na standardni chybovy vystup
 */
void print_err_msg (uint8_t err)
{
    /*
     * Chybove hlaseni pro jednotlive chybove stavy
     * Jejich poradi odpovida poradi konstant v enum error
     * v souboru main.h
     */
    const char *err_msg[] =
    {
        "Flawless program run.",
        "Wrong count of arguments.",
        "Wrong format of arguments.",
        "Wrong range of argument value.",
        "Memory allocation fault.",
        "Creating process fault.",
        "Creating semaphore fault",
        "Creating shared memory fault.",
        "Destroying semaphore fault.",
        "Freeing shared memory fault.",
        "Closing file fault.",
        "Wrong free code value.",
        "Unknown error."
    };

    /*
     * Hodnota chyby je vetsi nez hodnota posledni chyby
     * v enum (UNKNOWN_ERROR)
     * Nastaveni hodnoty chyby na neznamou chybu
     */
    if(err > UNKNOWN_ERR)
    {
        err = UNKNOWN_ERR;
    }
    
    fprintf(stderr, "Error: %s\n", err_msg[err]);
    return;
}

/*
 * Funkce zvysi sdilenou promennou counter o jedna
 * Sdilena promenna counter slouzi k cislovani vypisu
 */
void inc_counter (unsigned int *counter)
{
    (*counter)++;
    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze Santa jde spat
 */
void santa_go_sleep (FILE *fp, semaphore_t *sem, unsigned int *counter)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: Santa: going to sleep\n", *counter);
    fflush(fp);

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze Santa jde pomahat elfum
 */
void santa_help_elves (FILE *fp, semaphore_t *sem, unsigned int *counter)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: Santa: helping elves\n", *counter);
    fflush(fp);

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze Santa zavira dilnu
 */
void santa_close_workshop (FILE *fp, semaphore_t *sem, unsigned int *counter)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: Santa: closing workshop\n", *counter);
    fflush(fp);

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze Vanoce zacinaji
 * -> Santa uz zaprahl vsechny soby
 */
void santa_christmas_started (FILE *fp, semaphore_t *sem, unsigned int *counter)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: Santa: Christmas started\n", *counter);
    fflush(fp);

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze elf startuje
 */
void elf_started (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int elf_id)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: Elf %d: started\n", *counter, elf_id);
    fflush(fp); 

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze elf potrebuje pomoc
 */
void elf_need_help (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int elf_id)
{
    sem_wait(sem->write);


    inc_counter(counter);
    fprintf(fp, "%d: Elf %d: need help\n", *counter, elf_id);
    fflush(fp); 

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze elf dostal pomoc
 */
void elf_get_help (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int elf_id)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: Elf %d: get help\n", *counter, elf_id);
    fflush(fp); 

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze elf si bere dovolenou
 * -> na dverich dilny je napis Vanoce-zavreno
 */
void elf_taking_holidays (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int elf_id)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: Elf %d: taking holidays\n", *counter, elf_id);
    fflush(fp); 

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze sob startuje
 */
void reindeer_started (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int reindeer_id)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: RD %d: rstarted\n", *counter, reindeer_id);
    fflush(fp);

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze sob se vraci domu
 */
void reindeer_return_home (FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int reindeer_id)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: RD %d: return home\n", *counter, reindeer_id);
    fflush(fp);

    sem_post(sem->write);

    return;
}

/*
 * Funkce slouzi k vypisu do souboru
 * Vypisuje, ze sob je zaprazen
 */
void reindeer_get_hitched(FILE *fp, semaphore_t *sem, unsigned int *counter, unsigned int reindeer_id)
{
    sem_wait(sem->write);

    inc_counter(counter);
    fprintf(fp, "%d: RD %d: get hitched\n", *counter, reindeer_id);
    fflush(fp);

    sem_post(sem->write);

    return;
}

/************************** Konec souboru write.c *************************/