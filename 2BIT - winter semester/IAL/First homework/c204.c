/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického výrazu
** do postfixového tvaru. Pro převod využijte zásobník (Stack), který byl
** implementován v rámci příkladu c202. Bez správného vyřešení příkladu c202
** se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix ... konverzní funkce pro převod infixového výrazu
**                      na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**    
**    untilLeftPar ... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

int solved;

/**
 * Pomocná funkce untilLeftPar.
 * Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka bude
 * také odstraněna.
 * Pokud je zásobník prázdný, provádění funkce se ukončí.
 *
 * Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
 * znaků postfixExpression.
 * Délka převedeného výrazu a též ukazatel na první volné místo, na které se má
 * zapisovat, představuje parametr postfixExpressionLength.
 *
 * Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
 * nadeklarovat a používat pomocnou proměnnou typu char.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void untilLeftPar(Stack *stack, char *postfixExpression, unsigned *postfixExpressionLength ) {
    char stack_char;

    /*
     * Testování, zda není zásobník prázdný. Při splnění podmínky je
     * hodnota na vrcholu zásobníku uchována a odebrána ze zásobníku.
     */ 
    if(!Stack_IsEmpty(stack))
    {
        Stack_Top(stack, &stack_char);
        Stack_Pop(stack);

        /*
         * Jednotlivé znaky jsou odebírány ze zásobníku a vkládány do výstupního řetězce. 
         * Při nalezení levé závorky je i tento znak odstraněn ze zásobníku, jinak je zásobník
         * zcela vyprázdněn a všechny jeho prvky vloženy do výstupního řetězce. 
         */ 
        while (!Stack_IsEmpty(stack) && stack_char != '(')
        {                
            postfixExpression[(*postfixExpressionLength)++] = stack_char;

            Stack_Top(stack, &stack_char);
            Stack_Pop(stack);        
        }
    }            
}

/**
 * Pomocná funkce doOperation.
 * Zpracuje operátor, který je předán parametrem c po načtení znaku ze
 * vstupního pole znaků.
 *
 * Dle priority předaného operátoru a případně priority operátoru na vrcholu
 * zásobníku rozhodneme o dalším postupu.
 * Délka převedeného výrazu a taktéž ukazatel na první volné místo, do kterého
 * se má zapisovat, představuje parametr postfixExpressionLength, výstupním
 * polem znaků je opět postfixExpression.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param c Znak operátoru ve výrazu
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void doOperation( Stack *stack, char c, char *postfixExpression, unsigned *postfixExpressionLength ) {    
    char stackTop;
    int operatorPriority;
    int stackTopPriority;    

    /*
     * Přiřazení priority zadanému operátoru.     
     */ 
    switch (c)
    {
        case '+':
            operatorPriority = 0;
            break;    
        case '-':
            operatorPriority = 0;
            break;
        case '*':
            operatorPriority = 1;
            break;
        case '/':
            operatorPriority = 1;
            break;                
        default:
            break;
    }    
    
    if(!Stack_IsEmpty(stack))
    {        
        /*
         * Uložení hodnoty prvku na vrcholu zásobníku a přiřazení priority.         
         */
        Stack_Top(stack, &stackTop);
        
        switch (stackTop)
        {
            case '(':
                break;    
            case '+':
                stackTopPriority = 0;
                break;            
            case '-':
                stackTopPriority = 0;
                break;
            case '*':
                stackTopPriority = 1;
                break;
            case '/':
                stackTopPriority = 1;
                break;                
            default:
                stackTopPriority = -1;
                break;
        }        

        /*
         * V následujících případech bude znak z vrcholu zásobníku vložen do výstupního řetězce
         * a odstraněn ze zásobníku:
         * 
         * 1. Zásobník není prázdný
         * 2. Na vrcholu zásobníku není levá závorka
         * 3. Na vrcholu zásobníku není operátor s nižší prioritou
         * 
         * Tento scénář se opakuje dokud není jedna z nutných podmínek porušena. V takovém případě
         * je na zásobník vložen zadaný operátor.
         */
        while (!Stack_IsEmpty(stack) && 
                stackTop != '(' &&
                operatorPriority <= stackTopPriority)
        {          
            postfixExpression[(*postfixExpressionLength)++] = stackTop;        

            Stack_Pop(stack);      

            /*
             * Pokud nebude zásobník prázdný, bude hodnota prvku z vrcholu zásobníku uložena
             * a bude jí přiřazena priorita.
             */
            if(!Stack_IsEmpty(stack))
            {
                Stack_Top(stack, &stackTop);                
                
                switch (stackTop)
                {
                    case '(':
                        break; 
                    case '+':
                        stackTopPriority = 0;
                        break;            
                    case '-':
                        stackTopPriority = 0;
                        break;
                    case '*':
                        stackTopPriority = 1;
                        break;
                    case '/':
                        stackTopPriority = 1;
                        break;                
                    default:
                        stackTopPriority = -1;
                        break;
                }                
            }                              
        }        
    }           

    /*
     * Vložení zadaného operátoru na vrchol zásobníku.     
     */
    Stack_Push(stack, c);
}

/**
 * Konverzní funkce infix2postfix.
 * Čte infixový výraz ze vstupního řetězce infixExpression a generuje
 * odpovídající postfixový výraz do výstupního řetězce (postup převodu hledejte
 * v přednáškách nebo ve studijní opoře).
 * Paměť pro výstupní řetězec (o velikosti MAX_LEN) je třeba alokovat. Volající
 * funkce, tedy příjemce konvertovaného řetězce, zajistí korektní uvolnění zde
 * alokované paměti.
 *
 * Tvar výrazu:
 * 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
 *    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
 *    násobení má stejnou prioritu jako dělení. Priorita násobení je
 *    větší než priorita sčítání. Všechny operátory jsou binární
 *    (neuvažujte unární mínus).
 *
 * 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
 *    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
 *
 * 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
 *    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
 *    chybné zadání výrazu).
 *
 * 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen
 *    ukončovacím znakem '='.
 *
 * 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
 *
 * Poznámky k implementaci
 * -----------------------
 * Jako zásobník použijte zásobník znaků Stack implementovaný v příkladu c202.
 * Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
 *
 * Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
 *
 * Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
 * char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
 *
 * Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
 * nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
 * by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
 * ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
 * řetězce konstantu NULL.
 *
 * @param infixExpression Znakový řetězec obsahující infixový výraz k převedení
 *
 * @returns Znakový řetězec obsahující výsledný postfixový výraz
 */
char *infix2postfix( const char *infixExpression ) {    
    Stack *stack = (Stack *) malloc(sizeof(Stack));
    char *postfixExpression = (char *) malloc(MAX_LEN * sizeof(char));
    char *returnValue = NULL;
    unsigned i = 0;
    unsigned postfixExpressionLength = 0;                        

    if(stack != NULL && postfixExpression != NULL)
    {       
        Stack_Init(stack);

        /*
         * Na vstupu je očekáván pouze korektní zápis infixové notace 
         * končící znakem '='.
         */  
        while(infixExpression[i] != '=')
        { 
            /*
             * Operátor
             */                                       
            if(infixExpression[i] == '+' ||
               infixExpression[i] == '-' ||
               infixExpression[i] == '*' ||
               infixExpression[i] == '/')
            {
                doOperation(stack, 
                            infixExpression[i], 
                            postfixExpression, 
                            &postfixExpressionLength);
            }            
            else if(infixExpression[i] == '(')
            {
                Stack_Push(stack, '(');
            }            
            else if(infixExpression[i] == ')')
            {                
                untilLeftPar(stack, postfixExpression, &postfixExpressionLength);
            }
            else
            {
                /*
                 * Znak na vstupu je operand. Může nabývat jedné z hodnot: 0..9, a..z, A..Z.
                 */                 
                postfixExpression[postfixExpressionLength++] = infixExpression[i];                
            }

            i++;
        }

        /*         
         * Vyprázdnění zásobníku. Všechny znaky v něm uložené jsou vloženy do 
         * výstupního řetězce.
         */ 
        while(!Stack_IsEmpty(stack))
        {
            Stack_Top(stack, &postfixExpression[postfixExpressionLength++]);
            Stack_Pop(stack);            
        }

        /*
         * Vložení znaku '=' a nulového znaku na konec výstupního řetězce.
         */
        postfixExpression[postfixExpressionLength++] = '=';
        postfixExpression[postfixExpressionLength] = '\0';
        
        /*
         * Nastavení ukazatele na výstupní řetězce do proměnné obsluhující
         * návratovou hodnotu funkce.
         */
        returnValue = postfixExpression;
    }

    free(stack);

    return returnValue;
}

/* Konec c204.c */
