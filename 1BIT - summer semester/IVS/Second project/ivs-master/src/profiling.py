import sys
from calc import add, subtract, multiply, divide, square_root, power

## @file      profiling.py
#  @brief     Skript pro vypocet vyberove smerodatne odchylky z posloupnosti cisel
#  @author    Martin Balaz <xbalaz15@stud.fit.vutbr.cz>
#  @author    David Chocholaty <xchoch09@stud.fit.vutbr.cz>
#  @version   1.0
#  @date      2021-03-25
#  @copyright GNU Public License

## Funkce pro nacteni vstupu ze stdin
#
#  @return Vraci seznam obsahujici jednotlive ciselne hodnoty
def read_input():    
    input = sys.stdin.read()        
    numbers = input.split()
    numbers = list(map(float, numbers)) # Prevod hodnot seznamu ze string na float

    return numbers

## Funkce pro vypocet sumy druhych mocnin vstupnich cisel
#
#  @param numbers Seznam vstupnich ciselnych hodnot
#  @return Vraci vyslednou hodnotu sumy
def sum_squares(numbers):
    sum = 0
    for i in numbers:        
        power_res = power(i)
        sum = add(sum, power_res)
    
    return sum

## Funkce pro vypocet sumy vstupnich cisel
#
#  @param numbers Seznam vstupnich ciselnych hodnot
#  @return Vraci vyslednou hodnotu sumy
def sum_numbers(numbers):
    sum = 0
    for i in numbers:        
        sum = add(sum, i)
    
    return sum

## Funkce pro vypocet aritmetickeho prumeru vstupnich cisel
#
#  @param numbers Seznam vstupnich ciselnych hodnot
#  @param items_count Pocet vstupnich cisel
#  @return Vraci vyslednou hodnotu aritmetickeho prumeru
def mean(numbers, items_count):    
    sum = sum_numbers(numbers)     
    return divide(sum, float(items_count))

## Funkce pro vypocet smerodatne odchylky
#
#  @param items_count Pocet vstupnich cisel
#  @param mean Aritmeticky prumer vstupnich cisel
#  @param sum_squares Suma druhych mocnin vstupnich cisel
#  @return Vraci hodnotu smerodatne odchylky
def standard_deviation(items_count, mean, sum_squares):            
    s = multiply(items_count, power(mean))               # N * (x_^2)
    s = subtract(sum_squares, s)                         # sum 1 to N (x_i^2) - N * (x_^2) 
    s = divide(s, float(subtract(items_count, 1)))       # 1/(N - 1) * sum 1 to N (x_i^2) - N * (x_^2)
    s = square_root(s)                                   # sqrt(1/(N - 1) * sum 1 to N (x_i^2) - N * (x_^2))
    return s

## Hlavni ridici funkce skriptu profiling.py
def main():
    numbers = read_input()
    items_count = len(numbers)     

    mean_res = mean(numbers, items_count)
    sum_squares_res = sum_squares(numbers)
    s = standard_deviation(items_count, mean_res, sum_squares_res)

    print(s)
    
if __name__ == '__main__':
    main()

# Konec souboru profiling.py #
