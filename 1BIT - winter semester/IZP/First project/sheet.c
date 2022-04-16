/*
 * Soubor: sheet.c
 * Datum: 08.11.2020
 * Autor: David Chocholaty, xchoch09@fit.vutbr.cz
 * Projekt: Prace s textem, 1. pro predmet IZP
 * Popis: Program implementujici zakladni operace tabulkovych procesoru.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#define MAXLENGTHOFLINE 10242
#define MAXLENGTHOFARGUMENT 100
#define MAXLENGTHOFCELL 100
#define FIRSTCHARACTERINDEX 0
#define NOFUNCTIONARGUMENT 1
#define	ONEFUNCTIONARGUMENT 2
#define TWOFUNCTIONARGUMENTS 3
#define NOSELECTIONARGUMENT false
#define SELECTIONARGUMENT true
#define CONDITIONSMET 1
#define CONDITIONSNOTMET 0
#define NULLARGUMENT 0
#define INDEXWITHOUTDELIM 1
#define INDEXWITHDELIM 3
#define FUNCTION(function) {function, #function},

/*
 * Chybove kody, ktere mohou nastat behem programu.
 */
enum errors
{
	NOERROR,                       //< Bez chyby.
	NONNUMERICARGUMENT,            //< Neciselna hodnota argumentu.
	NEGATIVEARGUMENT,              //< Zaporna hodnota.
	ZEROARGUMENT,                  //< Nulova hodnota.
	INVALIDARGUMENT,               //< Nespravne zadane cislo (napriklad 12abc)
	MAXCELLLENGTHEXCEED,           //< Prekroceni maximalni delky bunky.
	ERRORFUNCTIONNAME,             //< Nespravny nazev funkce (operace)
	MAXROWLENGTHEXCEED,            //< Prekroceni maximalni delky radku.
	MAXARGUMENTLENGTHEXCEED,       //< Prekroceni maximalni delky argumentu.
	MISSINGARGUMENT,               //< Chybejici argument funkce (operace).
	EMPTYFILE,                     //< Prazdny soubor jako vstupni data.
	NONNUMERICVALUE,               //< Neciselna hodnota
	WRONGARGUMENTSORDER,           //< Nespravne poradi argumentu.
	UNKNOWNERROR                   //< Neznama chyba.
};

/*
 * Struktura prezentujici vstupni data argumentu prikazoveho radku.
 */
typedef struct{
	int argc;
	char **argv;
	char *delim;
} argument_t;

/*
 * Struktura prezentujici data spusteneho programu.
 */
typedef struct{
	int actual_index;
	int number_of_row;
	int count_of_delims;
	int index_first_char;
	int length_original;
	int length_argument;
} process_t;

/*
 * Struktura prezentujici funkci dane operace.
 */
typedef struct{
	int (*function)(char *, argument_t *, process_t *);
	const char *name;
} function_t;

/*
 * Struktura prezentujici sloupec ze vstupniho radku.
 */
typedef struct{
	char *argument_string;
	int argument_column;
} column_t;

/*
 * Struktura prezentujici ciselne argumenty operace.
 */
typedef struct{
	int first_argument_column;
	int second_argument_column;
} arg_column_t;

/*
 * Nastaveni delimiteru. Pokud bude zadan prepinac -d DELIM, nastavi delimiter
 * na tuto zadanou hodnotu. V opacnem pripadu nastavi delimiter na znak mezery.
 */
void set_delim(argument_t *argument)
{
	//Prvni argument zadany od uzivatele se nachazi na indexu 1.
	if((argument->argc > 2) && (strcmp(argument->argv[1], "-d") == 0))
	{
		argument->delim = argument->argv[2];
	}
	else
	{
		argument->delim = " ";
	}

	return;
}

/*
 * Nastaveni argumentu do struktury.
 */
void set_argument(int argc, char **argv, argument_t *argument)
{
	argument->argc = argc;
	argument->argv = argv;

	return;
}

/*
 * Nastaveni vychoziho indexu. Pokud byl zadan jako argument prepinac -d
 * a hodnota DELIM, je vychozi index INDEXWITHDELIM. V opacnem pripadu
 * je nastavena vychozi hodnota INDEXWITHOUTDELIM.
 */
void set_default_index(argument_t *argument, process_t *process)
{
	if(strcmp(argument->delim, " ") == 0)
	{
		process->actual_index = INDEXWITHOUTDELIM;
	}
	else
	{
		process->actual_index = INDEXWITHDELIM;
	}

	return;
}

/*
 * Nastaveni vychozi hodnoty poctu delimiteru v radku a cisla radku.
 */
void set_process(process_t *process)
{
	process->count_of_delims = 0;
	process->number_of_row = 1;

	return;
}

/*
 * Nastaveni aktualniho indexu v poli argv (uchovane ve strukture
 * typu argument_t).
 */
void set_actual_index(int number_to_add, process_t *process)
{
	process->actual_index += number_to_add;

	return;
}

/*
 * Testovani argumentu, zda se jedna o ciselnou hodnotu. Z duvodu pouziti
 * funkce atoi vracejici hodnotu 0 pro znak 0 a take pro neplatny vstup.
 */
bool valid_number_argument(char *argument)
{
	for(long unsigned int i=0; i<strlen(argument); i++)
	{
		if(isdigit(argument[i]) == 0) return false;
	}

	return true;
}

/*
 * Testovani argumentu. Musi splnovat podminky (viz podminky ve funkci):
 * - neni zaporny
 * - neni 0
 * - jedna se o kladne cislo (cislo neobsahuje jeste jine znaky)
 * - nepresahuje maximalni delku argumentu.
 */
int valid_argument(argument_t *argument)
{
	for(int i=0; i<argument->argc; i++)
	{
		if(atoi(argument->argv[i]) < 0)
		{
			return -NEGATIVEARGUMENT;
		}

		if((atoi(argument->argv[i]) == 0) &&
		   (valid_number_argument(argument->argv[i]) == true))
		{
  		return -ZEROARGUMENT;
		}

		if((atoi(argument->argv[i])>0) &&
		   (valid_number_argument(argument->argv[i]) == false))
		{
			return -INVALIDARGUMENT;
		}

		if(strlen(argument->argv[i]) > MAXLENGTHOFARGUMENT)
		{
			return -MAXARGUMENTLENGTHEXCEED;
		}
	}

	return NOERROR;
}

/*
 * Testovani delky vstupniho radku.
 */
bool valid_row(char *buffer, argument_t *argument)
{
	int counter = 0;

	for(long unsigned int i=0; i<strlen(buffer); i++, counter++)
	{
		if(strchr(argument->delim, buffer[i]) == NULL)
		{
			counter = 0;
		}

		if(counter>MAXLENGTHOFCELL)
		{
			return false;
		}

		if(buffer[i]=='\n')
		{
			return true;
		}
	}

	return false;
}

/*
 * Nacitani vstupniho radku.
 * V pripadu konce radku vraci ukazatel na NULL.
 */
char *scan_line(char *buffer)
{
  return fgets(buffer, MAXLENGTHOFLINE, stdin);
}

/*
 * Nastavuje oddelovac vystupnich hodnot do pole buffer.
 */
void set_output_delim(char *buffer, argument_t *argument)
{
	for(int i = 0; buffer[i] != '\0'; i++)
	{
		if(buffer[i] != argument->delim[0] &&
		   strchr(argument->delim, buffer[i]) != NULL)
		{
			buffer[i] = argument->delim[0];
		}
	}

	return;
}

/*
 * Vypisuje jiz upraveny radek.
 */
void print_line(char *buffer, argument_t *argument)
{
	set_output_delim(buffer, argument);
	printf("%s", buffer);

	return;
}

/*
 * Vypisuje novy radek.
 */
void print_new_line(argument_t *argument, process_t *process)
{
	for(int i=0; i<process->count_of_delims; i++)
	{
		printf("%c", argument->delim[0]);
	}

	printf("\n");

	return;
}

/*
 * Slouzi pro smazani radku.
 */
void delete_row(char *buffer)
{
	buffer[0] = '\0';
	return;
}

/*
 * Operace selekce radku. Argument muze byt ve tvaru rows CISLO CISLO nebo
 * ve tvaru rows CISLO -. Tvar argumentu rows - - je testovan v jine
 * casti programu z duvodu upravy pouze posledniho radku.
 */
int rows(argument_t * argument, process_t *process)
{
	char *second_argument = argument->argv[process->actual_index + 2];
	int second_argument_num = atoi(second_argument);
	int first_argument_num = atoi(argument->argv[process->actual_index + 1]);
	int number_of_row =	process->number_of_row;

	set_actual_index(TWOFUNCTIONARGUMENTS, process);

	//Testovani, zda prvni argument neni ve tvaru bezneho textu.
	if(first_argument_num == 0)
	{
		return -NONNUMERICARGUMENT;
	}

	//Operace rows ve tvaru rows CISLO -. Znak "-" znaci az do posledniho radku.
	if(strcmp(second_argument, "-") == 0)
	{
		if(number_of_row >= first_argument_num)
		{
			return CONDITIONSMET;
		}

		return	CONDITIONSNOTMET;
	}

	if(first_argument_num > second_argument_num)
	{
		return -WRONGARGUMENTSORDER;
	}

	//Operace rows ve tvaru rows CISLO CISLO.
	if((number_of_row >= first_argument_num) &&
	   (number_of_row <= second_argument_num))
	{
		return CONDITIONSMET;
	}

	return	CONDITIONSNOTMET;
}

/*
 * Vraci index sloupce v poli buffer. Pokud neni nalezen, vraci INT_MAX.
 */
int get_position(char *buffer, int argument_column, argument_t *argument)
{
	int	count_of_delims = 0;

	int i=0;
	for(; buffer[i]!='\0'; i++)
	{
		//Testovani, zda se jedna o index pozadovaneho sloupce.
		if(count_of_delims == argument_column - 1)
		{
			return i;
		}

		if(strchr(argument->delim, buffer[i]))
		{
			count_of_delims++;
		}
	}

	return INT_MAX;
}

/*
 * Nastavuje hodnotu sloupce z pole buffer do pole argument_string.
 */
void set_column_value(char *buffer, char *argument_string,
                      int position_of_column, argument_t *argument)
{
	int i = position_of_column, j = 0;
	while(buffer[i] != '\n' && strchr(argument->delim, buffer[i]) == NULL)
	{
		argument_string[j] = buffer[i];
		i++;
		j++;
	}

	argument_string[j] = '\0';

	return;
}

/*
 * Testuje, zda se jedna o operaci beginswith nebo contains. Pri nesplneni
 * podminek vradi CONDITIONSNOTMET.
 */
int beginswith_contains(const char *function_name,
                        char *column_value,
                        char *second_argument)
{
	if((strcmp(function_name, "contains")) &&
		 (strncmp(second_argument, column_value, strlen(second_argument)) == 0))
	{
		return CONDITIONSMET;
	}

	if((strcmp(function_name, "beginswith")) &&
		 (strstr(column_value, second_argument) != NULL))
	{
		return CONDITIONSMET;
	}

	return CONDITIONSNOTMET;
}

/*
 * Slouzi pro spolecny proces operaci beginswith a contains. Ziska potrebne
 * hodnoty a vraci navratovou hodnotu funkce beginswith_contains.
 */
int process_beginswith_contains(char *buffer, argument_t *argument,
                                process_t *process, const char *function_name)
{
	char column_value[MAXLENGTHOFCELL + 1];
	int position_of_column;
	int first_argument = atoi(argument->argv[process->actual_index + 1]);
	char *second_argument = argument->argv[process->actual_index + 2];

	set_actual_index(TWOFUNCTIONARGUMENTS, process);

	if(first_argument == 0)
	{
		return -NONNUMERICARGUMENT;
	}

	position_of_column = get_position(buffer, first_argument, argument);
	set_column_value(buffer, column_value, position_of_column, argument);

	return beginswith_contains(function_name, column_value, second_argument);
}

/*
 * Funkce vraci navratovou hodnotu funkce process_beginswith_contains,
 * ktera slouzi pro spolecny proces operaci beginswith a contains.
 */
int beginswith(char *buffer, argument_t *argument, process_t *process)
{
	return process_beginswith_contains(buffer, argument, process, __func__);
}

/*
 * Funkce vraci navratovou hodnotu funkce process_beginswith_contains,
 * ktera slouzi pro spolecny proces operaci beginswith a contains.
 */
int contains(char *buffer, argument_t *argument, process_t *process)
{
	return process_beginswith_contains(buffer, argument, process, __func__);
}

/*
 * Slouzi pro testovani operaci selekce radku. V pripadu splneni podminek vola
 * funkci pro danou operaci. Vraci pravdivostni hodnotu typu bool,
 * zda byla nalezena operace selekce radku.
 */
bool selection_argument(char *buffer, argument_t *argument,
                        process_t *process, int *ptr_state)
{
	if(process->actual_index + 2 < argument->argc){
		if(strcmp(argument->argv[process->actual_index], "rows") == 0)
		{
			//Pripadny chybovy stav je prirazen v zaporne hodnote do ptr_state.
			*ptr_state = rows(argument, process);
		}
		else if(strcmp(argument->argv[process->actual_index], "beginswith") == 0)
		{
			*ptr_state = beginswith(buffer, argument, process);
		}
		else if(strcmp(argument->argv[process->actual_index], "contains") == 0)
		{
			*ptr_state = contains(buffer, argument, process);
		}
		else
		{
			return NOSELECTIONARGUMENT;
		}

		return SELECTIONARGUMENT;
	}

	return NOSELECTIONARGUMENT;
}

/*
 * Testuje jednotlive argumenty, zda se nejedna o operaci selekce radku.
 * Vraci informaci o tom, zda byly splneny podminky selekce radku a take
 * pripadne vraci zapornou hodnotu chyboveho stavu.
 */
int row_selection(char *buffer, argument_t *argument, process_t *process)
{
	int state = CONDITIONSMET;

	while(selection_argument(buffer, argument, process, &state) !=
	      NOSELECTIONARGUMENT)
	{
		if(state != CONDITIONSMET)
		{
			break;
		}
	}

	return state;
}

/*
 * Funkce pro binarni vyhledavani slouzici k nalezeni indexu funkce
 * v poli struktur functions_map.
 */
int binary_search(char *function_name,
                  function_t *functions_map,
                  int size_of_func_map)
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

	return -ERRORFUNCTIONNAME;
}

/*
 * Slouzi pro prochazeni argumentu pro kazdy radek. Vraci hodnotu NOERROR
 * v pripadu spravneho prubehu programu. V opacnem pripadu vraci
 * zapornou hodnotu chyboveho stavu.
 */
int process_arguments(char *buffer, argument_t *argument, process_t *process,
                      function_t *functions_map, int map_size)
{
	int state = NOERROR;

	while(process->actual_index < argument->argc)
	{
		char *function_name = argument->argv[process->actual_index];
		int function_index = binary_search(function_name, functions_map, map_size);

		if(function_index != -ERRORFUNCTIONNAME)
		{
			/*
			 * functions_map[function_index].function(buffer, argument, process)
			 * - Slouzi pro zavolani funkce. Dle pozice nazvu funkce v poli struktur
			 *   functions_map pouzije ukazatel na funkci, definovany na stejnem
			 *   indexu ve vyse zminenem poli.
			 */
			state = functions_map[function_index].function(buffer, argument, process);
		}
		/*
		 * V pripadu nalezeni operace arow bude tento argument preskocen.
		 * Funkce arow se provadi az po vypsani vsech radku tabulky.
		 */
		else if(strcmp(argument->argv[process->actual_index], "arow") == 0)
		{
			set_actual_index(NOFUNCTIONARGUMENT, process);
		}
		/*
		 * Chybovy stav. Nazev operace nebyl nalezen. Jedna se o nespravny vstup.
		 */
		else
		{
			state = -ERRORFUNCTIONNAME;
		}

		if(state < NOERROR)
		{
			return state;
		}
	}

	return state;
}

/*
 * Funkce slouzici pro spocitani poctu delimiteru v radku.
 */
int count_of_delims(char *buffer, argument_t *argument)
{
	int actual_count = 0;

	for(int i=0; buffer[i] != '\0'; i++)
	{
	    if(strchr(argument->delim, buffer[i]) != NULL)
	    {
	    	actual_count++;
	    }
	}

	return actual_count;
}

/*
 * Funkce nastavuje pocet delimiteru.
 */
void set_count_of_delims(char *buffer, argument_t *argument, process_t *process)
{
	process->count_of_delims = count_of_delims(buffer, argument);
	return;
}

/*
 * Funkce vraci hodnotu argumentu. V pripadu chyby vraci zapornou hodnotu
 * chyboveho stavu.
 */
int get_argument(argument_t *argument, process_t *process)
{
	if(process->actual_index + 1 >= argument->argc)
	{
		return -MISSINGARGUMENT;
	}

	int argument_num = atoi(argument->argv[process->actual_index + 1]);

	if(argument_num == 0)
	{
		return -NONNUMERICARGUMENT;
	}

	return argument_num;
}

/*
 * Slouzi pro spolecny proces operaci irow a drow. Testuje, zda se jedna o
 * operaci drow nebo o operaci irow.
 */
void process_irow_drow(char *buffer, argument_t *argument, process_t *process,
                       const char *function_name, int argument_row)
{
	if(process->number_of_row == argument_row)
	{
		if(strcmp(function_name, "drow") == 0)
		{
			delete_row(buffer);
		}
		else
		{
			if(process->count_of_delims == 0)
			{
				set_count_of_delims(buffer, argument, process);
			}

			print_new_line(argument, process);
		}
	}
}

/*
 * Slouzi pro spolecny proces funkci irow a drow.
 */
int irow_drow(char *buffer, argument_t *argument,
              process_t *process, const char *function_name)
{
	int argument_row = get_argument(argument, process);

	/*
	 * V pripadu splneni podminky byl argument zadan nespravne.
	 * Navrat chyboveho stavu.
	 */
	if(argument_row < 0)
	{
			return argument_row;
	}

	process_irow_drow(buffer, argument, process, function_name, argument_row);
	set_actual_index(ONEFUNCTIONARGUMENT, process);

	return NOERROR;
}

/*
 * Vraci navratovou hodnotu funkce pro spolecny proces operaci irow a drow.
 * V pripadu vyskytu chyby vraci zapornou hodnotu chyboveho stavu.
 */
int irow(char *buffer, argument_t *argument, process_t *process)
{
	return irow_drow(buffer, argument, process, __func__);
}

/*
 * Slouzi pro operaci arow.
 */
void arow(char *buffer, argument_t *argument, process_t *process)
{
	if(process->count_of_delims == 0)
	{
		set_count_of_delims(buffer, argument, process);
	}

	print_new_line(argument, process);

	return;
}

/*
 * Testuje, zda se ma provest operace arow.
 */
void check_arow(char *buffer, argument_t *argument, process_t *process)
{
	while(process->actual_index < argument->argc)
	{
		if(strcmp(argument->argv[process->actual_index], "arow") == 0)
		{
			arow(buffer, argument, process);
		}

		set_actual_index(NOFUNCTIONARGUMENT, process);
	}

	return;
}

/*
 * Vraci navratovou hodnotu funkce pro spolecny proces operaci irow a drow.
 * V pripadu vyskytu chyby vraci zapornou hodnotu chyboveho stavu.
 */
int drow(char *buffer, argument_t *argument, process_t *process)
{
	return irow_drow(buffer, argument, process, __func__);
}

/*
 * Slouzi pro operaci drows. Otestuje spravnost argumentu. V pripadu spravneho
 * prubehu s platnosti kriterii zavola funkci delete_row pro smazani radku.
 */
int drows(char *buffer, argument_t *argument, process_t *process)
{
	if(process->actual_index + 2 >= argument->argc)
	{
		return -MISSINGARGUMENT;
	}

	int first_argument_row = atoi(argument->argv[process->actual_index + 1]);
	int second_argument_row = atoi(argument->argv[process->actual_index + 2]);
	int number_of_row = process->number_of_row;

	if(first_argument_row == 0 || second_argument_row == 0)
	{
		return -NONNUMERICARGUMENT;
	}

	if(first_argument_row > second_argument_row)
	{
		return -WRONGARGUMENTSORDER;
	}

	if((number_of_row >= first_argument_row) &&
		 (number_of_row <= second_argument_row))
	{
		delete_row(buffer);
	}

	set_actual_index(TWOFUNCTIONARGUMENTS, process);

	return NOERROR;
}

/*
 * Vraci index konce radku.
 */
int get_end_of_line(char *buffer)
{
	int i=0;
	for(; buffer[i]!='\0'; i++);

	return (i+1);
}

/*
 * Slouzi pro operaci acol. Prekopiruje znaky \0 a \n. Pote vlozi prvni
 * znak delimiteru.
 */
int acol(char *buffer, argument_t *argument, process_t *process)
{
	int end_of_line = get_end_of_line(buffer);
	int i = end_of_line;

	//Prekopirovani pouze znaku \n a \0 (2 znaky -> end_of_line - 2).
	for(; i>=end_of_line-2; i--)
	{
		buffer[i] = buffer[i-1];
	}

	buffer[i+1] = argument->delim[0];

	set_actual_index(NOFUNCTIONARGUMENT, process);

	return NOERROR;
}

/*
 * Slouzi pro operaci icol. Prekopiruje od konce znaky o jeden index
 * a vlozi prvni znak delimiteru.
 */
void process_icol(char *buffer, int argument_column, argument_t *argument)
{
	int position_of_column = get_position(buffer, argument_column, argument);

	if(position_of_column != INT_MAX)
	{
		int end_of_line = get_end_of_line(buffer);

		int i = end_of_line;
		for(; i>position_of_column; i--)
		{
			buffer[i] = buffer[i-1];
		}

		buffer[i] = argument->delim[0];
	}

	return;
}

/*
 * Funkce otestuje spravnost argumentu. V pripadu bezchybneho prubehu
 * vola funkci process_icol.
 */
int icol(char *buffer, argument_t *argument, process_t *process)
{
	int argument_column = get_argument(argument, process);

	if(argument_column < 0)
	{
			return argument_column;
	}

	process_icol(buffer, argument_column, argument);
	set_actual_index(ONEFUNCTIONARGUMENT, process);

	return NOERROR;
}

/*
 * Slouzi pro spolecny proces operaci dcol a dcols.
 */
void process_dcol_dcols(char *buffer,
                        int position_of_column,
                        int position_of_next_column)
{
	/*
	 * Na vstupu bylo zadano cislo vyssi nez je pocet sloupcu tabulky.
	 * Zmena teto hodnoty na konec radku (posledni sloupec).
	 */
	if(position_of_next_column + 1 == INT_MAX)
	{
		position_of_next_column = get_end_of_line(buffer) - 2;
	}

	if((position_of_column + 1 != INT_MAX))
	{
		int i;
		int j;

		if(position_of_column >= 0)
		{
			i = position_of_column;
			j = position_of_next_column;
		}
		else
		{
			i = 0;
			j = position_of_next_column + 1;
		}

		/*
		 * Prekopirovani znaku. Slouzi pro smazani sloupce.
		 * V pripadu dcols smaze zminenym prekopirovanim znaku vice sloupcu.
		 */
		for(; buffer[j] != '\0' ; i++, j++)
		{
			buffer[i] = buffer[j];
		}

		buffer[i] = buffer[j];
	}

	return;
}

/*
 * Slouzi pro nastaveni hodnot operace dcol pro funkci process_dcol_dcols.
 */
void process_dcol(char *buffer, int argument_column, argument_t *argument)
{
	int column_position = get_position(buffer, argument_column, argument) - 1;
	int next_column_position = get_position(buffer,
	                                        argument_column + 1,
                                          argument) - 1;

	process_dcol_dcols(buffer, column_position, next_column_position);

	return;
}

/*
 * Slouzi pro otestovani argumentu operace dcol. Pri prubehu bez chyby zavola
 * funkci process_dcol pro dalsi zpracovani operace.
 */
int dcol(char *buffer, argument_t *argument, process_t *process)
{
	int argument_column = get_argument(argument, process);

	if(argument_column < 0)
	{
			return argument_column;
	}

	process_dcol(buffer, argument_column, argument);
	set_actual_index(ONEFUNCTIONARGUMENT, process);

	return NOERROR;
}

/*
 * Slouzi pro otestovani argumentu operace dcols. Pri prubehu bez chyby zavola
 * funkci process_dcol_dcols pro dalsi zpracovani operace.
 */
int dcols(char *buffer, argument_t *argument, process_t *process)
{
	if(process->actual_index + 2 >= argument->argc)
	{
		return -MISSINGARGUMENT;
	}

	int first_argument_column = atoi(argument->argv[process->actual_index + 1]);
	int second_argument_column = atoi(argument->argv[process->actual_index + 2]);
	int column_position;
	int next_column_position;

	//V pripadu splneni podminky je jeden z argumentu ve formatu bezneho textu.
	if(first_argument_column == 0 || second_argument_column == 0)
	{
		return -NONNUMERICARGUMENT;
	}

	if(first_argument_column > second_argument_column)
	{
		return -WRONGARGUMENTSORDER;
	}

	column_position = get_position(buffer, first_argument_column, argument) - 1;
	next_column_position = get_position(buffer,
                                      second_argument_column + 1,
                                      argument) - 1;

	process_dcol_dcols(buffer, column_position, next_column_position);
	set_actual_index(TWOFUNCTIONARGUMENTS, process);

	return NOERROR;
}

/*
 * Otestovani spravne velikosti argumentu.
 */
bool valid_argument_size(int first_argument, int second_argument,
                         char *buffer, argument_t *argument)
{
	int number_of_columns = count_of_delims(buffer, argument) + 1;

	return ((first_argument <= number_of_columns) &&
	        (second_argument <= number_of_columns));
}

/*
 * Vraci delku sloupce v poli argument_string.
 */
int get_length_of_string(char *argument_string, int index, argument_t *argument)
{
	int i = index;
	while(argument_string[i]!='\0' &&
	      strchr(argument->delim, argument_string[i]) == NULL)
	{
		i++;
	}

	return (i-index);
}

/*
 * Nastaveni parametru do struktury process.
 */
void set_parameters_insert_column(char *buffer, column_t *column,
                                  argument_t *argument, process_t *process)
{
	//zjistim delku v bunce v danem sloupci
	process->index_first_char = get_position(buffer,
                                           column->argument_column,
                                           argument);
	process->length_original = get_length_of_string(buffer,
                                                  process->index_first_char,
                                                  argument);
	//delka stringu
	process->length_argument = get_length_of_string(column->argument_string,
                                                  FIRSTCHARACTERINDEX,
                                                  argument);

	return;
}

/*
 * Slouzi pro vlozeni retezce do pole. Vklada do daneho sloupce delsi retezec,
 * nez ktery puvodne v danem sloupci byl zapsan.
 */
void insert_longer_string(char *buffer,
                          char *argument_string,
                          process_t *process)
{
	int end_of_line = get_end_of_line(buffer);
	int index = process->index_first_char;
	int delim_position = index + process->length_argument;
	int length_difference = process->length_argument - process->length_original;
	int i = end_of_line + length_difference;

	if(i-delim_position == 1)
	{
		length_difference++;
	}

	//Posunuti vsech znaku za sloupcem, do ktereho se bude vkladat delsi retezec.
	for(; i >= delim_position; i--)
	{
		buffer[i] = buffer[i - length_difference];
	}

	//Vlozeni delsiho retezce do sloupce.
	for(i=index; argument_string[i-index]!='\0'; i++)
	{
		buffer[i] = argument_string[i-index];
	}

	return;
}

/*
 * Slouzi pro vlozeni retezce do pole. Vklada do daneho sloupce kratsi retezec,
 * nez ktery puvodne v danem sloupci byl zapsan.
 */
void insert_shorter_string(char *buffer,
                           char *argument_string,
                           process_t *process)
{
	int index = process->index_first_char;
	int length_difference = process->length_original - process->length_argument;

	//Vlozeni kratsiho retezce do sloupce.
	int i=index;
	for(; argument_string[i-index]!='\0'; i++)
	{
		buffer[i] = argument_string[i-index];
	}

	/*
	 * Osetreni v pripadu vkladani kratsiho retezce do posledniho sloupce.
	 * Snizeni length_difference o 1 zpusobi, ze pri 1. iteraci
	 * nasledujiciho cylku se prekopiruje znak \n.
	 * Pri 2. iteraci jiz nebude splnena podminka cyklu.
	 */
	if(buffer[i+length_difference]=='\0')
	{
		length_difference--;
	}

	//Posunuti znaku v poli za vlozenym (kratsim) retezcem.
	for(; buffer[i+length_difference]!='\0'; i++)
	{
		buffer[i] = buffer[i+length_difference];
	}

	buffer[i] = buffer[i+length_difference];

	return;
}

/*
 * Funkce slouzi pro testovani. Bud bude vlozen retezec delsi nebo roven delce
 * puvodniho retezce, anebo retezec kratsi.
 */
void insert_string(char *buffer, column_t *column,
                   argument_t *argument, process_t *process)
{
	set_parameters_insert_column(buffer, column, argument, process);

	if(process->length_argument >= process->length_original)
	{
		//novy retezec je delsi jak puvodni
		insert_longer_string(buffer, column->argument_string, process);
	}
	else
	{
		//novy retezec je stejne dlouhy nebo kratsi jak puvodni retezec
		insert_shorter_string(buffer, column->argument_string, process);
	}

	return;
}

/*
 * Nastavuje hodnoty do struktury column.
 */
void set_column(column_t *column, char *argument_string, int argument_column)
{
	column->argument_string = argument_string;
	column->argument_column = argument_column;

	return;
}

/*
 * Funkce slouzi pro testovani operace cset. V pripadu spravneho prubehu
 * zavola funkci insert_string pro nasledne provedeni vlozeni retezce.
 */
int cset(char *buffer, argument_t *argument, process_t *process)
{
	if(process->actual_index + 2 >= argument->argc)
	{
		return -MISSINGARGUMENT;
	}

	int argument_column = atoi(argument->argv[process->actual_index + 1]);
	char *argument_string = argument->argv[process->actual_index + 2];
	column_t column;

	set_column(&column, argument_string, argument_column);

	if(argument_column == 0)
	{
		return -NONNUMERICARGUMENT;
	}

	set_actual_index(TWOFUNCTIONARGUMENTS, process);

	if(!valid_argument_size(argument_column, NULLARGUMENT, buffer, argument))
	{
		return NOERROR;
	}

	insert_string(buffer, &column, argument, process);

	return NOERROR;
}

/*
 * Funkce slouzi k procesu operace copy. Vola potrebne funkce pro nastaveni
 * retezce a pote vola funkci pro vlozeni daneho retezce.
 */
void process_copy(char *buffer, arg_column_t *arg_column,
                  argument_t *argument, process_t *process)
{
	char argument_string[MAXLENGTHOFCELL + 1];
	int position_of_column = get_position(buffer,
                                        arg_column->first_argument_column,
                                        argument);
	column_t column;

	set_column(&column, argument_string, arg_column->second_argument_column);
	set_column_value(buffer, argument_string, position_of_column, argument);
	insert_string(buffer, &column, argument, process);

	return;
}

/*
 * Funkce je spolecna pro provedeni operace copy, move, swap. Testuje, o kterou
 * operaci se jedna a pote zavola potrebne funkce pro jeji provedeni.
 */
void process_copy_move_swap(char *buffer, argument_t *argument,
                            process_t *process, arg_column_t *arg_column,
                            const char *function_name)
{
	if(strcmp(function_name, "copy") == 0)
	{
  	process_copy(buffer, arg_column, argument, process);
	}
	else if(strcmp(function_name, "move") == 0)
	{
		if(arg_column->first_argument_column > arg_column->second_argument_column)
		{
			arg_column->first_argument_column++;
		}

		process_icol(buffer, arg_column->second_argument_column, argument);
		process_copy(buffer, arg_column, argument, process);
		process_dcol(buffer, arg_column->first_argument_column, argument);
	}
	else if(strcmp(function_name, "swap") == 0)
	{
		int position_of_column = get_position(buffer,
                                          arg_column->second_argument_column,
                                          argument);
		char argument_string[MAXLENGTHOFCELL + 1];
		column_t column;

		set_column(&column, argument_string, arg_column->first_argument_column);

		//ulozim si hodnotu ze sloupce, ktery prepisu pri copy
		set_column_value(buffer, argument_string, position_of_column, argument);
		process_copy(buffer, arg_column, argument, process);
		insert_string(buffer, &column, argument, process);
	}
}

/*
 * Funkce slouzi k nastaveni hodnot do struktury arg_column.
 */
void set_arg_column(arg_column_t *arg_column,
                    int first_argument_column,
                    int second_argument_column)
{
	arg_column->first_argument_column = first_argument_column;
	arg_column->second_argument_column = second_argument_column;

	return;
}

/*
 * Funkce slouzi pro otestovani argumentu. V pripadu bezchybneho prubehu vola
 * funkci process_copy_move_swap pro provedeni dane operace.
 */
int copy_move_swap(char *buffer, argument_t *argument,
                   process_t *process, const char *function_name)
{
	if(process->actual_index + 2 >= argument->argc)
	{
		return -MISSINGARGUMENT;
	}

	int first_argument_column = atoi(argument->argv[process->actual_index + 1]);
	int second_argument_column = atoi(argument->argv[process->actual_index + 2]);
	arg_column_t arg_column;

	set_arg_column(&arg_column, first_argument_column, second_argument_column);

	//V pripadu splneni podminky je jeden z argumentu ve formatu bezneho textu.
	if(first_argument_column == 0 || second_argument_column == 0)
	{
		return -NONNUMERICARGUMENT;
	}

	set_actual_index(TWOFUNCTIONARGUMENTS, process);

	if(!valid_argument_size(first_argument_column, second_argument_column,
                          buffer, argument))
	{
		return NOERROR;
	}

	process_copy_move_swap(buffer, argument, process, &arg_column, function_name);

	return NOERROR;
}

/*
 * Funkce vraci hodnotu po provedeni funkce copy_move_swap.
 * NOERROR pri bezchybnem prubehu. V opacnem pripadu zapornou hodnotu
 * chyboveho stavu.
 */
int copy(char *buffer, argument_t *argument, process_t *process)
{
	return copy_move_swap(buffer, argument, process, __func__);
}

/*
 * Funkce vraci hodnotu po provedeni funkce copy_move_swap.
 * NOERROR pri bezchybnem prubehu. V opacnem pripadu zapornou hodnotu
 * chyboveho stavu.
 */
int move(char *buffer, argument_t *argument, process_t *process)
{
	return copy_move_swap(buffer, argument, process, __func__);
}

/*
 * Funkce vraci hodnotu po provedeni funkce copy_move_swap.
 * NOERROR pri bezchybnem prubehu. V opacnem pripadu zapornou hodnotu
 * chyboveho stavu.
 */
int swap(char *buffer, argument_t *argument, process_t *process)
{
	return copy_move_swap(buffer, argument, process, __func__);
}

/*
 * Funkce slouzi k rozhodnuti o prevedeni operace tolower nebo operace toupper.
 */
void convert_lower_upper(char *buffer, const char *function_name,
                         int position_of_column, argument_t *argument)
{
	int i=position_of_column;
	while(buffer[i]!='\0' && strchr(argument->delim, buffer[i]) == NULL)
	{
		if(strcmp(function_name, "process_to_lower") == 0)
		{
			buffer[i] = tolower(buffer[i]);
		}
		else
		{
			buffer[i] = toupper(buffer[i]);
		}

		i++;
	}

	return;
}

/*
 * Funkce slouzi ke spolecnemu procesu operaci tolower a toupper.
 * Pri bezchybnem prubehu vola funkce convert_lower_upper.
 */
int process_lower_upper(char *buffer, const char *function_name,
                        argument_t *argument, process_t *process)
{
	int argument_column = get_argument(argument, process);

	if(argument_column < 0)
	{
			return argument_column;
	}

	int position_of_column = get_position(buffer, argument_column, argument);

	if(position_of_column != INT_MAX)
	{
		convert_lower_upper(buffer, function_name, position_of_column, argument);
	}

	set_actual_index(ONEFUNCTIONARGUMENT, process);

	return NOERROR;
}

/*
 * Funkce vraci hodnotu NOERROR pri bezchybnem prubehu. V opacnem pripadu
 * vraci zapornou hodnotu chyboveho stavu.
 */
int process_to_lower(char *buffer, argument_t *argument, process_t *process)
{
	return process_lower_upper(buffer, __func__, argument, process);
}

/*
 * Funkce vraci hodnotu NOERROR pri bezchybnem prubehu. V opacnem pripadu
 * vraci zapornou hodnotu chyboveho stavu.
 */
int process_to_upper(char *buffer, argument_t *argument, process_t *process)
{
	return process_lower_upper(buffer, __func__, argument, process);
}

/*
 * Funkce pro testovani, zda se jedna o ciselnou hodnotu. Vraci true pri
 * splneni podminek. V opacnem pripadu vraci false.
 */
bool is_numeric_value(char *buffer,
                      int position_of_column,
                      argument_t *argument)
{
	int i = position_of_column;
	for(; buffer[i]!='\n' && strchr(argument->delim, buffer[i]) == NULL; i++)
	{
		/*
		 * V pripadu, ze se jedna o ciselnou hodnotu, znak "-"
		 * reprezentuje zapornou hodnotu.
		 */
		if((i == position_of_column) && (buffer[i] == '-'))
		{
			continue;
		}

		/*
		 * V pripadu, ze se jedna o ciselnou hodnotu, znak "."
		 * reprezentuje desetinnou tecku.
		 */
		if(buffer[i]=='.')
		{
			continue;
		}

		//Pri splneni podminky znak na pozici buffer[i] neni ciselna hodnota.
		if(isdigit(buffer[i]) == 0)
		{
			return false;
		}
	}

	return true;
}

/*
 * Funkce slouzi pro rozhodnuti o provedeni operace zaokrouhleni round.
 */
bool check_round(char *buffer,
                 const char *function_name,
                 int i, argument_t *argument)
{
	if(strcmp(function_name, "process_round") != 0)
	{
		return false;
	}

	if((buffer[i]=='\n') ||
	   (strchr(argument->delim, buffer[i]) != NULL))
	{
		return false;
	}

	//Pokud je cislo, ktere rozhoduje o zaokrouhleni, vetsi nebo rovno cislu 5.
	if((buffer[i+1] - '0') >= 5)
	{
		return true;
	}

	return false;
}

/*
 * Funkce slouzi pro nastaveni hodnoty sloupce pro spolecny proces
 * operaci round a int.
 */
void set_column_value_round_int(char *buffer, char *argument_string,
                                int position_of_column,
                                const char *function_name,
                                argument_t *argument)
{
	int i = position_of_column, j=0;
	while(buffer[i]!='.' && buffer[i]!='\n' &&
		  strchr(argument->delim, buffer[i]) == NULL)
	{
		argument_string[j] = buffer[i];
		i++;
		j++;
	}

	argument_string[j] = '\0';

	//Rozhodnuti, zda se ma posledni cislo zvysit o 1 (operace round).
	if(check_round(buffer, function_name, i, argument))
	{
		argument_string[j-1] = (buffer[i-1] - '0') + 1 + '0';
	}

	return;
}

/*
 * Funkce slouzi pro spolecny proces operaci round a int. Testuje spravnost
 * argumentu. Pri bezchybnem prubehu vola funkci set_column_value_round_int.
 */
int process_round_int(char *buffer, const char *function_name,
                      argument_t *argument, process_t *process)
{
	int argument_column = get_argument(argument, process);
	char argument_string[MAXLENGTHOFCELL + 1];
	column_t column;

	set_column(&column, argument_string, argument_column);

	if(argument_column < 0)
	{
			return argument_column;
	}

	set_actual_index(ONEFUNCTIONARGUMENT, process);

	if(!valid_argument_size(argument_column, NULLARGUMENT, buffer, argument))
	{
		return NOERROR;
	}

	int position_of_column = get_position(buffer, argument_column, argument);

	if(!is_numeric_value(buffer, position_of_column, argument))
	{
		return -NONNUMERICVALUE;
	}

	set_column_value_round_int(buffer, argument_string, position_of_column,
                             function_name, argument);
	insert_string(buffer, &column, argument, process);

	return NOERROR;
}

/*
 * Funkce vraci hodnotu NOERROR pri bezchybnem prubehu. V opacnem pripadu
 * vraci zapornou hodnotu chyboveho stavu.
 */
int process_round(char *buffer, argument_t *argument, process_t *process)
{
	int state = process_round_int(buffer, __func__, argument, process);
	return state;
}

/*
 * Funkce vraci hodnotu NOERROR pri bezchybnem prubehu. V opacnem pripadu
 * vraci zapornou hodnotu chyboveho stavu.
 */
int process_int(char *buffer, argument_t *argument, process_t *process)
{
	int state = process_round_int(buffer, __func__, argument, process);
	return state;
}

/*
 * Funkce slouzi pro nastaveni pole struktur obsahujici ukazatele na funkce
 * a nazvy funkci. Dale nastavuje velikost daneho pole.
 * Vraci navratovou hodnotu funkce process_arguments.
 */
int process_functions(char *buffer, argument_t *argument, process_t *process)
{
	/*
	 * Explicitni zapis je pouzit pri rozdilnem nazvu funkce a operace.
	 * Pouziti z duvodu stejne pojmenovanych funkci definovanych v knihovnach.
	 */
	function_t functions_map [] = {
	  FUNCTION(acol)
	  FUNCTION(copy)
	  FUNCTION(cset)
	  FUNCTION(dcol)
	  FUNCTION(dcols)
	  FUNCTION(drow)
	  FUNCTION(drows)
	  FUNCTION(icol)
	  {process_int, "int"},
	  FUNCTION(irow)
	  FUNCTION(move)
	  {process_round, "round"},
	  FUNCTION(swap)
	  {process_to_lower, "tolower"},
	  {process_to_upper, "toupper"},
	};

	int map_size = (sizeof(functions_map) / sizeof(functions_map[0]));

	return process_arguments(buffer, argument, process, functions_map, map_size);
}

/*
 * Funkce slouzici pro provedeni operaci pouze na poslednim radku
 * vstupni tabulky.
 */
int process_last_line(char *second_buffer,
                      argument_t *argument,
                      process_t *process)
{
	int state = row_selection(second_buffer, argument, process);

	if(state == CONDITIONSMET)
	{
		state = process_functions(second_buffer, argument, process);
	}

	if(state < NOERROR)
	{
		return state;
	}

	print_line(second_buffer, argument);
  set_default_index(argument, process);

  return NOERROR;
}

/*
 * Funkce slouzi pro zkoupirovani pole buffer do pole second_buffer.
 */
void copy_array(char *buffer, char *second_buffer)
{
	int i = 0;
	for(; buffer[i]!='\0'; i++)
	{
		second_buffer[i] = buffer[i];
	}

	second_buffer[i] = buffer[i];

	return;
}

/*
 * Funkce pri bezchybnem prubehu vypisuje vsechny radky krome posleniho radku.
 */
int process_only_last_line(char *buffer,
                           argument_t *argument,
                           process_t *process)
{
	char second_buffer[MAXLENGTHOFLINE];

	while(scan_line(buffer) != NULL)
	{
		if(!valid_row(buffer, argument))
		{
			return -MAXROWLENGTHEXCEED;
		}

		if(process->number_of_row > 1)
		{
			print_line(second_buffer, argument);
		}

		copy_array(buffer, second_buffer);
	  process->number_of_row++;
	}

	if(process->number_of_row == 1)
	{
		return -EMPTYFILE;
	}

	set_actual_index(TWOFUNCTIONARGUMENTS, process);

	return process_last_line(second_buffer, argument, process);
}

/*
 * Funkce slouzi pro volani funkce pro nacitani radku a provedeni
 * jednotlivych operaci.
 */
int process_all_lines(char *buffer,
                      argument_t *argument,
                      process_t *process)
{
	int state;

	while(scan_line(buffer) != NULL)
	{
		if(!valid_row(buffer, argument))
		{
			return -MAXROWLENGTHEXCEED;
		}

		state = row_selection(buffer, argument, process);

        if(state == CONDITIONSMET)
		{
	        state = process_functions(buffer, argument, process);
		}

		if(state < NOERROR) return state;

		print_line(buffer, argument);
       	set_default_index(argument, process);
       	process->number_of_row++;
	}

	if(process->number_of_row==1) return -EMPTYFILE;

	return NOERROR;
}

/*
 * Funkce slouzi pro testovani operace rows, ve tvaru rows - -.
 * V tomto tvaru program provede operace pouze na poslednim radku.
 */
bool check_only_last_line(argument_t *argument, process_t *process)
{
	int index_of_argv = process->actual_index;

	if((index_of_argv + 2 < argument->argc) &&
	   (strcmp(argument->argv[index_of_argv], "rows") == 0))
	{
		char *first_argument = argument->argv[index_of_argv+ 1];
		char *second_argument = argument->argv[index_of_argv + 2];

		return (strcmp(first_argument, "-")==0 &&
				strcmp(second_argument, "-")==0);
	}

	return false;
}

/*
 * Funkce testuje, zda se maji provest operace pouze na poslednim radku.
 * V opacnem pripadu zavola funkci process_all_lines. V tomto pripadu
 * testovani dalsich operaci selekce radku probiha pri jednotlivych
 * radcich tabulky.
 */
int process_rows(argument_t *argument, process_t *process)
{
	char buffer[MAXLENGTHOFLINE];
	int state;

	if(check_only_last_line(argument, process))
	{
		state =	process_only_last_line(buffer, argument, process);
	}
	else
	{
		state = process_all_lines(buffer, argument, process);
	}

	if(state != NOERROR) return state;

	check_arow(buffer, argument, process);

	return NOERROR;
}

/*
 * Funkce slouzi pro vypis chybovych hlaseni na stderr.
 */
void print_error(int error)
{
	/*
	 * Chybove hlaseni pro jednotlive chybove stavy.
	 * Jejich poradi odpovida poradi konstant v enum errors.
	 */
	const char *emsg[] =
	{
		"Pri behu programu nenastala zadna chyba.",
		"Chybny format argumentu - ocekavana ciselna hodnota.",
		"Chybny format argumentu - ocekavana kladna ciselna hodnota.",
		"Chybny format argumentu - ocekavana kladna ciselna hodnota.",
		"Chybny format argumentu - ocekavana kladna ciselna hodnota.",
		"Prekrocena maximalni delka retezce v bunce.",
		"Nespravny nazev argumentu.",
		"Prekrocena maximalni delka radku.",
		"Prekrocena maximalni delka argumentu.",
		"Chybejici argument funkce.",
		"Chybna vstupni tabulka - prazdny soubor.",
		"Chybna hodnota bunky - ocekavana ciselna hodnota.",
		"Nespravne poradi argumentu.",
		"Neznama chyba."
	};

	if(error > UNKNOWNERROR) error = UNKNOWNERROR;
	fprintf(stderr, "Chyba: %s\n", emsg[error]);
	return;
}

/*
 * Hlavni funkce main.
 */
int main(int argc, char **argv)
{
	argument_t argument;
	process_t process;
	int error;

	set_argument(argc, argv, &argument);
	set_delim(&argument);
	set_default_index(&argument, &process);
	set_process(&process);

	if((error = valid_argument(&argument)) != NOERROR ||
	   (error = process_rows(&argument, &process)) != NOERROR)
	{
		print_error(abs(error));
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
