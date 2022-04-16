/*
 * Soubor: sps.c
 * Datum: 06.12.2020
 * Autor: David Chocholaty, xchoch09@stud.fit.vutbr.cz
 * Projekt: Prace s datovymi strukturami, 2. pro predmer IZP
 * Popis: Program implementujici operace tabulkovych procesoru.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>

#define DARGINDEX 1
#define DELIMARGINDEX 2
#define MINARGSCOUNT 3
#define MAXARGSCOUNT 5
#define SEQINDEXNODELIM 1
#define SEQINDEXNODELIM 1
#define SEQINDEXWITHDELIM 3
#define FILEINDEXNODELIM 2
#define FILEINDEXWITHDELIM 4
#define EMPTYQUEUE 0
#define ENDOFFILE false
#define NOTENDOFFILE true
#define STOPONERROR false
#define POSITIONOFSTRING 6
#define MAXFLOATINGPOINTSIZE 14
#define COUNTOFTMPVARIABLES 10
#define ARGUMENTVALUELENGTH 2
#define FUNCTION(function) {function, #function},

/*
 * Chybove kody, ktere mohou nastat behem programu.
 */
enum error
{
    NO_ERROR,                   //< Bez chyby.
    WRONG_ARGS_COUNT,           //< Nespravny pocet argumentu.
    WRONG_DELIM_FORMAT,         //< Nespravny format argumentu DELIM.
    WRONG_FILE_NAME,            //< Nespravny nazev souboru.
    FULL_HEAP,                  //< Vnitrni chyba programu - nedostatek pameti.
    WRONG_INSERT_COL_ARG,       //< Nespravne zadany argument vlozeni sloupce.
    WRONG_INSERT_ROW_ARG,       //< Nespravne zadany argument vlozeni radku.
    ERROR_DELETING_CELL,        //< Chyba pri mazani bunky tabulky - nespravny argument.
    ERROR_DELETING_ROW,         //< Chyba pri mazani radku tabulky - nespravny argument.
    NULL_TABLE_POINTER,         //< Chyba - nedefinovana vstupni tabulka.
    NULL_ROW_POINTER,           //< Chyba - nedefinovany radek.
    NULL_CELL_POINTER,          //< Chyba - nedefinovana bunka.
    ERROR_EOF,                  //< Chyba konce souboru.
    WRONG_SELECTION_ARG,        //< Chybny argument selekce.
    ERROR_FUNCTION_NAME,        //< Nespravny nazev funkce (operace).
    WRONG_ARGUMENTS_ORDER,      //< Nespravne poradi argumentu.
    WRONG_ARGUMENT_VALUE,       //< Nespravna hodnota funkce.
    NULL_SELECTION_POINTER,     //< Chyba selekce.
    UNKNOWN_ERROR               //< Neznama chyba.
};

/*
 * Struktura reprezentujici selekci v tabulce.
 */
typedef struct{
    unsigned int first_row;
    unsigned int second_row;
    unsigned int first_col;
    unsigned int second_col;
} selection_t;

/*
 * Struktura reprezentujici docasne promenne.
 */
typedef struct {
    selection_t tmp_var_selection;
    char **tmp_variables;
} tmp_var_t;

/*
 * Struktura reprezentujici argumenty zadane na prikazove radce.
 */
typedef struct{
    char *delims;
    char output_delim;
    char **cmd_sequence;
    char *file;
    char *command_value;
    tmp_var_t *tmp_var;
} argument_t;

/*
 * Struktura reprezentujici bunky tabulky.
 */
typedef struct cell{
    char *cell;
    struct cell *next;
} cell_t;

/*
 * Struktura reprezentujici radky tabulky.
 */
typedef struct row{
    cell_t *first_cell;
    struct row *next;
} row_t;

/*
 * Struktura reprezentujici tabulku.
 */
typedef struct table{
    row_t *first_row;
} table_t;

/*
 * Struktura prezentujici funkci dane operace.
 */
typedef struct{
    uint8_t (*function)(table_t *, selection_t *, argument_t *);
    const char *name;
} function_t;

/*
 * Struktura reprezentujici uzel ve fronte.
 */
typedef struct node{
    uint8_t character;
    struct node *next;
} node;

/*
 * Stuktura reprezentujici frontu.
 */
typedef struct{
    node *head;
    node *tail;
} queue;

/*
 * Funkce slouzi pro pocatecni inicializaci fronty.
 */
void init_queue(queue *queue)
{
    queue->head = NULL;
    queue->tail = NULL;
    return;
}

/*
 * Funkce slouzi pro pridani prvku do fronty.
 */
bool enqueue(queue *queue, uint8_t character)
{
    node *new_node = (node *) malloc(sizeof(node));

    if(new_node == NULL)
    {
        return false;
    }

    new_node->character = character;
    new_node->next = NULL;

    if(queue->tail != NULL)
    {
        queue->tail->next = new_node;
    }

    queue->tail = new_node;

    if(queue->head == NULL)
    {
        queue->head = new_node;
    }

    return true;
}

/*
 * Funkce slouzi pro odebrani prvniho prvku z fronty.
 */
uint8_t dequeue(queue *queue)
{
    if(queue->head == NULL)
    {
        return EMPTYQUEUE;
    }

    node *tmp = queue->head;
    uint8_t result = tmp->character;

    queue->head = queue->head->next;

    if(queue->head == NULL)
    {
        queue->tail = NULL;
    }

    free(tmp);
    tmp = NULL;

    return result;
}

/*
 * Funkce slouzi pro odebrani posledniho prvku z fronty.
 */
uint8_t remove_queue_last(queue *queue)
{
    if(queue->tail == NULL)
    {
        return EMPTYQUEUE;
    }

    node *tmp = queue->tail;
    uint8_t result = tmp->character;

    node *prev_node = queue->head;

    if(prev_node != tmp)
    {
        while(prev_node->next != tmp)
        {
            prev_node = prev_node->next;
        }

        prev_node->next = NULL;
    }
    else
    {
       prev_node = NULL;
    }

    free(tmp);
    tmp = NULL;

    return result;
}

/*
 * Fukce slouzi pro ziskani prvniho prvku z fronty
 * bez jeho odebrani.
 */
uint8_t get_queue_front(queue *queue)
{
    if(queue->head == NULL)
    {
        return EMPTYQUEUE;
    }

    node *tmp = queue->head;
    uint8_t result = tmp->character;

    return result;
}

/*
 * Funkce slouzi pro ziskani posledniho prvku z fronty
 * bez jeho odebrani.
 */
uint8_t get_queue_back(queue *queue)
{
    if(queue->tail == NULL)
    {
        return EMPTYQUEUE;
    }

    node *tmp = queue->tail;
    uint8_t result = tmp->character;

    return result;
}

/*
 * Funkce slouzi pro inicializaci promenne typu ukazatel na table_t.
 */
void init_table(table_t *table)
{
   table->first_row = NULL;
   return;
}

/*
 * Funkce vytvori kopii retezce a vrati ukazatel na retezec.
 * V pripadu chyby vraci NULL.
 */
char *strdup(const char *string)
{
    char *new_string = (char *) malloc(strlen(string) * sizeof(char) + 1);

    if(new_string)
    {
        strcpy(new_string, string);
    }

    return new_string;
}

/*
 * Funkce slouzi pro ziskani poctu radku tabulky.
 */
unsigned int get_count_of_rows(table_t *table, uint8_t *error)
{
    unsigned int count = 0;

    if(table == NULL)
    {
        *error = NULL_TABLE_POINTER;
        return count;
    }

    row_t *row = table->first_row;

    if(row == NULL)
    {
        *error = NULL_ROW_POINTER;
        return count;
    }

    while(row != NULL)
    {
        row = row->next;
        count++;
    }

    return count;
}

/*
 * Funkce slouzi k ziskani poctu sloupcu tabulky.
 */
unsigned int get_count_of_cols(table_t *table, uint8_t *error)
{
    unsigned int count = 0;

    if(table == NULL)
    {
        *error = NULL_TABLE_POINTER;
        return count;
    }

    row_t *row = table->first_row;

    if(row == NULL)
    {
        *error = NULL_ROW_POINTER;
        return count;
    }

    cell_t *cell = row->first_cell;

    while(cell != NULL)
    {
        cell = cell->next;
        count++;
    }

    return count;
}

/*
 * Funkce slouzi k pridani bunky na konec radku.
 */
bool append_cell(row_t *row, char *cell)
{
    cell_t *new_cell = (cell_t *) malloc(sizeof(cell_t));

    if(new_cell == NULL)
    {
        return false;
    }

    new_cell->cell = strdup(cell);

    if(new_cell->cell == NULL)
    {
        return false;
    }

    new_cell->next = NULL;
    cell_t *last_cell = row->first_cell;

    //Pridani prvni bunky na radku.
    if(row->first_cell == NULL)
    {
        row->first_cell = new_cell;
    }
    else
    {
        //Pruchod bunkami, dokud se nejedna o posledni bunku.
        while(last_cell->next != NULL)
        {
            last_cell = last_cell->next;
        }

        last_cell->next = new_cell;
    }

    return true;
}

/*
 * Funkce slouzi k pridani bunky pred danou bunku v radku.
 */
uint8_t add_cell_before(row_t *row, cell_t *next_cell, char *cell)
{
    if(next_cell == NULL)
    {
        return WRONG_INSERT_COL_ARG;
    }

    cell_t *new_cell = (cell_t *) malloc(sizeof(cell_t));

    if(new_cell == NULL)
    {
        return FULL_HEAP;
    }

    cell_t *prev_cell = row->first_cell;

    if (prev_cell == next_cell)
    {
        new_cell->next = next_cell;
        row->first_cell = new_cell;
    }
    else {
        while (prev_cell->next != next_cell) {
            prev_cell = prev_cell->next;
        }

        new_cell->next = next_cell;
        prev_cell->next = new_cell;
    }

    new_cell->cell = strdup(cell);

    if(new_cell->cell == NULL)
    {
        return FULL_HEAP;
    }

    return NO_ERROR;
}

/*
 * Funkce slouzi pro pridani bunky za danou bunku v radku.
 */
uint8_t add_cell_after(cell_t *prev_cell, char *cell)
{
    if(prev_cell == NULL)
    {
        return WRONG_INSERT_COL_ARG;
    }

    cell_t *new_cell = (cell_t *) malloc(sizeof(cell_t));

    if(new_cell == NULL)
    {
        return FULL_HEAP;
    }

    new_cell->cell = strdup(cell);

    if(new_cell->cell == NULL)
    {
        return FULL_HEAP;
    }

    new_cell->next = prev_cell->next;
    prev_cell->next = new_cell;

    return NO_ERROR;
}

/*
 * Funkce slouzi pro pridani noveho radku na konec tabulky.
 */
bool append_row(table_t *table)
{
    row_t *new_row = (row_t *) malloc(sizeof(row_t));

    if(new_row == NULL)
    {
        return false;
    }

    new_row->first_cell = NULL;
    new_row->next = NULL;

    row_t *last_row = table->first_row;

    //Pridani prvniho radku v tabulce
    if(table->first_row == NULL)
    {
        table->first_row = new_row;
    }
    else
    {
        while(last_row->next != NULL)
        {
            last_row = last_row->next;
        }

        last_row->next = new_row;
    }

    return true;
}

/*
 * Funkce slouzi pro pridani noveho radku pred dany radek v tabulce.
 */
uint8_t add_row_before(table_t *table, row_t *next_row)
{
    uint8_t error = NO_ERROR;
    bool append_error = true;
    unsigned int count_of_cols;

    if(next_row == NULL)
    {
        error = WRONG_INSERT_ROW_ARG;
        return error;
    }

    row_t *new_row = (row_t *) malloc(sizeof(row_t));

    if(new_row == NULL)
    {
        error = FULL_HEAP;
        return error;
    }

    count_of_cols = get_count_of_cols(table, &error);
    row_t *prev_row = table->first_row;

    if(prev_row == next_row)
    {
        new_row->next = next_row;
        table->first_row = new_row;
    }
    else {
        while (prev_row->next != next_row) {
            prev_row = prev_row->next;
        }

        new_row->next = next_row;
        prev_row->next = new_row;
    }

    new_row->first_cell = NULL;

    //Pridani bunek do noveho radku.
    while(count_of_cols > 0 && (append_error = append_cell(new_row, "")))
    {
        count_of_cols--;
    }

    if(!append_error)
    {
        error = FULL_HEAP;
    }

    return error;
}

/*
 * Funkce slouzi pro pridani noveho radku za dany radek v tabulce.
 */
uint8_t add_row_after(table_t *table, row_t *prev_row)
{
    uint8_t error = NO_ERROR;
    bool append_error = true;
    unsigned int count_of_cols;

    if(prev_row == NULL)
    {
        error = WRONG_INSERT_ROW_ARG;
        return error;
    }

    count_of_cols = get_count_of_cols(table, &error);
    row_t *new_row = (row_t *) malloc(sizeof(row_t));

    if(new_row == NULL)
    {
        error = FULL_HEAP;
        return error;
    }

    new_row->next = prev_row->next;
    prev_row->next = new_row;
    new_row->first_cell = NULL;

    //Pridani bunek do noveho radku.
    while(count_of_cols > 0 && (append_error = append_cell(new_row, "")))
    {
        count_of_cols--;
    }

    if(!append_error)
    {
        return FULL_HEAP;
    }

    return error;
}

/*
 * Funkce slozi pro smazani bunky v tabulce.
 */
uint8_t delete_cell(row_t *row, int position)
{
    if(row == NULL)
    {
       return ERROR_DELETING_CELL;
    }

    cell_t *tmp = row->first_cell;

    if(position == 1)
    {
        row->first_cell = tmp->next;

        free(tmp->cell);
        tmp->cell = NULL;

        free(tmp);
        tmp = NULL;

        return NO_ERROR;
    }

    for(int i = 1; tmp != NULL && i < position - 1; i++)
    {
        tmp = tmp->next;
    }

    if(tmp == NULL || tmp->next == NULL)
    {
        return ERROR_DELETING_CELL;
    }

    cell_t *next = tmp->next->next;
    free(tmp->next->cell);
    tmp->next->cell = NULL;

    free(tmp->next);
    tmp->next = NULL;

    tmp->next = next;

    return NO_ERROR;
}

/*
 * Funkce slouzi pro smazani bunek v danem radku.
 */
uint8_t delete_row_cells(row_t *row)
{
    uint8_t error = NO_ERROR;
    cell_t *cell = row->first_cell;
    unsigned int cell_position = 1;

    while (cell != NULL)
    {
        while (cell->next != NULL)
        {
            cell = cell->next;
            cell_position++;
        }

        error = delete_cell(row, cell_position);

        if(error != NO_ERROR)
        {
            return error;
        }

        cell = row->first_cell;
        cell_position = 1;
    }

    return error;
}

/*
 * Funkce slouzi pro smazani radku tabulky.
 */
uint8_t delete_row(table_t *table, int position)
{
    uint8_t error = NO_ERROR;

    if(table == NULL)
    {
        error = ERROR_DELETING_ROW;
        return error;
    }

    row_t *tmp = table->first_row;

    /*
     * Prvni radek tabulky.
     */
    if(position == 1)
    {
        table->first_row = tmp->next;
        error = delete_row_cells(tmp);

        if(error != NO_ERROR)
        {
            return error;
        }

        free(tmp);
        tmp = NULL;

        error = NO_ERROR;
        return error;
    }

    for(int i = 1; tmp != NULL && i < position - 1; i++)
    {
        tmp = tmp->next;
    }

    if(tmp == NULL || tmp->next == NULL)
    {
        table->first_row = tmp->next;
        error = delete_row_cells(tmp);

        if(error != NO_ERROR)
        {
            return error;
        }

        free(tmp);
        tmp = NULL;
    }
    else
    {
        row_t *next = tmp->next->next;

        error = delete_row_cells(tmp->next);

        if(error != NO_ERROR)
        {
            return error;
        }

        free(tmp->next);
        tmp->next = NULL;

        tmp->next = next;
    }

    return error;
}

/*
 * Funkce slouzi pro ziskani hodnoty bunky na danem radku
 * a v danem sloupci tabulky.
 */
char *get_cell_value(unsigned int row_number, unsigned int col_number,
                     table_t *table, uint8_t *error)
{
    unsigned int row_counter = 1;
    unsigned int col_counter = 1;

    if(table == NULL)
    {
        *error = NULL_TABLE_POINTER;
    }

    row_t *row = table->first_row;

    if(row == NULL)
    {
        *error = NULL_TABLE_POINTER;
    }

    while(row_counter < row_number &&
          row->next != NULL)
    {
        row = row->next;
        row_counter++;
    }

    cell_t *cell = row->first_cell;

    if(cell == NULL)
    {
        *error = NULL_CELL_POINTER;
    }

    while(col_counter < col_number &&
          cell->next != NULL)
    {
        cell = cell->next;
        col_counter++;
    }

    return cell->cell;
}

/*
 * Funkce slouzi pro pridani noveho sloupce do tabulky pro operace icol a acol.
 */
uint8_t add_column(table_t *table, int position, const char *function_name)
{
    uint8_t error = NO_ERROR;

    if(table == NULL)
    {
        error = NULL_TABLE_POINTER;
        return error;
    }

    row_t *row = table->first_row;
    cell_t *cell;

    while(row != NULL)
    {
        cell = row->first_cell;

        for(int i = 0; cell != NULL && i < position-1; i++)
        {
            cell = cell->next;
        }

        /*
         * V pripadu operace icol se pridava bunka pred danou bunky.
         */
        if(strcmp(function_name, "icol") == 0)
        {
            error = add_cell_before(row, cell, "");

            if(error != NO_ERROR)
            {
                return error;
            }
        }
        else
        {
            /*
             * Operace acol.
             * V pripadu operace acol se pridava bunka za danou bunku.
             */
            error = add_cell_after(cell, "");

            if(error != NO_ERROR)
            {
                return error;
            }
        }

        row = row->next;
    }

    return error;
}

/*
 * Funkce slouzi pro smazani sloupce v tabulce.
 */
uint8_t delete_column(table_t *table, int position)
{
    uint8_t error = NO_ERROR;

    if(table == NULL)
    {
        error = NULL_TABLE_POINTER;
        return error;
    }

    row_t *row = table->first_row;

    while(row != NULL)
    {
        error = delete_cell(row, position);
        row = row->next;
    }

    return error;
}

/*
 * Funkce slouzi pro smazani nadbytecnych prazdnych poslednich sloupcu tabulky.
 */
uint8_t delete_redundant_cols(table_t *table)
{
    uint8_t error = NO_ERROR;
    bool delete_cells = true;
    row_t *row = table->first_row;
    cell_t *cell;
    unsigned int count_of_cols;

    if(table != NULL && row != NULL)
    {
        while(delete_cells != false && row->first_cell != NULL)
        {
            count_of_cols = get_count_of_cols(table, &error);

            if(error != NO_ERROR)
            {
                break;
            }

            while(row != NULL)
            {
                cell = row->first_cell;

                while(cell->next != NULL)
                {
                    cell = cell->next;
                }

                if(strcmp(cell->cell, "") != 0)
                {
                    delete_cells = false;
                    break;
                }

                row = row->next;
            }

            if(delete_cells)
            {
                row = table->first_row;

                while(row != NULL)
                {
                    error = delete_cell(row, count_of_cols);
                    row = row->next;
                }
            }

            row = table->first_row;

            if(error != NO_ERROR)
            {
                break;
            }
        }
    }

    return error;
}

/*
 * Funkce slouzi pro smazani nadbytecnych prazdnych poslednich radku tabulky.
 */
uint8_t delete_redundant_rows(table_t *table)
{
    uint8_t error = NO_ERROR;
    row_t *row = table->first_row;
    cell_t *cell;
    unsigned int count_of_rows;

    if(table != NULL && row != NULL)
    {
        while (row != NULL)
        {
            count_of_rows = get_count_of_rows(table, &error);

            while(row->next != NULL)
            {
                row = row->next;
            }

            cell = row->first_cell;

            while(cell != NULL)
            {
                if(strcmp(cell->cell, "") != 0)
                {
                    return error;
                }

                cell = cell->next;
            }

            delete_row(table, count_of_rows);
            row = table->first_row;
        }
    }

    return error;
}

/*
 * Funkce slouzi pro smazani nadbytecnych bunek tabulku.
 * Obsahuje funkci pro mazani nadbytecnych radku
 * a funkci pro mazani nadbytecnych sloupcu.
 */
uint8_t delete_redundant_cells(table_t *table)
{
    uint8_t error = NO_ERROR;

    /*
     * Smazani nadbytecnych sloupcu jeste pred smazanim nadbytecnych radku.
     */
    error = delete_redundant_cols(table);

    if(error != NO_ERROR)
    {
        return error;
    }

    error = delete_redundant_rows(table);

    return error;
}

/*
 * Funkce slouzi ke zjisteni, zda se ma cela bunka vlozit do uvozovek.
 */
bool insert_quotation_marks(argument_t *argument, char *cell)
{
    bool insert = false;
    int index;

    if(strchr(cell, '\"') != NULL)
    {
        insert = true;
    }
    else
    {
        index = 0;
        while(argument->delims[index] != '\0')
        {
            if(strchr(cell, argument->delims[index]) != NULL)
            {
                insert = true;
                break;
            }

            index++;
        }
    }

    return insert;
}

/*
 * Funkce slouzi pro vypis tabulky do souboru.
 */
uint8_t print_table(table_t *table, argument_t *argument)
{
    int index;
    bool insert;
    row_t *row = table->first_row;
    cell_t *cell;
    FILE *file;

    file = fopen(argument->file, "w");

    if(file == NULL)
    {
        return WRONG_FILE_NAME;
    }

    while(row != NULL)
    {
        cell = row->first_cell;

        while(cell != NULL)
        {
            /* Vypsani delimiteru, pred bunku,
             * pokud se nejedna o prvni bunku v radku.
             */
            if(cell != row->first_cell)
            {
                fprintf(file, "%c", argument->output_delim);
            }

            insert = false;
            if(insert_quotation_marks(argument, cell->cell))
            {
                insert = true;
                fprintf(file, "%c", '\"');
            }

            index = 0;
            while(cell->cell[index] != '\0')
            {
                if(cell->cell[index] == '\\' || cell->cell[index] == '\"')
                {
                    fprintf(file, "%c", '\\');
                }

                fprintf(file, "%c", cell->cell[index]);
                index++;
            }

            if(insert)
            {
                fprintf(file, "%c", '\"');
            }

            cell = cell->next;
        }

        /*
         * Odsazeni noveho radku.
         */
        fprintf(file, "\n");
        row = row->next;
    }

    fclose(file);

    return NO_ERROR;
}

/*
 * Funkce slouzi pro inicializaci selekce na zacatku programu.
 */
uint8_t init_selection(selection_t *selection)
{
    uint8_t error = NO_ERROR;

    selection->first_row = 1;
    selection->first_col = 1;
    selection->second_row = 1;
    selection->second_col = 1;

    return error;
}

/*
 * Funkce slouzi pro testovani spravneho poctu argumentu.
 */
uint8_t valid_args_count(int argc)
{
    uint8_t error = NO_ERROR;

    if(argc < MINARGSCOUNT || argc > MAXARGSCOUNT)
    {
        error = WRONG_ARGS_COUNT;
    }

    return error;
}

/*
 * Funkce slouzi pro testovani, zda pri zadani argumentu -d
 * byl zadan odpovidajici pocet argumentu.
 */
uint8_t valid_args_count_delims(int argc)
{
    uint8_t error = NO_ERROR;

    if(argc!=MAXARGSCOUNT)
    {
        error = WRONG_ARGS_COUNT;
    }

    return error;
}

/*
 * Funkce slouzi pro otestovani spravnosti zadaneho delimiteru.
 * Delimiter nesmi obsahovat znak uvozovky ani zpetne lomitko.
 */
uint8_t valid_delims(char **argv)
{
    uint8_t error = NO_ERROR;

    if(strchr(argv[DELIMARGINDEX], '\\') != NULL ||
       strchr(argv[DELIMARGINDEX], '\"') != NULL)
    {
        error = WRONG_DELIM_FORMAT;
    }

    return error;
}

/*
 * Nastaveni delimiteru. Pokud bude zadan prepinac -d DELIM, nastavi delimiter
 * na tuto zadanou hodnotu. V opacnem pripadu nastavi delimiter na znak mezery.
 */
uint8_t set_delims(int argc, char **argv, argument_t *argument)
{
    uint8_t error = NO_ERROR;

    /*
     * V pripadu zadaneho argumentu -d se bude nachazet na indexu DARGINDEX.
     */
    if(strcmp(argv[DARGINDEX], "-d") == 0)
    {
        if((error = valid_args_count_delims(argc)) == NO_ERROR &&
           (error = valid_delims(argv)) == NO_ERROR)
        {
            /*
             * Retezec DELIM se bude nachazet na indexu DELIMARGINDEX.
             */
            argument->delims = argv[DELIMARGINDEX];
        }
    }
    else
    {
        argument->delims = " ";
    }

    return error;
}

/*
 * Nastaveni delimiteru pro vystupni tabulku.
 */
void set_output_delim(argument_t *argument)
{
    /*
     * Delimiter pro vystupni hodnoty se nachazi na prvnim indexu
     * v zadanem retezci DELIM.
     */
    argument->output_delim = argument->delims[0];
    return;
}

/*
 * Funkce vraci pocet delimiteru ';' v retezci string.
 */
unsigned int get_count_of_delims(char *string)
{
    int i=0;
    for (i=0; string[i]; string[i]==';' ? i++ : *string++);

    return i;
}

/*
 * Funkce slouzi ke zjisteni, zda retezec zacina znakem uvozovky.
 */
bool starts_with_quotation_mark(queue *arg_characters)
{
    node *tmp = arg_characters->head;
    bool found = false;

    while(tmp->next != NULL)
    {
        /*
         * V pripadu, ze argument obsajuje mezeru
         * muze obsahovat jako argument operace retezec STR.
         */

        if(tmp->character == ' ')
        {
            if(tmp->next->character == '\"')
            {
                found = true;
            }

            break;
        }

        tmp = tmp->next;
    }

    return found;
}

/*
 * Funkce slouzi ke smazani uvozovky na zacatku retezce.
 */
void delete_starts_quotation_mark(queue *arg_characters)
{
    node *tmp = arg_characters->head;

    while(tmp->next != NULL)
    {
        if(tmp->character == ' ')
        {
            if(tmp->next->character == '\"')
            {
                /*
                 * Smazani uvozovky na zacatku retezce STR.
                 */
                 node *node = tmp->next;
                 tmp->next = tmp->next->next;
                 free(node);
                 node = NULL;
            }

            break;
        }

        tmp = tmp->next;
    }

    return;
}

/*
 * Funkce slouzi pro vyprazdneni fronty, kdyz uz neni dale mozna alokace pameti
 * pomoci malloc na heap ve funkci enqueue.
 */
void set_queue_empty(queue *cell_characters)
{
    while(dequeue(cell_characters) != EMPTYQUEUE);
    return;
}

/*
 * Konvertovani znaku ulozenych ve fronte do jednoho retezce (char array).
 */
uint8_t convert_arg_queue_to_string(queue *arg_characters, argument_t *argument,
                                    unsigned int *arg_index, int characters_count)
{
    uint8_t error = NO_ERROR;
    uint8_t character;
    char arg[characters_count];

    int index = 0;
    while((character = dequeue(arg_characters)) != EMPTYQUEUE)
    {
        arg[index] = (char)character;
        index++;
    }

    arg[index] = '\0';
    argument->cmd_sequence[*arg_index] = strdup(arg);

    if(argument->cmd_sequence[*arg_index] == NULL)
    {
        error = FULL_HEAP;
    }

    return error;
}

/*
 * Prevod argumentu v retezci string do pole retezcu (jednotlivych argumentu).
 */
uint8_t string_to_arg_sequence(argument_t *argument, char *string)
{
    uint8_t error = NO_ERROR;
    char delim = ';';
    char character;
    unsigned int characters_count;
    queue arg_characters;
    unsigned int index = 0;
    unsigned int arg_index = 0;
    unsigned int string_length = strlen(string);

    argument->cmd_sequence = (char **) malloc((get_count_of_delims(string)+2) * sizeof(char *));

    if(argument->cmd_sequence == NULL)
    {
        error = FULL_HEAP;
        return error;
    }

    /*
     * Pocatecni inicializace fronty.
     */
    init_queue(&arg_characters);
    character = string[index];

    while(index < string_length)
    {
        characters_count = 0;

        while(character != delim && character != '\0')
        {

            /*
             * Pripad escape sequence.
             */
            if(character == '\\')
            {
                index++;
                character = string[index];
            }

            if(!enqueue(&arg_characters, character))
            {
                set_queue_empty(&arg_characters);
                error = FULL_HEAP;
                return error;
            }

            characters_count++;
            index++;
            character = string[index];


            /*
             * Pripad s uvozovkami.
             */
            if(character == '\0' || character == delim || character == ']')
            {
                if((char)get_queue_back(&arg_characters) == '\"' &&
                    starts_with_quotation_mark(&arg_characters))
                {
                    /*
                     * Smazani uvozovky na konci retezce STR.
                     */
                    remove_queue_last(&arg_characters);

                    /*
                     * Smazani uvozovky na zacatku retezce STR.
                     */
                    delete_starts_quotation_mark(&arg_characters);

                    characters_count-=2;
                }
            }
        }

        /*
         * Prevedeni fronty obsahujici znaky jedne bunky do retezce
         * datoveho typu char *.
         */
        error = convert_arg_queue_to_string(&arg_characters,argument,
                                            &arg_index, characters_count);

        if(error != NO_ERROR)
        {
            return error;
        }

        arg_index++;
        index++;
        character = string[index];
    }

    argument->cmd_sequence[arg_index] = NULL;

    return error;
}

/*
 * Funkce slouzi pro nastaveni sekvence prikazu a nazvu souboru
 * do struktury datoveho typu argument_t.
 */
uint8_t set_cmd_sequence_file(char **argv, argument_t *argument)
{
    uint8_t error;
    if(strcmp(argument->delims, " ") == 0)
    {
        /*
         * Defaultni hodnota delimiteru.
         */
        error = string_to_arg_sequence(argument, argv[SEQINDEXNODELIM]);
        argument->file = argv[FILEINDEXNODELIM];
    }
    else
    {
        /*
         * Na vstupu je zadan delimiter.
         */
        error = string_to_arg_sequence(argument, argv[SEQINDEXWITHDELIM]);
        argument->file = argv[FILEINDEXWITHDELIM];
    }

    return error;
}

/*
 * Funkce slouzi pro pocatecni inicializaci docasnych promennych.
 */
uint8_t init_tmp_var(tmp_var_t *tmp_var)
{
    unsigned int i = 0;

    init_selection(&(tmp_var->tmp_var_selection));

    while(i < COUNTOFTMPVARIABLES)
    {
        tmp_var->tmp_variables[i] = strdup("");

        if(tmp_var->tmp_variables[i] == NULL)
        {
            return FULL_HEAP;
        }

        i++;
    }

    return NO_ERROR;
}

/*
 * Funkce slouzi pro vytvoreni a zaalokovani mista pro docasne promenne.
 */
tmp_var_t *create_tmp_var(uint8_t *error)
{
    tmp_var_t *tmp_var = (tmp_var_t *) malloc(sizeof(tmp_var_t));

    if(tmp_var == NULL)
    {
        *error = FULL_HEAP;
        return tmp_var;
    }

    tmp_var->tmp_variables = (char **) malloc(sizeof(char *) * (COUNTOFTMPVARIABLES));

    if(tmp_var->tmp_variables == NULL)
    {
        free(tmp_var);
        tmp_var = NULL;
        *error = FULL_HEAP;
    }

    return tmp_var;
}

/*
 * Funkce slouzi pro nastaveni argumentu z prikazoveho radku
 * do struktury datoveho typu argument_t.
 */
uint8_t set_arguments(int argc, char **argv, argument_t *argument)
{
    uint8_t error = NO_ERROR;

    if((error = valid_args_count(argc)) == NO_ERROR)
    {
        /*
         * Nastaveni delimiteru.
         */
        error = set_delims(argc, argv, argument);
        set_output_delim(argument);
        argument->command_value = NULL;

        if(error == NO_ERROR)
        {
            /*
             * Nastaveni cmd_sequence a file.
             */
            error = set_cmd_sequence_file(argv, argument);

            if(error == NO_ERROR)
            {
                argument->tmp_var = create_tmp_var(&error);
            }
        }
    }

    return error;
}

/*
 * Nastaveni hodnoty retezce cell do bunky.
 * Pocatecni vkladani bunek do tabulky pri jejim nacitani ze vstupu.
 */
uint8_t set_cell_value(table_t *table, bool new_line, char *cell)
{
    uint8_t error = NO_ERROR;

    /*
     * Pridani prvniho nebo dalsiho radku do tabulky.
     */
    if(table->first_row == NULL)
    {
        if(!append_row(table))
        {
            error = FULL_HEAP;
            return error;
        }
    }

    row_t *row = table->first_row;

    while(row->next != NULL)
    {
        row = row->next;
    }

    if(!append_cell(row, cell))
    {
        error = FULL_HEAP;
    }

    if(new_line)
    {
        if(!append_row(table))
        {
            error = FULL_HEAP;
        }
    }

    return error;
}

/*
 * Nastaveni prazdne hodnoty dane bunky.
 */
uint8_t set_cell_empty_value(cell_t *cell)
{
    free(cell->cell);
    cell->cell = NULL;

    cell->cell = strdup("");

    if(cell->cell == NULL)
    {
        return FULL_HEAP;
    }

    return NO_ERROR;
}

/*
 * Nastaveni nove hodnoty do dane bunky.
 */
uint8_t set_cell_new_value(cell_t *cell, char *value)
{
    free(cell->cell);
    cell->cell = NULL;

    cell->cell = strdup(value);

    if(cell->cell == NULL)
    {
        return FULL_HEAP;
    }

    return NO_ERROR;
}

/*
 * Funkce slouzi pro prevedeni jednotlivych znaku ve fronte
 * do retezce (pole znaku).
 * Vysledne pole funkce preda funkci set_cell_value,
 * ktera prida retezec do struktury cell.
 */
uint8_t convert_queue_to_string(queue *cell_characters,
                                unsigned int characters_count,
                                table_t *table,
                                bool new_line)
{
    uint8_t character;
    uint8_t error = NO_ERROR;

    char cell[characters_count];

    int index = 0;
    while((character = dequeue(cell_characters)) != EMPTYQUEUE)
    {
        cell[index] = (char)character;
        index++;
    }

    /*
     * Pridani znaku konce retezce.
     */
    cell[index] = '\0';

    error = set_cell_value(table, new_line, cell);

    return error;
}

/*
 * Funkce slouzi pro ziskani nasledujiciho znaku ze streamu.
 */
int fpeek(FILE *stream)
{
    int character;

    character = fgetc(stream);
    ungetc(character, stream);

    return character;
}

/*
 * Funkce testuje konec souboru.
 */
bool end_of_file(FILE *file, uint8_t *error)
{
    int character = fpeek(file);

    if(character == EOF)
    {
        if(feof(file))
        {
            return true;
        }
        else
        {
            *error = ERROR_EOF;
        }
    }

    return false;
}

/*
 * Slouzi pro nacitani znaku ze souboru.
 * V pripadu konce souboru vraci ENDOFFILE.
 * V opacnem pripadu vraci NOTENDOFFILE.
 */
bool scan_cell(FILE *file, argument_t *argument,
               table_t *table, uint8_t *error)
{
    char character;
    unsigned int characters_count = 1;
    queue cell_characters;
    bool new_line;
    bool first_character_backslash = false;

    //Pocatecni inicializace fronty.
    init_queue(&cell_characters);

    character = fgetc(file);

    while(character != '\n' &&
          character != EOF &&
          strchr(argument->delims, character) == NULL)
    {

        /*
         * Pripad escape sequence.
         */
        if(character == '\\')
        {
            if(characters_count == 1)
            {
                first_character_backslash = true;
            }

            character = fgetc(file);
        }

        /*
         * Pridani znaku do fronty.
         * Podminka se vyhodnoti jako true, pokud nebude mozne
         * dale pridavat prvky do fronty z duvodu zaplneni pameti heap.
         */
        if(!enqueue(&cell_characters, character))
        {
            set_queue_empty(&cell_characters);
            *error = FULL_HEAP;
            return STOPONERROR;
        }

        character = fgetc(file);
        characters_count++;

        /*
         * Pokud je hodnota znaku z retezce delim
         * a prvni znak v bunce je znak uvozovky.
         */
        if((character == '\n' ||
           strchr(argument->delims, character) != NULL) &&
           (char)get_queue_front(&cell_characters) == '\"')
        {
            /*
             * Pokud je predchozi znak pred oddelovacem,
             * budou uvozovky na zacatku a konci bunky odstraneny.
             */
            if((char)get_queue_back(&cell_characters) == '\"' &&
                first_character_backslash == false)
            {
                /*
                 * Odstraneni posledniho znaku bunky.
                 */
                remove_queue_last(&cell_characters);

                /*
                 * Odstraneni prvniho znaku bunky.
                 */
                dequeue(&cell_characters);
                characters_count-=2;
            }
        }
    }

    new_line = (character == '\n' && !end_of_file(file, error)) ? true : false;

    if(*error != NO_ERROR)
    {
        return STOPONERROR;
    }

    /*
     * Volani funkce, ktera prevede znaky z fronty na retezec.
     */
    *error = convert_queue_to_string(&cell_characters, characters_count,
                                     table, new_line);

    //Otestovani konce souboru.
    if(end_of_file(file, error))
    {
        return ENDOFFILE;
    }

    return NOTENDOFFILE;
}

/*
 * Nacteni cele tabulky.
 * Funkce slouzi pro volani funkce scan_cell nacitajici bunky.
 */
void scan_table_cells(FILE *file, argument_t *argument,
                      table_t *table, uint8_t *error)
{
    while(scan_cell(file, argument, table, error) != ENDOFFILE &&
          *error == NO_ERROR);
    return;
}

/*
 * Funkce slouzici pro nacteni tabulky ze souboru.
 */
void scan_table(argument_t *argument, table_t *table, uint8_t *error)
{
    FILE *file;

    file = fopen(argument->file, "r");

    if(file == NULL)
    {
        *error = WRONG_FILE_NAME;
    }
    else
    {
       scan_table_cells(file, argument, table, error);
       fclose(file);
    }

    return;
}

/*
 * Funkce slouzi pro ziskani poctu elementu v argumentu pro selekci.
 */
int get_count_of_elements(char *command)
{
    int count;
    for(count = 0; command[count]; command[count] == ',' ? count++ : *command++);

    return count;
}

/*
 * Ziskani ciselne hodnotu z retezce command obsahujiciho prikaz pro selekci.
 */
unsigned int get_command_num_value(char *command,
                                   unsigned int *index,
                                   uint8_t *error)
{
    unsigned int number = 0;
    unsigned int number_to_add = 0;

    for(; command[*index]!=',' && command[*index] != ']'; (*index)++)
    {
        number*=10;
        number_to_add = command[*index] - '0';

        if(number_to_add > 9 || (number_to_add == 0 && number == 0))
        {
            *error = WRONG_SELECTION_ARG;
            return 0;
        }

        number+= number_to_add;
    }

    /*
     * Odchyceni chybne zadaneho vyberu selekce.
     * Testovani nasledujicich pripadu:
     * - [,...]
     * - [...,]
     * - [,]
     */
    if(command[*index]==',')
    {
        if(command[*index - 1] == '[' || command[*index + 1] == ']')
        {
            *error = WRONG_SELECTION_ARG;
            return 0;
        }
    }
    else
    {
        if(command[*index-1] == '[')
        {
            *error = WRONG_SELECTION_ARG;
            return 0;
        }
    }

    (*index)++;

    return number;
}

/*
 * Testovani argumentu, zda se jedna o ciselnou hodnotu.
 */
bool is_number(char *argument)
{
    if(strcmp(argument, "") == 0)
    {
        return false;
    }

	for(long unsigned int i=0; i<strlen(argument); i++)
	{
		if(isdigit(argument[i]) == 0)
        {
            if(argument[i] != '-' && argument[i] != '+' && argument[i] != '.')
            {
                return false;
            }
        }
	}

	return true;
}

/*
 * Nastaveni hodnoty selekce pri vyberu pouze jedne bunky.
 */
void set_selection_one_cell(selection_t *selection,
                            unsigned int row,
                            unsigned int col)
{
    selection->first_row = row;
    selection->second_row = row;
    selection->first_col = col;
    selection->second_col = col;
    return;
}

/*
 * Nastaveni selekce operace [min].
 */
uint8_t set_selection_min(selection_t *selection, table_t *table)
{
    uint8_t error = NO_ERROR;
    double number_min = DBL_MAX;
    double number = 0;
    unsigned int row = 0;
    unsigned int col = 0;
    char *cell;

    for(unsigned int i = selection->first_row; i <= selection->second_row; i++)
    {
        for(unsigned int j = selection->first_col; j <= selection->second_col; j++)
        {
            cell =  get_cell_value(i, j, table, &error);

            if(error != NO_ERROR)
            {
                return error;
            }

            if(is_number(cell))
            {
                number = atof(cell);

                if(number < number_min)
                {
                    /*
                     * Nastaveni pozice radku a sloupce
                     * na pozici bunky s minimalni hodnotou.
                     */
                    number_min = number;
                    col = j;
                    row = i;
                }
            }
        }
    }

    if(row != 0 && col != 0)
    {
        set_selection_one_cell(selection, row, col);
    }

    return error;
}

/*
 * Nastaveni selekce operace [max].
 */
uint8_t set_selection_max(selection_t *selection, table_t *table)
{
    uint8_t error = NO_ERROR;
    double number_max = -DBL_MAX;
    double number = 0;
    unsigned int row = 0;
    unsigned int col = 0;
    char *cell;

    for(unsigned int i = selection->first_row; i <= selection->second_row; i++)
    {
        for(unsigned int j = selection->first_col; j <= selection->second_col; j++)
        {
            cell = get_cell_value(i, j, table, &error);

            if(error != NO_ERROR)
            {
                return error;
            }

            if(is_number(cell))
            {
                number = atof(cell);
                if(number > number_max)
                {
                    /*
                     * Nastaveni pozice radku a sloupce
                     * na pozici bunky s maximalni hodnotou.
                     */
                    number_max = number;
                    col = j;
                    row = i;
                }
            }
        }
    }

    if(row != 0 && col != 0)
    {
        set_selection_one_cell(selection, row, col);
    }

    return error;
}

/*
 * Nastaveni selekce operace [find STR].
 */
uint8_t set_selection_string(selection_t *selection,
                             table_t *table,
                             char *string)
{
    uint8_t error = NO_ERROR;
    char *cell;

    for(unsigned int i = selection->first_row; i <= selection->second_row; i++)
    {
        for(unsigned int j = selection->first_col; j <= selection->second_col; j++)
        {
            cell = get_cell_value(i, j, table, &error);

            if(error != NO_ERROR)
            {
                return error;
            }

            if(strstr(cell, string) != NULL)
            {
                set_selection_one_cell(selection, i, j);
                return error;
            }
        }
    }

    return error;
}

/*
 * Funkce slouzi pro vyber zadane operace z operaci pro vyber jedne bunky.
 * V pripadu nenalezeni dane operace se jedna o chybny tvar operace.
 */
uint8_t set_selection_one_element(char *command,
                                  selection_t *selection,
                                  table_t *table)
{
    uint8_t error = NO_ERROR;

    /*
     * Mozne tvary:
     * [min]
     * [max]
     * [find STR]
     * V jinem pripadu se jedna o chybny tvar.
     */

    if(strcmp(command, "[min]") == 0)
    {
        error = set_selection_min(selection, table);
    }
    else if(strcmp(command, "[max]") == 0)
    {
        error = set_selection_max(selection, table);
    }
    else if(strstr(command, "[find") != NULL)
    {
        command[strlen(command) - 1] = '\0';
        error = set_selection_string(selection, table, &command[POSITIONOFSTRING]);
    }
    else
    {
        //Chybny tvar.
        error = WRONG_SELECTION_ARG;
    }

    return error;
}

/*
 * Funkce slouzi pro nastaveni selekce
 * pro operace obsahujici dve ciselne hodnoty.
 */
uint8_t set_selection_two_elements(char *command,
                                   selection_t *selection,
                                   table_t *table)
{
     uint8_t error = NO_ERROR;
     unsigned int index = 1;
    /*
     * Mozne tvary:
     * [R, C]
     * [R, _]
     * [_, C]
     * [_, _]
     */

    /*
     * command[1] - znak '_' by se nachazel v pripadu jeho zadani na indexu 1.
     */
    if(command[index] == '_')
    {
        selection->first_row = 1;
        selection->second_row = get_count_of_rows(table, &error);
        index+=2;
    }
    else
    {
        //Ciselna hodnota.
        selection->first_row = get_command_num_value(command, &index, &error);
        selection->second_row = selection->first_row;
    }

     if(error != NO_ERROR)
     {
        	return error;
     }

    if(command[index] == '_')
    {
        selection->first_col = 1;
        selection->second_col = get_count_of_cols(table, &error);
        index+=2;
    }
    else
    {
        //Ciselna hodnota.
        selection->first_col = get_command_num_value(command, &index, &error);
        selection->second_col = selection->first_col;
    }

    return error;
}

/*
 * Funkce slouzi pro nastaveni selekce pro nasledujici tvary operace:
 * [R1,C1,R2,C2]
 * [R1,C1,-,C2]
 * [R1,C1,R2,-]
 * [R1,C1,-,-]
 */
uint8_t set_selection_four_elements(char *command,
                                    selection_t *selection,
                                    table_t *table)
{
    uint8_t error = NO_ERROR;
    unsigned int index = 1;

    selection->first_row = get_command_num_value(command, &index, &error);
    selection->first_col = get_command_num_value(command, &index, &error);

    if(error != NO_ERROR)
   	{
   		return error;
   	}

    if(command[index] == '-')
    {
        selection->second_row = get_count_of_rows(table, &error);
        index+=2;
    }
    else
    {
        selection->second_row = get_command_num_value(command, &index, &error);
    }

    if(error != NO_ERROR)
   	{
   		return error;
    }

    if(command[index] == '-')
    {
        selection->second_col = get_count_of_cols(table, &error);
        index+=2;
    }
    else
    {
        selection->second_col = get_command_num_value(command, &index, &error);
    }

    if(selection->second_row < selection->first_row ||
       selection->second_col < selection->first_col)
    {
        error = WRONG_ARGUMENTS_ORDER;
    }

    return error;
}

/*
 * Funkce slouzi pro vyber, zda dana operace selekce:
 * - vybira pouze jednu bunku
 * - vybira radky a sloupce
 * - vybira okno bunek
 * V pripadu neshody se jedna o nespravny tvar
 * a je vracena chyba WRONG_SELECTION_ARG.
 */
uint8_t set_selection(char *command, selection_t *selection, table_t *table)
{
    uint8_t error = NO_ERROR;
    int count_of_elements = get_count_of_elements(command);

    switch(count_of_elements)
    {
        case 0:
            error = set_selection_one_element(command, selection, table);
            break;
        case 1:
            error = set_selection_two_elements(command, selection, table);
            break;
        case 3:
            error = set_selection_four_elements(command, selection, table);
            break;
        default:
            error = WRONG_SELECTION_ARG;
    }

    return error;
}

/*
 * Funkce slouzi pro ziskani ukazatele na radek na dane pozici v tabulce.
 */
row_t *get_row_pointer(table_t *table, unsigned int position, uint8_t *error)
{
    if(table == NULL)
    {
        *error = NULL_TABLE_POINTER;
    }

    row_t *row = table->first_row;

    if(row == NULL)
    {
        *error = NULL_ROW_POINTER;
    }

    while(position > 1)
    {
        row = row->next;
        position--;
    }

    return row;
}

/*
 * Funkce slouzi pro ziskani ukazatele na bunku
 * na danem radku tabulky a dane pozici v tabulce.
 */
cell_t *get_cell_pointer(table_t *table, unsigned int row_position,
                         unsigned int position, uint8_t *error)
{
    if(table == NULL)
    {
        *error = NULL_TABLE_POINTER;
    }

    row_t *row = get_row_pointer(table, row_position, error);

    if(*error != NO_ERROR)
    {
        return NULL;
    }

    if(row == NULL)
    {
        *error = NULL_ROW_POINTER;
    }

    cell_t *cell = row->first_cell;

    while(position > 1)
    {
        cell = cell->next;
        position--;
    }

    return cell;
}

/*
 * Ziskani ukazatele na posledni radek v tabulce.
 */
row_t *get_last_row_pointer(table_t *table)
{
    row_t *row = table->first_row;

    while(row->next != NULL)
    {
        row = row->next;
    }

    return row;
}

/*
 * Funkce slouzi pro pridani novych radku.
 */
uint8_t add_new_rows(table_t *table, unsigned int difference)
{
    row_t *row;
    uint8_t error = NO_ERROR;
    unsigned int count_of_cols = get_count_of_cols(table, &error);
    unsigned int count;

    if(error != NO_ERROR)
    {
        return error;
    }

    while(difference > 0)
    {
        if(!append_row(table))
        {
            return FULL_HEAP;
        }

        row = get_last_row_pointer(table);
        count = 0;

        while(count < count_of_cols)
        {
            if(!append_cell(row, ""))
            {
                return FULL_HEAP;
            }

            count++;
        }

        difference--;
    }

    return NO_ERROR;
}

/*
 * Funkce slouzi pro pridani novych sloupcu.
 */
uint8_t add_new_cols(table_t *table, unsigned int difference)
{
    row_t *row = table->first_row;
    unsigned int count;

    while(row != NULL)
    {
        count = difference;

        while(count > 0)
        {
            if(!append_cell(row, ""))
            {
                return FULL_HEAP;
            }

            count--;
        }

        row = row->next;
    }

    return NO_ERROR;
}

/*
 * Funkce slouzi pro vlozeni jednoho prazdneho radku pred vybrane bunky.
 */
uint8_t irow(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    row_t *row = get_row_pointer(table, selection->first_row, &error);
    unsigned int second_selection = selection->second_row;
    argument->command_value = NULL;

    if(error != NO_ERROR)
    {
        return error;
    }

    for(unsigned int i = selection->first_row; i <= second_selection; i++)
    {
        error = add_row_before(table, row);

        if(error != NO_ERROR)
        {
            break;
        }

        row = row->next;
        i++;
        second_selection++;
    }

    return error;
}

/*
 * Funkce slouzi pro vlozeni jednoho prazdneho radku za vybrane bunky.
 */
uint8_t arow(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    row_t *row = get_row_pointer(table, selection->first_row, &error);
    unsigned int second_selection = selection->second_row;
    argument->command_value = NULL;

    if(error != NO_ERROR)
    {
        return error;
    }

    for(unsigned int i = selection->first_row; i <= second_selection; i++)
    {
        error = add_row_after(table, row);

        if(error != NO_ERROR)
        {
            break;
        }

        row = row->next->next;
        i++;
        second_selection++;
    }

    return error;
}

/*
 * Funkce slouzi pro pridani prazdneho sloupce nalevo od vybranych bunek.
 */
uint8_t icol(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    argument->command_value = NULL;
    unsigned int second_selection = selection->second_col;

    for(unsigned int i = selection->first_col; i <= second_selection; i+=2)
    {
        error = add_column(table, i, __func__);
        second_selection++;

        if(error != NO_ERROR)
        {
            break;
        }
    }

    return error;
}

/*
 * Funkce slouzi pro pridani prazdneho sloupce napravo od vybranych bunek.
 */
uint8_t acol(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    argument->command_value = NULL;
    unsigned int second_selection = selection->second_col;

    for(unsigned int i = selection->first_col; i <= second_selection; i+=2)
    {
        error = add_column(table, i, __func__);
        second_selection++;

        if(error != NO_ERROR)
        {
            break;
        }
    }

    return error;
}

/*
 * Funkce slouzi pro nastaveni prazdneho obsahu vybranych bunek.
 */
uint8_t clear(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    row_t *row = table->first_row;
    cell_t *cell;
    unsigned int row_counter;
    unsigned int col_counter;
    argument->command_value = NULL;

    if(row == NULL)
    {
        error =  NULL_ROW_POINTER;
    }
    else
    {
        for(unsigned int i = selection->first_row; i <= selection->second_row; i++)
        {
            row_counter = 1;

            /*
             * Vyber radku.
             */
            while(row->next != NULL && row_counter < i)
            {
                row = row->next;
                row_counter++;
            }

            cell = row->first_cell;

            for(unsigned int j = selection->first_col; j <= selection->second_col; j++)
            {
                col_counter = 1;

                while(cell->next != NULL && col_counter < j)
                {
                    cell = cell->next;
                    col_counter++;
                }

                error = set_cell_empty_value(cell);

                if(error != NO_ERROR)
                {
                    return error;
                }
            }
        }
    }

    return error;
}

/*
 * Funkce slouzi pro smazani vybranych radku.
 */
uint8_t drow(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error;
    argument->command_value = NULL;

    for(unsigned int i = selection->second_row; i >= selection->first_row; i--)
    {
        error = delete_row(table, i);

        if(error != NO_ERROR)
        {
            break;
        }
    }

    return error;
}

/*
 * Funkce slouzi pro smazani vybranych sloupcu.
 */
uint8_t dcol(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error;
    argument->command_value = NULL;

    for(unsigned int i = selection->second_col; i >= selection->first_col; i--)
    {
        error = delete_column(table, i);

        if(error != NO_ERROR)
        {
            break;
        }
    }

    if(table->first_row->first_cell == NULL)
    {
        selection->first_row = 1;
        selection->second_row = get_count_of_rows(table, &error);
        selection->first_col = 1;
        selection->second_col = get_count_of_cols(table, &error);

        drow(table, selection, argument);
    }

    return error;
}

/*
 * Funkce slouzi pro nastaveni retezce do vybranych bunek.
 */
uint8_t set(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    row_t *row = table->first_row;
    cell_t *cell;
    unsigned int row_counter;
    unsigned int col_counter;

    if(row == NULL)
    {
        error =  NULL_ROW_POINTER;
    }
    else
    {
        for(unsigned int i = selection->first_row; i <= selection->second_row; i++)
        {
            row_counter = 1;

            /*
             * Vyber radku.
             */
            while(row->next != NULL && row_counter < i)
            {
                row = row->next;
                row_counter++;
            }

            cell = row->first_cell;

            for(unsigned int j = selection->first_col; j <= selection->second_col; j++)
            {
                col_counter = 1;

                while(cell->next != NULL && col_counter < j)
                {
                    cell = cell->next;
                    col_counter++;
                }

                error = set_cell_new_value(cell, argument->command_value);

                if(error != NO_ERROR)
                {
                    return error;
                }

                cell = row->first_cell;
            }

            row = table->first_row;
        }
    }

    return error;
}

/*
 * Funkce slouzi pro nastaveni pozice bunky v tabulce.
 */
uint8_t set_cell_position(argument_t *argument,
                          unsigned int *row,
                          unsigned int *col)
{
    uint8_t error = NO_ERROR;
    unsigned int index = 1;

    if(argument->command_value[0] != '[')
    {
        return WRONG_ARGUMENT_VALUE;
    }

    *row = get_command_num_value(argument->command_value, &index, &error);

    if(error != NO_ERROR)
    {
        return WRONG_ARGUMENT_VALUE;
    }

    *col = get_command_num_value(argument->command_value, &index, &error);

    if(error != NO_ERROR)
    {
        return WRONG_ARGUMENT_VALUE;
    }

    return error;
}

/*
 * Funkce slouzi pro nastaveni rozmeru tabulky.
 */
uint8_t set_table_size(table_t *table, unsigned int *rows, unsigned int *cols)
{
    uint8_t error = NO_ERROR;

    *rows = get_count_of_rows(table, &error);

    if(error != NO_ERROR)
    {
        return error;
    }

    *cols = get_count_of_cols(table, &error);

    return error;
}

/*
 * Funkce slouzi pro vymenu obsahu bunek.
 * Postupne jsou prochazeny vybrane bunky pomoci selekce.
 * Jejich obsah je vymenovan s obsahem bunky, zadane jako argument operace
 * swap.
 * Tvar operace swap: swap [R,C].
 */
uint8_t swap(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    char *tmp;
    char *selected_cell_value;
    unsigned int row;
    unsigned int col;
    unsigned int count_of_rows;
    unsigned int count_of_cols;
    cell_t *cell;
    cell_t *cell_ptr;

    error = set_cell_position(argument, &row, &col);

    if(error != NO_ERROR)
    {
        return error;
    }

    error = set_table_size(table, &count_of_rows, &count_of_cols);

    if(error != NO_ERROR)
    {
        return error;
    }

    error = set_table_size(table, &count_of_rows, &count_of_cols);

    if(error != NO_ERROR)
    {
        return WRONG_ARGUMENT_VALUE;
    }

    if(col > count_of_cols)
    {
        error = add_new_cols(table, col - count_of_cols);

        if(error != NO_ERROR)
        {
            return error;
        }
    }

    cell_ptr = get_cell_pointer(table, row, col, &error);

    if(error != NO_ERROR)
    {
        return error;
    }

    /*
     * Postupna iterace jednotlivymi bunkami vybranymi pomoci selekce
     * a vymena obsahu bunek.
     */
    for(unsigned int i = selection->first_row; i <= selection->second_row; i++)
    {
        for(unsigned int j = selection->first_col; j <= selection->second_col; j++)
        {
            tmp = get_cell_value(row, col, table, &error);

            if(error != NO_ERROR)
            {
                return error;
            }

            tmp = strdup(tmp);

            if(tmp == NULL)
            {
                return FULL_HEAP;
            }

            selected_cell_value = get_cell_value(i, j, table, &error);
            selected_cell_value = strdup(selected_cell_value);

            if(selected_cell_value == NULL)
            {
                return FULL_HEAP;
            }

            if(error != NO_ERROR)
            {
                return error;
            }

            error = set_cell_new_value(cell_ptr, selected_cell_value);

            if(error != NO_ERROR)
            {
                return error;
            }

            cell = get_cell_pointer(table, i, j, &error);

            if(error != NO_ERROR)
            {
                return error;
            }

            error = set_cell_new_value(cell, tmp);

            free(tmp);
            tmp = NULL;

            free(selected_cell_value);
            selected_cell_value = NULL;

            if(error != NO_ERROR)
            {
                return error;
            }
        }
    }

    return error;
}

/*
 * Funkce slouzi pro prevod cisla datoveho typu unsigned int
 * do retezce typu char *.
 */
char *utoa(unsigned int value, char *str, int base)
{
    const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i, j;
    unsigned remainder;
    char c;

    if(base < 2 || base > 36)
    {
        str[0] = '\0';
        return NULL;
    }

    i = 0;
    do
    {
        remainder = value % base;
        str[i++] = digits[remainder];
        value = value / base;
    }while (value != 0);

    str[i] = '\0';

    for(j = 0, i--; j < i; j++, i--)
    {
        c = str[j];
        str[j] = str[i];
        str[i] = c;
    }

    return str;
}

/*
 * Funkce vrati delku (pocet pozic) daneho cisla.
 */
unsigned int number_places(unsigned int number) {
    unsigned int counter = 1;

    while(number > 9) {
        number /= 10;
        counter++;
    }

    return counter;
}

/*
 * Funkce slouzi pro nastaveni poctu neprazdnych bunek do dane bunky.
 */
uint8_t count(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    unsigned int row;
    unsigned int col;
    unsigned int count_of_rows;
    unsigned int count_of_cols;
    char *cell;
    cell_t *cell_ptr;
    unsigned int count = 0;

    error = set_cell_position(argument, &row, &col);

    if(error != NO_ERROR)
    {
        return error;
    }

    error = set_table_size(table, &count_of_rows, &count_of_cols);

    if(error != NO_ERROR)
    {
        return error;
    }

    error = set_table_size(table, &count_of_rows, &count_of_cols);

    if(error != NO_ERROR)
    {
        return WRONG_ARGUMENT_VALUE;
    }

    if(col > count_of_cols)
    {
        error = add_new_cols(table, col - count_of_cols);

        if(error != NO_ERROR)
        {
            return error;
        }
    }

    for(unsigned int i = selection->first_row; i <= selection->second_row; i++)
    {
        for(unsigned int j = selection->first_col; j <= selection->second_col; j++)
        {
            cell = get_cell_value(i, j, table, &error);

            if(error != NO_ERROR)
            {
                return error;
            }

            if(strcmp(cell, "") != 0)
            {
                count++;
            }
        }
    }

    cell_ptr = get_cell_pointer(table, row, col, &error);

    if(error != NO_ERROR)
    {
        return error;
    }

    /*
     * Maximalni pocet cifer count pro tabulku a znak \0.
     */
    char buffer[number_places(count) + 1];

    utoa(count, buffer, 10);
    error = set_cell_new_value(cell_ptr, buffer);

    return error;
}

/*
 * Funkce slouzi pro nastaveni delky retezce aktualne vybrane bunky
 * pomoci selekce do dane bunky.
 */
uint8_t len(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    unsigned int row;
    unsigned int col;
    unsigned int count_of_rows;
    unsigned int count_of_cols;
    char *cell;
    cell_t *cell_ptr;
    unsigned int length;

    if(selection->first_row != selection->second_row ||
       selection->first_col != selection->second_col)
    {
        return WRONG_SELECTION_ARG;
    }

    error = set_cell_position(argument, &row, &col);

    if(error != NO_ERROR)
    {
        return error;
    }

    error = set_table_size(table, &count_of_rows, &count_of_cols);

    if(error != NO_ERROR)
    {
        return error;
    }

    error = set_table_size(table, &count_of_rows, &count_of_cols);

    if(error != NO_ERROR)
    {
        return WRONG_ARGUMENT_VALUE;
    }

    if(col > count_of_cols)
    {
        error = add_new_cols(table, col - count_of_cols);

        if(error != NO_ERROR)
        {
            return error;
        }
    }

    cell = get_cell_value(selection->first_row, selection->first_col,
                          table, &error);

    if(error != NO_ERROR)
    {
        return error;
    }

    length = strlen(cell);

    cell_ptr = get_cell_pointer(table, row, col, &error);

    if(error != NO_ERROR)
    {
        return error;
    }

    /*
     * Maximalni pocet cifer count pro tabulku a znak \0.
     */
    char buffer[number_places(length) + 1];

    utoa(length, buffer, 10);
    error = set_cell_new_value(cell_ptr, buffer);

    return error;
}

/*
 * Funkce slouzi pro spolecny proces operaci sum a avg.
 */
uint8_t process_sum_avg(table_t *table, selection_t *selection,
                        argument_t *argument, const char *function_name)
{
    uint8_t error = NO_ERROR;
    unsigned int row;
    unsigned int col;
    unsigned int count_of_rows;
    unsigned int count_of_cols;
    unsigned int count;
    double number;
    double sum;
    char *cell;
    cell_t *cell_ptr;

    count = 0;
    sum = 0.0;
    error = set_cell_position(argument, &row, &col);

    if(error != NO_ERROR)
    {
        return error;
    }

    error = set_table_size(table, &count_of_rows, &count_of_cols);

    if(error != NO_ERROR)
    {
        return WRONG_ARGUMENT_VALUE;
    }

    if(col > count_of_cols)
    {
        error = add_new_cols(table, col - count_of_cols);

        if(error != NO_ERROR)
        {
            return error;
        }
    }

    if(row > count_of_rows)
    {
        error = add_new_rows(table, row - count_of_rows);

        if(error != NO_ERROR)
        {
            return error;
        }
    }

    for(unsigned int i = selection->first_row; i <= selection->second_row; i++)
    {
        for(unsigned int j = selection->first_col; j <= selection->second_col; j++)
        {
            cell = get_cell_value(i, j, table, &error);

            if(error != NO_ERROR)
            {
                return error;
            }

            if(is_number(cell))
            {
                number = atof(cell);
                sum += number;
                count++;
            }
        }
    }

    char string[MAXFLOATINGPOINTSIZE];
    cell_ptr = get_cell_pointer(table, row, col, &error);

    if(error != NO_ERROR)
    {
        return error;
    }

    if(strcmp(function_name, "sum") == 0)
    {
        sprintf(string, "%g", sum);
    }
    else
    {
        sprintf(string, "%g", sum / count);
    }

    error = set_cell_new_value(cell_ptr, string);

    return error;
}

/*
 * Funkce slouzi pro ulozeni souctu hodnot vybranych bunek
 * pomoci selekce do dane bunky.
 */
uint8_t sum(table_t *table, selection_t *selection, argument_t *argument)
{
    return process_sum_avg(table, selection, argument, __func__ );
}

/*
 * Funkce slouzi pro ulozeni aritmetickeho prumeru vybranych bunek
 * pomoci selekce do dane bunky.
 */
uint8_t avg(table_t *table, selection_t *selection, argument_t *argument)
{
    return process_sum_avg(table, selection, argument, __func__ );
}

/*
 * Funkce slouzi pro obnoveni vyberu selekce z docasne promenne [_].
 */
void recovery_tmp_variable(selection_t *selection, argument_t *argument)
{
    selection->first_row = argument->tmp_var->tmp_var_selection.first_row;
    selection->second_row = argument->tmp_var->tmp_var_selection.second_row;
    selection->first_col = argument->tmp_var->tmp_var_selection.first_col;
    selection->second_col = argument->tmp_var->tmp_var_selection.second_col;

    return;
}

/*
 * Funkce slouzi pro nastaveni docasne promenne [_] pomoci operace [set].
 */
void set_tmp_variable(selection_t *selection, argument_t *argument)
{
    argument->tmp_var->tmp_var_selection.first_row = selection->first_row;
    argument->tmp_var->tmp_var_selection.second_row = selection->second_row;
    argument->tmp_var->tmp_var_selection.first_col = selection->first_col;
    argument->tmp_var->tmp_var_selection.second_col = selection->second_col;

    return;
}

/*
 * Funkce vraci pravdivostni hodnotu, zda se jedna o validni hodnotu
 * ciselnych hodnot argumentu (0 az 9).
 */
bool is_valid_variable(char *command_value)
{
    bool state = true;

    if(command_value[0] != '_' ||
       strlen(command_value) != ARGUMENTVALUELENGTH)
    {
        state = false;
    }

    if(command_value[1] < '0' ||
       command_value[1] > '9')
    {
        state = false;
    }

    return state;
}

/*
 * Funkce slouzi pro nastaveni hodnoty aktualni bunky do docasne promenne.
 * Tvar operace: def _X.
 */
uint8_t def(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    cell_t *cell_ptr;
    unsigned int position;

    if(selection->first_row != selection->second_row ||
       selection->first_col != selection->second_col)
    {
        return WRONG_SELECTION_ARG;
    }

    if(!is_valid_variable(argument->command_value))
    {
        return WRONG_ARGUMENT_VALUE;
    }

    /*
     * Ve formatu def _X je hodnota command_value _X.
     * Hodnota X v rozsahu 0 az 9 se nachazi pri indexaci
     * od 0 na nasledujici druhe pozici, tedy na indexu 1.
     */
    position = ((unsigned int) argument->command_value[1]) - '0';
    cell_ptr = get_cell_pointer(table, selection->first_row,
                                selection->first_col, &error);

    if(error != NO_ERROR)
    {
        return error;
    }

    free(argument->tmp_var->tmp_variables[position]);
    argument->tmp_var->tmp_variables[position] = NULL;
    argument->tmp_var->tmp_variables[position] = strdup(cell_ptr->cell);

    if(argument->tmp_var->tmp_variables[position] == NULL)
    {
        return FULL_HEAP;
    }

    return error;
}

/*
 * Funkce slouzi pro nastaveni hodnoty aktualni bunky z docasne promenne.
 * Tvar operace: use _X
 */
uint8_t use(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    cell_t *cell_ptr;
    unsigned int position;

    if(!is_valid_variable(argument->command_value))
    {
        return WRONG_ARGUMENT_VALUE;
    }

    /*
     * Ve formatu use _X je hodnota command_value _X.
     * Hodnota X v rozsahu 0 az 9 se nachazi pri indexaci
     * od 0 na nasledujici druhe pozici, tedy na indexu 1.
     */
    position = ((unsigned int) argument->command_value[1]) - '0';

    for(unsigned int i = selection->first_row; i <= selection->second_row; i++)
    {
        for(unsigned int j = selection->first_col; j <= selection->second_col; j++)
        {
            cell_ptr = get_cell_pointer(table, i, j, &error);

            if(error != NO_ERROR)
            {
                return error;
            }

            error = set_cell_new_value(cell_ptr, argument->tmp_var->tmp_variables[position]);

            if(error != NO_ERROR)
            {
                return error;
            }
        }
    }

    return error;
}

/*
 * Funkce slouzi pro zvetseni numericke hodnoty v docasne promenne o 1.
 * Pokud se nejedna o numerickou hodnotu,
 * bude hodnota teto promenne nastavena na hodnotu 1.
 */
uint8_t inc(table_t *table, selection_t *selection, argument_t *argument)
{
    uint8_t error = NO_ERROR;
    unsigned int position;
    double value;
    char string[MAXFLOATINGPOINTSIZE];

    if(table == NULL)
    {
        return NULL_TABLE_POINTER;
    }

    if(selection == NULL)
    {
        return NULL_SELECTION_POINTER;
    }

    if(!is_valid_variable(argument->command_value))
    {
        return WRONG_ARGUMENT_VALUE;
    }

    /*
     * Ve formatu inc _X je hodnota command_value _X.
     * Hodnota X v rozsahu 0 az 9 se nachazi pri indexaci
     * od 0 na nasledujici druhe pozici, tedy na indexu 1.
     */
    position = ((unsigned int) argument->command_value[1]) - '0';

    if(is_number(argument->tmp_var->tmp_variables[position]))
    {
        value = atof(argument->tmp_var->tmp_variables[position]);
        value += 1.0;
    }
    else
    {
        value = 1.0;
    }

    sprintf(string, "%g", value);
    free(argument->tmp_var->tmp_variables[position]);
    argument->tmp_var->tmp_variables[position] = NULL;
    argument->tmp_var->tmp_variables[position] = strdup(string);

    if( argument->tmp_var->tmp_variables[position] == NULL)
    {
        return FULL_HEAP;
    }

    return error;
}

/*
 * Funkce pro binarni vyhledavani slouzici k nalezeni indexu funkce
 * v poli struktur functions_map.
 */
int binary_search(char *function_name,
                  function_t *functions_map,
                  int size_of_func_map,
                  uint8_t *error)
{
	int left, right, middle, result_of_compare;
	left = 0;
	right = size_of_func_map - 1;

	while(left <= right)
	{
		middle = (left + right) / 2;

		result_of_compare = strcmp(function_name, functions_map[middle].name);
		if(result_of_compare == 0)
		{
			return middle;
		}

		else if(result_of_compare > 0)
		{
			left = middle + 1;
		}
		else
		{
			right = middle - 1;
		}
	}

    *error = ERROR_FUNCTION_NAME;
    return 0;
}

/*
 * Funkce slouzi pro nastaveni hodnoty operace.
 */
void set_command_value(argument_t *argument, char *command)
{
    if(strchr(command, ' ') != NULL)
    {
        //command obsahuje mezeru
        int i=0;
        while(command[i] != ' ')
        {
            i++;
        }

        argument->command_value = &command[++i];
    }

    return;
}

/*
 * Funkce slouzi pro ziskani jmena operace z retezce commnad typu char *.
 */
char *get_function_name(char *command)
{
    if(strchr(command, ' ') != NULL)
    {
        //command obsahuje mezeru
        int i=0;
        while(command[i] != ' ')
        {
            i++;
        }

        command[i] = '\0';
    }

    return command;
}

/*
 * Funkce slouzi pro pridani sloupcu za ucelem nastaveni
 * poctu sloupcu tabulky dle vyberu selekce.
 */
uint8_t add_cols(selection_t *selection,
                 table_t *table,
                 unsigned int count_of_cols)
{
    row_t *row = table->first_row;
    unsigned int count = count_of_cols;

    while(row != NULL)
    {
        while(count < selection->second_col)
        {
            if(!append_cell(row, ""))
            {
                return FULL_HEAP;
            }

            count++;
        }

        row = row->next;
        count = count_of_cols;
    }

    return NO_ERROR;
}

/*
 * Funkce slouzi pro pridani radku za ucelem nastaveni
 * poctu radku tabulky dle vyberu selekce.
 */
uint8_t add_rows(selection_t *selection,
                 table_t *table,
                 unsigned int count_of_rows)
{
    row_t *row;
    unsigned int count_of_cols;

    while(count_of_rows < selection->second_row)
    {
        count_of_cols = 0;

        if(!append_row(table))
        {
            return FULL_HEAP;
        }

        row = get_last_row_pointer(table);

        /*
         * Pridani bunek do noveho radku.
         */
        while(count_of_cols < selection->second_col)
        {
            /*
             * Pridani bunek
             */
            if(!append_cell(row, ""))
            {
                return FULL_HEAP;
            }

            count_of_cols++;
        }

        count_of_rows++;
    }

    return NO_ERROR;
}

/*
 * Funkce slouzi pro zjisteni, zda je potreba doplnit bunky tabulky
 * pri danem vyberu selekce.
 * Dle vyberu postupne doplnuje sloupce a radky.
 */
uint8_t check_selection(selection_t *selection, table_t *table)
{
    uint8_t error = NO_ERROR;
    unsigned int count_of_rows;
    unsigned int count_of_cols;

    count_of_rows = get_count_of_rows(table, &error);

    if(error != NO_ERROR)
    {
        return error;
    }

    count_of_cols = get_count_of_cols(table, &error);

    if(error != NO_ERROR)
    {
        return error;
    }

    if(selection->second_col > count_of_cols)
    {
        error = add_cols(selection, table, count_of_cols);

        if(error != NO_ERROR)
        {
            return error;
        }
    }

    if(selection->second_row > count_of_rows)
    {
        error = add_rows(selection, table, count_of_rows);
    }

    return error;
}

/*
 * Funkce prida bunky do noveho radku.
 */
uint8_t add_cols_one_row(row_t *row, unsigned int difference)
{
    while(difference > 0)
    {
        if(!append_cell(row, ""))
        {
            return FULL_HEAP;
        }

        difference--;
    }

    return NO_ERROR;
}

/*
 * Vracena hodnota poctu bunek v danem radku.
 */
unsigned int get_count_of_row_cols(row_t *row)
{
    unsigned int count = 0;
    cell_t *cell = row->first_cell;

    while (cell != NULL)
    {
        cell = cell->next;
        count++;
    }

    return count;
}

/*
 * Funkce slouzi pro zajisteni stejneho poctu bunek ve vsech radcich tabulky.
 * Pri rozdilu jsou bunky doplneny zprava.
 */
uint8_t check_cells_count(table_t *table)
{
    uint8_t error = NO_ERROR;
    row_t *row = table->first_row;
    unsigned int count_row;
    unsigned int count_row_second;
    unsigned int difference;

    if(row == NULL)
    {
        return NULL_ROW_POINTER;
    }

    row_t *row_second = row->next;
    count_row = get_count_of_row_cols(row);

    while(row->next != NULL)
    {
        while (row_second != NULL)
        {
            count_row_second = get_count_of_row_cols(row_second);

            if(count_row != count_row_second)
            {
                if(count_row > count_row_second)
                {
                    difference = count_row - count_row_second;
                    error = add_cols_one_row(row_second, difference);
                }
                else
                {
                    difference = count_row_second - count_row;
                    error = add_cols_one_row(row, difference);
                }
            }

            if(error != NO_ERROR)
            {
                return error;
            }

            row_second = row_second->next;
        }

        count_row = get_count_of_row_cols(table->first_row);
        row_second = row->next;
        row = row->next;
    }

    return error;
}

/*
 * Slouzi pro prochazeni argumentu zadanych na vstupu. Vraci hodnotu NO_ERROR
 * v pripadu spravneho prubehu programu.
 * V opacnem pripadu vraci hodnotu chyboveho stavu.
 */
uint8_t process_arguments(table_t *table, argument_t *argument,
                          function_t *functions_map, int map_size)
{
    uint8_t error = NO_ERROR;
    selection_t selection;
    char *command;
    int function_index;

    error = init_selection(&selection);

    if(error != NO_ERROR)
    {
        return error;
    }

    /*
     * Kontrola, zda vsechny radky obsahuji stejny pocet bunek.
     * V opacnem pripadu doplni chybejici bunky.
     */
    error = check_cells_count(table);

    if(error != NO_ERROR)
    {
        return error;
    }

    int i=0;
    while(argument->cmd_sequence[i] != NULL)
    {
        command = argument->cmd_sequence[i];

        if(command[0] == '[')
        {
            //[_], [set]
            if(strcmp(command, "[_]") == 0)
            {
                recovery_tmp_variable(&selection, argument);
            }
            else if(strcmp(command, "[set]") == 0)
            {
                set_tmp_variable(&selection, argument);
            }
            else
            {
                error = set_selection(command, &selection, table);

                if(error != NO_ERROR)
                {
                    break;
                }

                /*
                 * Kontrola, zda vyber bunek nepresahuje hranice tabulky.
                 * V opacnem pripadu doplni chybejici bunky.
                 */
                error = check_selection(&selection, table);
            }
        }
        else
        {
            set_command_value(argument, command);

            /*
             * functions_map[function_index].function(...)
             * - Slouzi pro zavolani funkce. Dle pozice nazvu funkce v poli struktur
             *   functions_map pouzije ukazatel na funkci, definovany na stejnem
             *   indexu ve vyse zminenem poli.
             */
            function_index = binary_search(get_function_name(command),
                                           functions_map,
                                           map_size,
                                           &error);

            if(error == NO_ERROR)
            {
                error = functions_map[function_index].function(table,
                                                               &selection,
                                                               argument);
            }
        }

        if(error != NO_ERROR)
        {
            break;
        }

        i++;
    }

    return error;
}

/*
 * Funkce slouzi pro nastaveni pole struktur obsahujici ukazatele na funkce
 * a nazvy funkci. Dale nastavuje velikost daneho pole.
 * Vraci navratovou hodnotu funkce process_arguments.
 */
uint8_t process_functions(table_t *table, argument_t *argument)
{
    /*
     * Funkce jsou serazeny abecedne z duvodu pouziti binary search
     * pro vyhledavani v poli functions_map.
     */
    function_t functions_map[] = {
        FUNCTION(acol)
        FUNCTION(arow)
        FUNCTION(avg)
        FUNCTION(clear)
        FUNCTION(count)
        FUNCTION(dcol)
        FUNCTION(def)
        FUNCTION(drow)
        FUNCTION(icol)
        FUNCTION(inc)
        FUNCTION(irow)
        FUNCTION(len)
        FUNCTION(set)
        FUNCTION(sum)
        FUNCTION(swap)
        FUNCTION(use)
};

    int map_size = (sizeof(functions_map) / sizeof(functions_map[0]));

    return process_arguments(table, argument, functions_map, map_size);
}

/*
 * Slouzi pro vytvoreni a zaalokovani mista pro tabulku.
 */
table_t *create_table(void)
{
    return (table_t *) malloc(sizeof(table_t));
}

/*
 * Slouzi pro uvolneni pameti na heap pro sekvenci operaci (cmd_sequence).
 */
void dispose_cmd_sequence(argument_t *argument)
{
    unsigned int i=0;

    while(argument->cmd_sequence[i] != NULL)
    {
        free(argument->cmd_sequence[i]);
        argument->cmd_sequence[i] = NULL;
        i++;
    }

    free(argument->cmd_sequence);
    argument->cmd_sequence = NULL;

    return;
}

/*
 * Slouzi pro uvolneni pameti na heap pro docasne promenne.
 */
void dispose_tmp_var(argument_t *argument)
{
    unsigned int i=0;

    while(i < COUNTOFTMPVARIABLES)
    {
        free(argument->tmp_var->tmp_variables[i]);
        argument->tmp_var->tmp_variables[i] = NULL;
        i++;
    }

    free(argument->tmp_var->tmp_variables);
    argument->tmp_var->tmp_variables = NULL;

    free(argument->tmp_var);
    argument->tmp_var = NULL;

    return;
}

/*
 * Slouzi pro uvoleni mista na heap pro tabulky vcetne jejich bunek a radku.
 */
uint8_t dispose_table(table_t *table)
{
    row_t *row;
    cell_t *cell;
    uint8_t error;
    unsigned int row_position;
    unsigned int cell_position;


    if (table->first_row != NULL)
    {
        row = table->first_row;
        row_position = 1;

        while (row != NULL)
        {
            cell_position = 1;

            cell = row->first_cell;

            while (cell != NULL)
            {
                while (cell->next != NULL)
                {
                    cell = cell->next;
                    cell_position++;
                }

                error = delete_cell(row, cell_position);

                if(error != NO_ERROR)
                {
                    return error;
                }

                cell = row->first_cell;
                cell_position = 1;
            }

            row = row->next;
            error = delete_row(table, row_position);

            if(error != NO_ERROR)
            {
                break;
            }
        }
    }

    free(table);
    table = NULL;

    return error;
}

/*
 * Slouzi pro uvolneni pameti na heap. Uvolnuje nasledujici prvky:
 * - sekvenci operaci
 * - docasne promenne
 * - celou tabulku vcetne jejich bunek a radku
 */
uint8_t dispose(table_t *table, argument_t *argument, uint8_t *printed_error)
{
    uint8_t error = NO_ERROR;

    if(*printed_error != WRONG_ARGS_COUNT)
    {
        dispose_cmd_sequence(argument);
        dispose_tmp_var(argument);
    }

    error = dispose_table(table);

    return error;
}

/*
 * Funkce slouzi pro vypis chybovych hlaseni na stderr.
 */
void print_error(uint8_t error)
{
    /*
     * Chybove hlaseni pro jednotlive chybove stavy.
     * Jejich poradi odpovida poradi konstant v enum errors.
     */
    const char *error_msg[] =
    {
        "Pri behu programu nenastala zadna chyba.",
        "Nespravny pocet argumentu.",
        "Nespravny format argumentu DELIM.",
        "Nespravny nazev souboru.",
        "Vnitrni chyba programu - nedostatek pameti.",
        "Nespravne zadany argument vlozeni sloupce.",
        "Nespravne zadany argument vlozeni radku.",
        "Chyba pri mazani bunky tabulky - nespravny argument.",
        "Chyba pri mazani radku tabulky - nespravny argument.",
        "Chyba - nedefinovana vstupni tabulka.",
        "Chyba - nedefinovany radek.",
        "Chyba - nedefinovana bunka.",
        "Chyba konce souboru.",
        "Chybny argument selekce.",
        "Nespravny nazev argumentu.",
        "Nespravne poradi argumentu.",
        "Nespravna hodnota funkce",
        "Chyba selekce.",
        "Neznama chyba."
    };

    /*
     * Hodnota chyby je vetsi nez hodnota posledni chyby v enum (UNKNOWN_ERROR).
     * Nastaveni hodnoty chyby na neznamou chybu.
     */
    if(error > UNKNOWN_ERROR)
    {
        error = UNKNOWN_ERROR;
    }

    fprintf(stderr, "Chyba: %s\n", error_msg[error]);
    return;
}

/*
 * Funkce slouzi pro odchytavani chyb pri prubehu jednotlivych casti progarmu.
 * V pripadu bezchybneho prubehu vraci error s hodnotou NO_ERROR.
 */
uint8_t error_catcher(int argc, char **argv)
{
    uint8_t error = NO_ERROR;
    argument_t argument;

    /*
     * Vytvoreni tabulky.
     */
    table_t *table = create_table();

    if(table == NULL)
    {
        error = FULL_HEAP;
        print_error(error);
        dispose(table, &argument, &error);
        return error;
    }

    /*
     * Nastaveni argumentu zadanych na vstupu.
     */
    error = set_arguments(argc, argv, &argument);

    if(error != NO_ERROR)
    {
        print_error(error);
        dispose(table, &argument, &error);
        return error;
    }

    /*
     * Pocatecni inicializace tabulky.
     */
    init_table(table);

    /*
     * Nacteni tabulky ze souboru.
     */
    scan_table(&argument, table, &error);

    if(error != NO_ERROR)
    {
        print_error(error);
        dispose(table, &argument, &error);
        return error;
    }

    /*
     * Pocatecni inicializece docasnych promennych.
     */
    error = init_tmp_var(argument.tmp_var);

    if(error != NO_ERROR)
    {
        return error;
    }

    /*
     * Vykonani jednotlivych operaci zadanych v argumentu.
     */
    error = process_functions(table, &argument);

    if(error != NO_ERROR)
    {
        print_error(error);
        dispose(table, &argument, &error);
        return error;
    }

    /*
     * Smazani nadbytecnych poslednich bunek.
     */
    error = delete_redundant_cells(table);

    if(error != NO_ERROR)
    {
        print_error(error);
        dispose(table, &argument, &error);
        return error;
    }

    /*
     * Vypis tabulky do souboru.
     */
    error = print_table(table, &argument);

    if(error != NO_ERROR)
    {
        print_error(error);
        dispose(table, &argument, &error);
        return error;
    }

    /*
     * V pripadu bezchybneho prubehu uvolneni pameti na heap.
     */
    error = dispose(table, &argument, &error);

    if(error != NO_ERROR)
    {
        print_error(error);
        return error;
    }

    return error;
}

/*
 * Hlavni funkce main.
 */
int main(int argc, char **argv)
{
    /*
     * V pripadu vyskytu chyby ve funkci error_catcher pri prubehu
     * je vracena hodnota EXIT_FAILURE.
     * V opacnem pripadu EXIT_SUCCESS.
     */
    if(error_catcher(argc, argv) != NO_ERROR)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}