## @file      calc.py
#  @brief     Matematicka knihovna se zakladnimi matematickymi operacemi
#  @author    Stepan Bakaj <xbakaj00@stud.fit.vutbr.cz>
#  @author    Martin Balaz <xbalaz15@stud.fit.vutbr.cz>
#  @author    David Chocholaty <xchoch09@stud.fit.vutbr.cz>
#  @version   1.0
#  @date      2021-03-21
#  @copyright GNU Public License

## Funkce pro secteni dvou realnych cisel.
#
#  @param x Prvni parametr souctu obsahujici realnou hodnotu
#  @param y Druhy parametr souctu obsahujici realnou hodnotu
#  @return Vraci soucet hodnot promenne x a promenne y
def add(x, y):
    return x + y

## Funkce pro vypocet rozdilu dvou realnych cisel.
#
#  @param x Prvni parametr rozdilu obsahujici realnou hodnotu
#  @param y Druhy parametr rozdilu obsahujici realnou hodnotu
#  @return Vraci rozdil hodnot promenne x a promenne y ve tvaru x - y
def subtract(x, y):
    return x - y

## Funkce pro vynasobeni dvou realnych cisel.
#
#  @param x Prvni parametr nasobeni obsahujici realnou hodnotu
#  @param y Druhy parametr nasobeni obsahujici realnou hodnotu
#  @return Vraci hodnotu nasobeni promenne x a promenne y
def multiply(x, y):
    return x * y

## Funkce pro vypocet podilu dvou realnych cisel.
#
#  @param x Delenec obsahujici realnou hodnotu
#  @param y Delitel obsahujici realnou hodnotu
#  @exception Exception Vyjimka je vyvolana, pokud delitel obsahuje hodnotu 0
#  @return Vraci hodnotu podilu promenne x a promenne y ve tvaru x / y
def divide(x, y):
    if y == 0.0:
        raise Exception("Deleni nulou neni definovano")
    return x / y

## Funkce pro vypocet faktorialu.
#
#  @param x Parametr pro vypocet faktorialu obsahujici realnou hodnotu
#  @exception Exception Vyjimka je vyvolana, pokud parametr obsahuje zapornou hodnotu
#  @return Vraci hodnotu faktorialu pro dany parametr
def fact(x):
    if x < 0.0:
        raise Exception("Faktorial je definovan pouze pro nezaporna realna cisla")
    elif x % 1 != 0:
        raise Exception("Nepodporovana funkcionalita")
    elif x == 0.0:
        return 1    
    else:
        fact = 1 
        for i in range(1, x + 1): 
            fact = fact * i
        return fact

## Funkce pro vypocet druhe odmocniny
#
#  @param x Parametr pro vypocet druhe odmocniny obsahujici realnou hodnotu
#  @exception Exception Vyjimka je vyvolana, pokud parametr obsahuje zapornou hodnotu
#  @return Vraci hodnotu druhe odmocniny pro dany parametr
def square_root(x):
    if x < 0.0:
        raise Exception("Druha odmocnina je definovana pouze pro nezaporna realna cisla")
    elif x == 0.0 or x == 1.0:
        return x
    else:
        return x ** (1 / 2)

## Funkce pro vypocet n-te odmocniny
#
#  @param x Parametr pro vypocet n-te odmocniny obsahujici realnou hodnotu
#  @param n Rad odmocniny obsahujici realnou hodnotu
#  @exception Exception Vyjimka je vyvolana, pokud se jedna o odmocninu nulteho radu, nebo pokud parametr x je zaporny a rad odmocniny je sudy
#  @return Vraci hodnotu n-te odmocniny pro dane parametry

def n_root(x, n):
    if n == 0.0:
        raise Exception("Odmocnina nulteho radu neni definovana")
    if n % 2 != 0: 
        return n_power(x, 1./n) if x >= 0 else -n_power(abs(x), 1./n)
    else:
        if x >= 0.0:
            return n_power(x, 1./n)
        else:
            raise Exception("Odmocnina sudeho radu je definovana pouze pro nezaporna realna cisla")

## Funkce pro vypocet druhe mocniny
#
#  @param x Zaklad pro vypocet druhe mocniny obsahujici realnou hodnotu
#  @return Vraci hodnotu druhe mocniny pro dany parametr
def power(x):
    return x ** 2

## Funkce pro vypocet n-te mocniny
#
#  @param x Zaklad pro vypocet n-te mocniny obsahujici realnou hodnotu
#  @param n Exponent pro vypocet n-te mocniny obsahujici realnou hodnotu
#  @return Vraci hodnotu n-te mocniny pro dane parametry
def n_power(x, n):
    return x ** n

## Funkce pro vypocet prirozeneho logaritmu
#
#  Funkce je pouzita pro vypocet dekadickeho logaritmu
#  @param x Parametr pro vypocet prirozeneho logaritmu obsahujici realnou hodnotu
#  @exception Exception Vyjimka je vyvolana, pokud parametr obsahuje zapornou nebo nulovou hodnotu
#  @return Vraci hodnotu prirozeneho logaritmu pro dany parametr
def ln(x):
    if x <= 0.0:
        raise Exception("Prirozeny logaritmus je definovany pouze pro kladna cisla")
    else:
        n = 99999999
        return n * (x ** float(1 / n) - 1.0)

## Funkce pro vypocet dekadickeho logaritmu
#
#  @param x Parametr pro vypocet dekadickeho logaritmu obsahujici realnou hodnotu
#  @exception Exception Vyjimka je vyvolana, pokud parametr obsahuje zapornou nebo nulovou hodnotu
#  @return Vraci hodnotu dekadickeho logaritmu pro dany parametr
def log(x):
    if x <= 0.0:
        raise Exception("Logaritmus je definovany pouze pro kladna realna cisla v oboru realnych cisel")
    else:
        accuracy = 5
        return round(ln(x) / ln(10), accuracy)

# Konec souboru calc.py #
