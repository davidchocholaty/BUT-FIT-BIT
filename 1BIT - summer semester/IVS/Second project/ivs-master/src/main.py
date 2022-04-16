## @file      main.py
#  @brief     Soubor main s nadstavbou pro gui
#  @author    Adam Kankovsky <xkanko00@stud.fit.vutbr.cz>
#  @author     Stepan Bakaj <xbakaj00@stud.fit.vutbr.cz>
#  @version   1.0
#  @date      2021-05-25
#  @copyright GNU Public License

from kivymd.app import MDApp
from kivy.lang.builder import Builder
from kivy.core.window import Window
from kivy.uix.screenmanager import ScreenManager,Screen
from kivy.properties import ObjectProperty
from kivy.resources import resource_add_path
import os, sys
import calc

## Hlavni trida grafickeho rozhrani
#
# @class MyGrid
# @brief Zajistuje komunikaci mezi promennymi z gui.kv a mistnimi promennymi urcenymi k vypoctu matematickych vypoctu
class MyGrid(Screen):
    calcimput = ObjectProperty(None)
    resultlabel = ObjectProperty(None)

    ## Funkce zaznamenava udalosti stisk klavesnice
    #  
    #  @param self Vola tridu MyGrid
    #  @param kwargs Umoznuje zadat jakekoliv mnozstvi parametru
    def __init__(self, **kwargs):
        super(MyGrid, self).__init__(**kwargs)
        Window.bind(on_key_down=self._on_keyboard_down)

    ## Funkce zaznamenava stisk kláves
    #
    #  @param self Vola tridu MyGrid
    #  @param instance Predava udalosti
    #  @param keyboard Predava data rozpolzeni klaves
    #  @param keycode Predava id stisknute klavesy
    #  @param text Predava text vlozeny pomoci stisknuti klaves
    #  @param modifiers Predava funkcni klavesy
    def _on_keyboard_down(self, instance, keyboard, keycode, text, modifiers):
        if keycode == 40 or keycode == 88:
            self.calculate()
        elif keycode == 6:
            self.calcimput.text = ""
        elif keycode == 42 or keycode == 76:
            self.calcimput.text = self.calcimput.text[: -1]
        elif keycode == 84:
            self.calcimput.text += '/'
        elif keycode == 85:
            self.calcimput.text += '*'
        elif keycode == 86:
            self.calcimput.text += '-'
        elif keycode == 87:
            self.calcimput.text += '+'
        elif keycode == 89 or keycode == 30:
            self.calcimput.text += '1'
        elif keycode == 90 or keycode == 31:
            self.calcimput.text += '2'
        elif keycode == 91 or keycode == 32:
            self.calcimput.text += '3'
        elif keycode == 92 or keycode == 33:
            self.calcimput.text += '4'
        elif keycode == 93 or keycode == 34:
            self.calcimput.text += '5'
        elif keycode == 94 or keycode == 35:
            self.calcimput.text += '6'
        elif keycode == 95 or keycode == 36:
            self.calcimput.text += '7'
        elif keycode == 96 or keycode == 37:
            self.calcimput.text += '8'
        elif keycode == 97 or keycode == 38:
            self.calcimput.text += '9'
        elif keycode == 98 or keycode == 39:
            self.calcimput.text += '0'
        elif keycode == 99 or keycode == 55:
            self.calcimput.text += '.'

    ## Funkce pri stisknuni tlacitka rovna se
    #
    #  @param self Vola tridu MyGrid
    def calculate(self):
        input_text = self.calcimput.text
        y = 0
        splited = ["", "", ""]
        result = ""
        err = 0
        for x in range(0, len(input_text)):
            if input_text[x] == "+" or input_text[x] == "*" or input_text[x] == "/" or input_text[x] == "^" or input_text[x] == "√":
                if y > 1:
                    self.resultlabel.text = "Tento příklad zdá se vypočítat neumíme."
                    err = 1
                    break
                else:
                    y += 1
                    splited[y] = input_text[x]
                    y += 1
            elif input_text[x] == "-":
                if splited[y] == "":
                    splited[y] = input_text[x]
                else:
                    y += 1
                    if y < 2:
                        splited[y] = input_text[x]
                        y += 1
                    else:
                        self.resultlabel.text = "Tento příklad zdá se vypočítat neumíme."
                        err = 1
                        break
            elif input_text[x] == "l" or input_text[x] == "n":
                if y > 1:
                    self.resultlabel.text = "Tento příklad zdá se vypočítat neumíme."
                    err = 1
                    break
                else:
                    if input_text[x] == "l" and splited[y] == "":
                        splited[y] = input_text[x]
                    elif input_text[x] == "n" and splited[y] == "l":
                        splited[y] += input_text[x]
                        y += 1
                    else:
                        self.resultlabel.text = "Zadana neplatna funkce"
                        err = 1
                        break
            elif input_text[x] == "!":
                if y > 1:
                    self.resultlabel.text = "Tento příklad zdá se vypočítat neumíme."
                    err = 1
                    break
                else:
                    y += 1
                    splited[y] = input_text[x]
                    break
            elif input_text[x] != " ":
                splited[y] = splited[y] + input_text[x]
        try:
            if err != 1:
                if splited[1] == '+':
                    if splited[2] == "" and splited[0] == "":
                        splited[2] = "0"
                    result = calc.add(float(splited[0]), float(splited[2]))
                elif splited[1] == '-':
                    if splited[2] == "" and splited[0] == "":
                        splited[2] = "0"
                    result = calc.subtract(float(splited[0]), float(splited[2]))
                elif splited[1] == '*':
                    if splited[2] != "" and splited[0] != "":
                        result = calc.multiply(float(splited[0]), float(splited[2]))
                elif splited[1] == '/':
                    if splited[2] != "" and splited[0] != "":
                        result = calc.divide(float(splited[0]), float(splited[2]))
                elif splited[1] == '^':
                    if splited[2] != "" and splited[0] != "":
                        if splited[2] == '2':
                            result = calc.power(float(splited[0]))
                        else:
                            result = calc.n_power(float(splited[0]), float(splited[2]))
                elif splited[1] == '√':
                    if splited[2] != "":
                        if splited[0] == '':
                            result = calc.square_root(float(splited[2]))
                        else:
                            result = calc.n_root(float(splited[2]), float(splited[0]))
                elif splited[1] == '!':
                    if float(splited[0]) % 1 == 0:
                        result = calc.fact(int(splited[0]))
                    else:
                        result = "Faktorial může být pouze z celých čísel"
                elif splited[0] == 'ln':
                    if splited[1] != "":
                        print(splited)
                        result = calc.ln(float(splited[1]))

            if str(result) != "":
                self.resultlabel.text = str(result)
            else:
                self.resultlabel.text = "nepodporované nebo neplatné zadání"
        except Exception as e:
            self.resultlabel.text = str(e)

    ## Funkce pri stisknuni tlacitka help
    #
    #  @param self Vola tridu MyGrid
    def help(self):
        self.manager.current = 'help'

## Trida grafickeho rozhrani pro okno napovedy
#
# @class Help
# @brief Graficke rozhrani pro napovedu volana tlacitkem help
class Help(Screen):
    def help(self):
        self.manager.current = 'app'

## Trida pro generovani okna pro graficke rozhrani
#
# @class KalkulatorApp
# @brief Vygeneruje okno a nacte soubor gui.kv
class KalkulatorApp(MDApp):
    def build(self):
        Window.size = 450, 640
        Window.minimum_width, Window.minimum_height = Window.size
        self.theme_cls.theme_style = 'Dark'
        KV = Builder.load_file('gui.kv')
        sc = ScreenManager()
        sc.add_widget(MyGrid(name='app'))
        sc.add_widget(Help(name='help'))
        return sc


if __name__ == '__main__':
    if hasattr(sys, '_MEIPASS'):
        resource_add_path(os.path.join(sys._MEIPASS))
    KalkulatorApp().run()