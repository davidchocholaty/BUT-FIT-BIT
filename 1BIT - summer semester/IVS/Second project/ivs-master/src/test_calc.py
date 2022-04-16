## @file      test_calc.py
#  @brief     Testy pro matematickou knihovnu
#  @author    Stepan Bakaj <xbakaj00@stud.fit.vutbr.cz>
#  @author    David Chocholaty <xchoch09@stud.fit.vutbr.cz>
#  @version   1.0
#  @date      2021-03-20
#  @copyright GNU Public License

import unittest
import calc
import math


class CalcTest(unittest.TestCase):
    ## Testy pro scitani    
    def test_add(self):
        self.assertEqual(calc.add(2, 2), 4)
        self.assertEqual(calc.add(2, -2), 0)
        self.assertEqual(calc.add(-2, -6), -8)
        self.assertEqual(calc.add(0, 0), 0)
        self.assertEqual(calc.add(21000, 25000), 46000)
        self.assertEqual(calc.add(2.5145, 5.7895), 8.304)        

    ## Testy pro rozdil
    def test_subtract(self):
        self.assertEqual(calc.subtract(2, 2), 0)
        self.assertEqual(calc.subtract(2, 6), -4)
        self.assertEqual(calc.subtract(2, -2), 4)
        self.assertEqual(calc.subtract(-2, 2), -4)
        self.assertEqual(calc.subtract(2, 0), 2)
        self.assertEqual(calc.subtract(15000, 7000), 8000)
        self.assertEqual(calc.subtract(7.5423, 2.9876), 4.5547)
        self.assertEqual(calc.subtract(2.1234, 10.9154), -8.792)

    ## Testy pro deleni
    def test_divide(self):
        self.assertRaises(Exception, calc.divide, 5, 0)
        self.assertEqual(calc.divide(-2, 2), -1)
        self.assertEqual(calc.divide(6, 2), 3)
        self.assertEqual(calc.divide(2, -2), -1)
        self.assertEqual(calc.divide(5, 2), 5/2)
        self.assertEqual(calc.divide(-1, -1), 1)
        self.assertEqual(calc.divide(15000, 5000), 3)
        self.assertEqual(calc.divide(5.7915, 2.75), 2.106)
        self.assertEqual(calc.divide(10.462, -5.231), -2)

    ## Testy pro nasobeni
    def test_multiply(self):
        self.assertEqual(calc.multiply(0, 1), 0)
        self.assertEqual(calc.multiply(5, 1), 5)
        self.assertEqual(calc.multiply(-5, 1), -5)
        self.assertEqual(calc.multiply(1, -6), -6)
        self.assertEqual(calc.multiply(3, 3), 9)
        self.assertEqual(calc.multiply(-6, -5), 30)
        self.assertEqual(calc.multiply(6, 0), 0)
        self.assertEqual(calc.multiply(2.1945, 5.4), 11.8503)
        self.assertEqual(calc.multiply(-2, 7.14), -14.28)

    ## Testy pro faktorial
    def test_fact(self):
        self.assertRaises(Exception, calc.fact, -3)
        self.assertRaises(Exception, calc.fact, -10000)
        self.assertRaises(Exception, calc.fact, 5.1234)
        self.assertEqual(calc.fact(5), 120)
        self.assertEqual(calc.fact(0), 1)
        self.assertEqual(calc.fact(1), 1)
        self.assertEqual(calc.fact(2), 2)
        self.assertEqual(calc.fact(3), 6)        
        self.assertEqual(calc.fact(0.0), 1)        

    ## Testy pro druhou odmocninu
    def test_square_root(self):
        self.assertRaises(Exception, calc.square_root, -1)
        self.assertRaises(Exception, calc.square_root, -35)
        self.assertEqual(calc.square_root(0), 0)
        self.assertEqual(calc.square_root(1), 1)
        self.assertEqual(calc.square_root(4), 2)
        self.assertEqual(calc.square_root(3), math.sqrt(3))
        self.assertEqual(calc.square_root(400), 20)
        self.assertEqual(calc.square_root(800), math.sqrt(800))
        self.assertEqual(calc.square_root(0.25), 0.5)

    ## Testy pro n-tou mocninu 
    def test_n_power(self):
        self.assertEqual(calc.n_power(2, 2), 4)
        self.assertEqual(calc.n_power(2, 0), 1)
        self.assertEqual(calc.n_power(0, 2), 0)
        self.assertEqual(calc.n_power(2, -2), 1/4)
        self.assertEqual(calc.n_power(-2, 2), 4)
        self.assertEqual(calc.n_power(5, 3), 125)
        self.assertEqual(calc.n_power(5, -1), 1/5)
        self.assertEqual(calc.n_power(7, 10), 282475249)
        self.assertEqual(calc.n_power(4, 0.5), 2)
        self.assertEqual(calc.n_power(0.5, 2), 0.25)
        self.assertEqual(calc.n_power(4, -0.5), 0.5)

    ## Testy pro druhou mocninu
    def test_power(self):
        self.assertEqual(calc.power(2), 4)
        self.assertEqual(calc.power(3), 9)
        self.assertEqual(calc.power(0), 0)
        self.assertEqual(calc.power(20), 400)
        self.assertEqual(calc.power(-2), 4)
        self.assertEqual(calc.power(5), 25)
        self.assertEqual(calc.power(51), 2601)
        self.assertEqual(calc.power(1), 1)
        self.assertEqual(calc.power(0.5), 0.25)
        self.assertEqual(calc.power(-0.62), 0.3844)

    ## Testy pro n-tou odmocninu
    def test_n_root(self):
        self.assertEqual(calc.n_root(-1, 5), -1)
        self.assertRaises(Exception, calc.n_root, -5, -2)
        self.assertEqual(calc.n_root(4,-2), 1/2)
        self.assertEqual(calc.n_root(1, 1), 1)
        self.assertEqual(calc.n_root(2, 3), math.pow(2, 1/3))
        self.assertEqual(calc.n_root(5, 1), 5)
        self.assertEqual(calc.n_root(2, 2), math.pow(2, 1/2))
        self.assertEqual(calc.n_root(0, 3), 0)
        self.assertEqual(calc.n_root(8, 3), 2)
        self.assertEqual(calc.n_root(0.25, 2), 0.5)
        
    ## Testy pro logaritmus
    def test_log(self):
        self.assertRaises(Exception, calc.log, 0)
        self.assertRaises(Exception, calc.log, -5)
        self.assertRaises(Exception, calc.log, -1)
        self.assertEqual(calc.log(1), 0)
        self.assertEqual(calc.log(10), 1)
        self.assertEqual(calc.log(5), round(math.log10(5), 5))
        self.assertEqual(calc.log(100), 2)
        self.assertEqual(calc.log(20), round(math.log10(20), 5))
        self.assertEqual(calc.log(0.1), -1)


if __name__ == '__main__':
    unittest.main()
