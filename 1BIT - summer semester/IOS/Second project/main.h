#ifndef HEADER_MAIN_H
#define HEADER_MAIN_H

/***************************************************************************
 * 
 * Soubor: main.h
 * Datum vytvoreni: 2021-04-25
 * Datum posledni zmeny: 2021-05-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Projekt: VUT FIT predmet IOS Projekt 2
 * Popis: Obsahuje chybove kody
 * 
***************************************************************************/

/*
 * Chybove kody, ktere mohou nastat behem behu programu.
 */
enum error
{
    NO_ERR,                   //< Bez chyby.
    WRONG_ARGS_CNT,           //< Nespravny pocet argumentu.
    WRONG_ARGS_FORMAT,        //< Nespravny format argumentu.
    WRONG_ARGS_RANGE,         //< Nespravny rozsah hodnoty argumentu.
    MALLOC_ERR,               //< Chyba alokaci pameti.
    FORK_ERR,                 //< Chyba pri vytvareni procesu. 
    SEM_ERR,                  //< Chyba pri vytvareni semaforu. 
    SHR_MEM_ERR,              //< Chyba pri vytvareni sdilene pameti.
    SEM_FREE_ERR,             //< Chyba pri ruseni semaforu.
    SHR_MEM_FREE_ERR,         //< Chyba pri ruseni sdilene pameti.
    CLOSE_FILE_ERR,           //< Chyba pri zavirani souboru.
    FREE_CODE_ERR,            //< Nespravna hodnota kodu free.
    UNKNOWN_ERR               //< Neznama chyba.
};

#endif /* HEADER_MAIN_H */