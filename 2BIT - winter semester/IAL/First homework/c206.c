
/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
 * uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

int error_flag;
int solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error() {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = TRUE;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {
    list->firstElement = NULL;
    list->activeElement = NULL;
    list->lastElement = NULL;    
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {
    DLLElementPtr delete_tmp;
    DLLElementPtr previous_tmp;

    delete_tmp = list->lastElement;

    while (delete_tmp != NULL)    
    {
        previous_tmp = delete_tmp->previousElement;

        free(delete_tmp);

        delete_tmp = previous_tmp;
    }
    
    list->firstElement = NULL;
    list->activeElement = NULL;
    list->lastElement = NULL;    
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, int data ) {
    DLLElementPtr newElement = (DLLElementPtr) malloc(sizeof(struct DLLElement));
    
    /*
     * Test, zda operace malloc proběhla úspěšně.
     */ 
    if(newElement == NULL)
    {
        DLL_Error();
    }
    else
    {
        /*
         * Nastavení hodnoty nového prvního prvku.
         */ 
        newElement->data = data;

        /*
         * Nastavení ukazatelů předchozího a následujícího prvku 
         * nového prvního prvku.
         */ 
        newElement->previousElement = NULL;
        newElement->nextElement = list->firstElement;    

        /*
         * Test, zda seznam již obsahuje první prvek.
         */ 
        if(list->firstElement != NULL)
        {
            list->firstElement->previousElement = newElement;
        }
        else
        {
            /*
             * Seznam je prázdný a jedná se tedy o vkládání prnvího prvku
             * do prázdného seznamu.
             */ 
            list->lastElement = newElement;
        }

        /*
         * Nastavení prvního prvku seznamu na nově vytvořený prvek.
         */ 
        list->firstElement = newElement;
        
    }    
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, int data ) {
    DLLElementPtr newElement = (DLLElementPtr) malloc(sizeof(struct DLLElement));
    
    /*
     * Test, zda operace malloc proběhla úspěšně.
     */ 
    if(newElement == NULL)
    {
        DLL_Error();
    }
    else
    {
        /*
         * Nastavení hodnoty nového prvního prvku.
         */ 
        newElement->data = data;

        /*
         * Nastavení ukazatelů předchozího a následujícího prvku 
         * nového prvního prvku.
         */ 
        newElement->previousElement = list->lastElement;
        newElement->nextElement = NULL;    

        /*
         * Test, zda seznam již obsahuje poslední prvek.
         */ 
        if(list->lastElement != NULL)
        {
            list->lastElement->nextElement = newElement;
        }
        else
        {
            /*
             * Seznam je prázdný a jedná se tedy o vkládání prvního prvku
             * do prázdného seznamu.
             */ 
            list->firstElement = newElement;
        }

        /*
         * Nastavení posledního prvku seznamu na nově vytvořený prvek.
         */ 
        list->lastElement = newElement;
    }    
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz (nepočítáme-li return),
 * aniž byste testovali, zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {
    list->activeElement = list->firstElement;
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz (nepočítáme-li return),
 * aniž byste testovali, zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {    
    list->activeElement = list->lastElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, int *dataPtr ) {
    /*
     * Test, zda seznam je seznam prázdný.
     */ 
    if(list->firstElement == NULL)
    {
        DLL_Error();
    }
    else
    {
        /*
         * Přiřazení hodnoty prvního prvku seznamu.
         */ 
        *dataPtr = list->firstElement->data;
    }      
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, int *dataPtr ) {
    /*
     * Test, zda seznam je seznam prázdný.
     */ 
    if(list->lastElement == NULL)
    {
        DLL_Error();
    }
    else
    {
        /*
         * Přiřazení hodnoty posledního prvku seznamu.
         */ 
        *dataPtr = list->lastElement->data;
    }  
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {
    DLLElementPtr tmp;

    /*
     * Test, zda seznam není prázdný.
     */ 
    if(list->firstElement != NULL)
    {
        /*
         * Uchování ukazatele na první prvek seznamu.
         */ 
        tmp = list->firstElement;

        /*
         * Test, zda daný prvek není aktivní.
         */ 
        if(list->activeElement == list->firstElement)
        {
            /*
             * Rušení aktivity.
             */ 
            list->activeElement = NULL;
        }

        /*
         * Test, zda seznam neobsahuje pouze jeden prvek.
         * V takovém případě po jeho smazání bude seznam prázdný
         * a bude se jednat o mazání posledního prvku seznamu.
         */ 
        if(list->firstElement == list->lastElement)
        {
            list->firstElement = NULL;
            list->lastElement = NULL;
        }
        else
        {
            /*
             * Seznam obsahuje více jak jeden prvek.
             * Proběhne tedy nastavení nového prvního prvku seznamu
             * a nastavení jeho ukazatele na předchozí prvek na NULL.
             */ 
            list->firstElement = list->firstElement->nextElement;
            list->firstElement->previousElement = NULL;
        }
        
        free(tmp);        
    }    
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {
    DLLElementPtr tmp;

    /*
     * Test, zda seznam není prázdný.
     */ 
    if(list->lastElement != NULL)
    {
        /*
         * Uchování ukazatele na poslední prvek seznamu.
         */ 
        tmp = list->lastElement;

        /*
         * Test, zda daný prvek není aktivní.
         */ 
        if(list->activeElement == list->lastElement)
        {
            /*
             * Rušení aktivity.
             */ 
            list->activeElement = NULL;
        }

        /*
         * Test, zda seznam neobsahuje pouze jeden prvek.
         * V takovém případě po jeho smazání bude seznam prázdný
         * a bude se jednat o mazání posledního prvku seznamu.
         */ 
        if(list->firstElement == list->lastElement)
        {
            list->firstElement = NULL;
            list->lastElement = NULL;
        }
        else
        {
            /*
             * Seznam obsahuje více jak jeden prvek.
             * Proběhne tedy nastavení nového posledního prvku seznamu
             * a nastavení jeho ukazatele na následující prvek na NULL.
             */ 
            list->lastElement = list->lastElement->previousElement;
            list->lastElement->nextElement = NULL;
        }
        
        free(tmp);        
    }        
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {
    DLLElementPtr tmp;

    /*
     * Test, zda je seznam list aktivní a zároveň zda aktivní prvek není 
     * posledním prvkem seznamu.
     */ 
    if(list->activeElement != NULL &&        
       list->activeElement->nextElement != NULL)
    {
        tmp = list->activeElement->nextElement;
        
        /*
         * Nastavení následujícího prvku pro aktivní prvek.
         * V případě, že mazaný prvek je posledním prvkem seznamu
         * bude tento ukazatel nastaven na NULL, jinak bude nastaven
         * na následující prvek za mazaným prvkem.
         */         
        list->activeElement->nextElement = tmp->nextElement;

        /*
         * Test, zda mazaný prvek není posledním prvekm seznamu.
         */ 
        if(list->lastElement == tmp)
        {
            /*
             * Aktivní prvek bude posledním prvkem seznamu.
             */ 
            list->lastElement = list->activeElement;
        }
        else
        {
            /*
             * Nastavení ukazatele na předchozí prvek pro prvek následující
             * za mazaným prvkem. Tento ukazatel tedy bude nastaven na 
             * aktivní prvek.            
             */ 
            tmp->nextElement->previousElement = list->activeElement;
        }

        free(tmp);
    }
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {
    DLLElementPtr tmp;

    /*
     * Test, zda je seznam list aktivní a zároveň zda aktivní prvek není 
     * prvním prvkem seznamu.
     */ 
    if(list->activeElement != NULL &&        
       list->activeElement->previousElement != NULL)
    {
        tmp = list->activeElement->previousElement;
        
        /*
         * Nastavení předchozího prvku pro aktivní prvek.
         * V případě, že mazaný prvek je prvním prvkem seznamu
         * bude tento ukazatel nastaven na NULL, jinak bude nastaven
         * na předchozí prvek před mazaným prvkem.
         */         
        list->activeElement->previousElement = tmp->previousElement;

        /*
         * Test, zda mazaný prvek není prvním prvekm seznamu.
         */ 
        if(list->firstElement == tmp)
        {
            /*
             * Aktivní prvek bude prvním prvkem seznamu.
             */ 
            list->firstElement = list->activeElement;
        }
        else
        {
            /*
             * Nastavení ukazatele na následující prvek pro prvek předchozí
             * před mazaným prvkem. Tento ukazatel tedy bude nastaven na 
             * aktivní prvek.            
             */ 
            tmp->previousElement->nextElement = list->activeElement;
        }

        free(tmp);
    }
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, int data ) {
    DLLElementPtr newElement;

    /*
     * Test, zda je seznam list aktivní.
     */ 
    if(list->activeElement != NULL)
    {
        newElement = (DLLElementPtr) malloc(sizeof(struct DLLElement));

        /*
         * Test, zda operace malloc proběhla úspěšně.
         */ 
        if (newElement == NULL)
        {
            DLL_Error();
        }
        else
        {
            /*
             * Nastavení hodnoty nového prvku.
             */ 
            newElement->data = data;

            /*
             * Nastavení ukazatelů předchozího a následujícího prvku nového prvku.
             */
            newElement->nextElement = list->activeElement->nextElement;
            newElement->previousElement = list->activeElement;

            /*
             * Nastavení následujícího prvku pro aktivní prvek.             
             */ 
            list->activeElement->nextElement = newElement;

            /*
             * Test, zda aktivní prvek není posledním prvkem seznamu.
             * Pokud ano, bude nově vložený prvek posledním prvkem seznamu.
             */ 
            if(list->lastElement == list->activeElement)
            {
                list->lastElement = newElement;
            }
            else
            {
                /*
                 * Nastavení předchozího prvku pro prvek následující 
                 * za nově vkládaným prvkem.                 
                 */ 
                newElement->nextElement->previousElement = newElement;
            }
        }
    }        
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, int data ) {
    DLLElementPtr newElement;

    /*
     * Test, zda je seznam list aktivní.
     */ 
    if(list->activeElement != NULL)
    {
        newElement = (DLLElementPtr) malloc(sizeof(struct DLLElement));

        /*
         * Test, zda operace malloc proběhla úspěšně.
         */ 
        if (newElement == NULL)
        {
            DLL_Error();
        }
        else
        {
            /*
             * Nastavení hodnoty nového prvku.
             */ 
            newElement->data = data;

            /*
             * Nastavení ukazatelů předchozího a následujícího prvku nového prvku.
             */
            newElement->nextElement = list->activeElement;
            newElement->previousElement = list->activeElement->previousElement;

            /*
             * Nastavení předchozího prvku pro aktivní prvek.             
             */ 
            list->activeElement->previousElement = newElement;

            /*
             * Test, zda aktivní prvek není prvním prvkem seznamu.
             * Pokud ano, bude nově vložený prvek prvním prvkem seznamu.
             */ 
            if(list->firstElement == list->activeElement)
            {
                list->firstElement = newElement;
            }
            else
            {
                /*
                 * Nastavení následujícího prvku pro prvek předchozí 
                 * před nově vkládaným prvkem.                 
                 */ 
                newElement->previousElement->nextElement = newElement;
            }
        }
    }            
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, int *dataPtr ) {
    /*
     * Test na aktivitu - zda seznam není aktivní (pro ošetření chybového stavu).
     */ 
    if(list->activeElement == NULL)
    {
        DLL_Error();
    }
    else
    {
        /*
         * Přiřazení hodnoty aktivního prvku seznamu.
         */ 
        *dataPtr = list->activeElement->data;
    }    
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, int data ) {
    /*
     * Test na aktivitu.
     */ 
    if (list->activeElement != NULL)
    {
        /*
         * Nastavení hodnoty aktivního prvku na hodnotu proměnné data.         
         */ 
        list->activeElement->data = data;
    }     
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {
    /*
     * Test na aktivitu.
     */ 
    if (list->activeElement != NULL)
    {
        /*
         * Nastavení aktivního prvku na následující prvek pomocí ukazatele
         * previousElement.
         */ 
        list->activeElement = list->activeElement->nextElement;
    }        
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {
    /*
     * Test na aktivitu.
     */ 
    if (list->activeElement != NULL)
    {
        /*
         * Nastavení aktivního prvku na předchozí prvek pomocí ukazatele
         * previousElement.
         */ 
        list->activeElement = list->activeElement->previousElement;
    }    
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
int DLL_IsActive( DLList *list ) {
    /* 
     * Seznam je aktivní v případě, že ukazatel na aktivní element 
     * nemá hodnotu NULL. Pak je podmínka splněna a vrácena 
     * nenulová hodnota, jinak je vrácena hodnota 0.
     */ 
    return (list->activeElement != NULL);
}

/* Konec c206.c */
