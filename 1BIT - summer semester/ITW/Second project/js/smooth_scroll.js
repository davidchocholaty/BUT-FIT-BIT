/*
 * Soubor: smooth_scroll.js
 * Popis: Javascript pro smooth scroll.
 * Datum: 2021-04-03
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Verze: 1.0
 * Projekt: Projekt 2, predmet ITW
 */

/*
 * Odkazy obsahujicich znak "#".
 */
let anchorSelector = 'a[href^="#"]';
      
/*
 * Vyber odkazu dle anchorSelector.
 */
let anchorList = document.querySelectorAll(anchorSelector);
          
/*
 * Pruchod vsemi odkazy.
 */
anchorList.forEach(link => {
    link.onclick = function (e) {
        
        // Zakaz posouvani pri prazdne hodnote hash.
        e.preventDefault();
            
        // Ziskani umisteni ke scroll.
        let destination = document.querySelector(this.hash);
            
        // Scroll na danou destinaci.
        destination.scrollIntoView({
        behavior: 'smooth'
        });
    }
});

/*---------- Konec souboru smooth_scroll.js ---------*/
