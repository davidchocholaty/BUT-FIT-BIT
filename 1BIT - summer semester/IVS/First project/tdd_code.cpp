//======== Copyright (c) 2021, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     DAVID CHOCHOLATY <xchoch09@stud.fit.vutbr.cz>
// $Date:       $2021-02-21
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author DAVID CHOCHOLATY
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"


PriorityQueue::PriorityQueue()
{
}

PriorityQueue::~PriorityQueue()
{
    // Odstraneni cele fronty od jejiho zacatku.    
    while (Length() > 0)
    {
        // Odstraneni prvni polozky fronty.
        Remove(GetHead()->value);
    }            
}

void PriorityQueue::Insert(int value)
{
    Element_t *tmp = GetHead();
    Element_t *new_element = new Element_t;                
    
    new_element->value = value;        

    // Pripad vlozeni polozky na zacatek fronty (prazdne ci neprazdne).
    if (GetHead() == NULL ||
        tmp->value < new_element->value)
    {        
        // Pripad prazdne fronty.
        if(GetHead() == NULL)
        {
            //m_pHead->pNext = NULL;
            new_element->pNext = NULL;
        }
        else
        {
            // Pripad neprazdne fronty.
            new_element->pNext = tmp;   
        }

        m_pHead = new_element;        
    }    
    else
    {                       
        // Vlozeni polozky na jinou pozici, nez na zacatek fronty.                                                          
        while (tmp->pNext != NULL &&
               tmp->pNext->value > new_element->value)
        {
            tmp = tmp->pNext;
        }

        // Vlozeni polozky do fronty na prislusnou pozici.
        new_element->pNext = tmp->pNext;
        tmp->pNext = new_element;                       
    }    
}

bool PriorityQueue::Remove(int value)
{
    Element_t *tmp = GetHead();    
    
    if (tmp != NULL)
    {
        // Pripad, kdy ma byt odstranena prvni polozka fronty.
        if (tmp->value == value)
        {                        
            m_pHead = tmp->pNext;            
            delete tmp;

            return true;
        }        
        
        // Odstraneni jine, nez prvni polozky fronty.
        while (tmp->pNext != NULL &&
               tmp->pNext->value > value)
        {
            tmp = tmp->pNext;            
        }

        if (tmp->pNext != NULL &&
            tmp->pNext->value == value)
        {
            Element_t *next = tmp->pNext->pNext;
            delete tmp->pNext;                
            tmp->pNext = next;

            return true;        
        }          
    }        
	
    return false;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    Element_t *tmp = GetHead();    

    while (tmp != NULL && tmp->value > value)
    {
        tmp = tmp->pNext;        
    }

    /*
     * Pripad konce fronty nebo nenalezeni polozky
     * ve fronte na prislusne pozici. 
     * Pokud se nejedna o konec fronty, hodnota value polozky (tmp->value)
     * muze byt rovna nebo mensi hledane hodnote (value).    
     */ 
    if (tmp == NULL || tmp->value != value)
    {
        return NULL;
    }    

    return tmp;
}

size_t PriorityQueue::Length()
{
    Element_t *tmp = GetHead();
    size_t count = 0;

    while (tmp != NULL)
    {
        tmp = tmp->pNext;
        ++count;
    }
    
	return count;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return m_pHead;
}

/*** Konec souboru tdd_code.cpp ***/
