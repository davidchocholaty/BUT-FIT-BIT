/*
 * Tabuľka s rozptýlenými položkami
 *
 * S využitím dátových typov zo súboru hashtable.h a pripravených kostier
 * funkcií implementujte tabuľku s rozptýlenými položkami s explicitne
 * zreťazenými synonymami.
 *
 * Pri implementácii uvažujte veľkosť tabuľky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptyľovacia funkcia ktorá pridelí zadanému kľúču index z intervalu
 * <0,HT_SIZE-1>. Ideálna rozptyľovacia funkcia by mala rozprestrieť kľúče
 * rovnomerne po všetkých indexoch. Zamyslite sa nad kvalitou zvolenej funkcie.
 */
int get_hash(char *key) {
  int result = 1;
  int length = strlen(key);
  for (int i = 0; i < length; i++) {
    result += key[i];
  }
  return (result % HT_SIZE);
}

/*
 * Inicializácia tabuľky — zavolá sa pred prvým použitím tabuľky.
 */
void ht_init(ht_table_t *table) {   
  for (int i = 0; i < HT_SIZE; i++)
  {    
    (*table)[i] = NULL;
  }  
}

/*
 * Vyhľadanie prvku v tabuľke.
 *
 * V prípade úspechu vráti ukazovateľ na nájdený prvok; v opačnom prípade vráti
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key) {
  int idx = get_hash(key);  
  ht_item_t *elem = (*table)[idx];

  while (elem != NULL)
  {
    if (strcmp(key, elem->key) == 0)
    {
      return elem;
    }
    
    elem = elem->next;
  }
  
  return NULL;    
}

/*
 * Vloženie nového prvku do tabuľky.
 *
 * Pokiaľ prvok s daným kľúčom už v tabuľke existuje, nahraďte jeho hodnotu.
 *
 * Pri implementácii využite funkciu ht_search. Pri vkladaní prvku do zoznamu
 * synonym zvoľte najefektívnejšiu možnosť a vložte prvok na začiatok zoznamu.
 */
void ht_insert(ht_table_t *table, char *key, float value) {  
  int idx;
  ht_item_t *new_elem;
  ht_item_t *elem;
  
  elem = ht_search(table, key);

  if (elem != NULL)
  {    
    elem->value = value;
  }
  else
  {
    new_elem = (ht_item_t *) malloc(sizeof(ht_item_t));

    if (new_elem != NULL)
    {
      new_elem->key = key;
      new_elem->value = value;
      new_elem->next = NULL;

      idx = get_hash(key);
     
      elem = (*table)[idx];

      if (elem != NULL)
      {
        new_elem->next = elem;
      }  

      (*table)[idx] = new_elem;
    }  
  }
}

/*
 * Získanie hodnoty z tabuľky.
 *
 * V prípade úspechu vráti funkcia ukazovateľ na hodnotu prvku, v opačnom
 * prípade hodnotu NULL.
 *
 * Pri implementácii využite funkciu ht_search.
 */
float *ht_get(ht_table_t *table, char *key) {
  ht_item_t *elem = ht_search(table, key);

  if (elem != NULL)
  {
    return &(elem->value);
  }
      
  return NULL;
}

/*
 * Zmazanie prvku z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje priradené k danému prvku.
 * Pokiaľ prvok neexistuje, nerobte nič.
 *
 * Pri implementácii NEVYUŽÍVAJTE funkciu ht_search.
 */
void ht_delete(ht_table_t *table, char *key) {
  int idx = get_hash(key);
  ht_item_t *elem = (*table)[idx];
  ht_item_t *prev_elem = NULL;

  while (elem != NULL && strcmp(elem->key, key) != 0)
  {
    prev_elem = elem;
    elem = elem->next;        
  }
  
  if (elem != NULL)
  {
    if (prev_elem == NULL)
    {
      (*table)[idx] = elem->next;
    }
    else
    {
      prev_elem->next = elem->next;
    }
    
    free(elem);
  }
}

/*
 * Zmazanie všetkých prvkov z tabuľky.
 *
 * Funkcia korektne uvoľní všetky alokované zdroje a uvedie tabuľku do stavu po
 * inicializácii.
 */
void ht_delete_all(ht_table_t *table) {
  ht_item_t *elem = NULL;
  ht_item_t *del_elem = NULL;
  
  for (int i = 0; i < HT_SIZE; i++)
  {    
    elem = (*table)[i];

    while (elem != NULL)
    {
      del_elem = elem;
      elem = elem->next;
      free(del_elem);
    }
    
    (*table)[i] = NULL;
  }      
}
