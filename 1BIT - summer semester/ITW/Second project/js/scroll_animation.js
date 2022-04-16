/*
 * Soubor: scroll_animation.js
 * Popis: Javascript pro scroll animace webove stranky.
 * Datum: 2021-04-01
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Verze: 1.0
 * Projekt: Projekt 2, predmet ITW
 */

 /*
  * Vyber elementu pro scroll.
  */
const scrollElements = document.querySelectorAll(".js-scroll");

const elementInView = (el, dividend = 1) => {
  /* 
   * Ziskani velikosti a pozice na viewport.
   */
  const elementTop = el.getBoundingClientRect().top;

  return (
    elementTop <=
    (window.innerHeight || document.documentElement.clientHeight) / dividend
  );
};

/*
 * Pridani tridy scrolled.
 */

const displayScrollElement = (element) => {
  element.classList.add("scrolled");
};

/*
 * Zmena typu animace pro mobilni zarizeni
 */
function switchAnimation(){	
	if (window.innerWidth < 960) {     
    /*---- Odstraneni animaci pro desktop. ----*/    

    /* About */
    document.getElementById("about-fst-col").classList.remove("slide-left");
    document.getElementById("about-sec-col").classList.remove("slide-right");
    /* Education */
    document.getElementById("edu-fst-col").classList.remove("slide-right");		
    document.getElementById("edu-sec-col").classList.remove("slide-left");
    /* Hobbies */
    document.getElementById("fst-col-fst").classList.remove("slide-left");
    document.getElementById("sec-col-sec").classList.remove("slide-left");
    document.getElementById("trd-col-fst").classList.remove("slide-left");
    document.getElementById("fth-col-sec").classList.remove("slide-left");

    document.getElementById("fst-col-sec").classList.remove("slide-right");
    document.getElementById("sec-col-fst").classList.remove("slide-right");
    document.getElementById("trd-col-sec").classList.remove("slide-right");
    document.getElementById("fth-col-fst").classList.remove("slide-right");

    /*---- Pridani animaci pro desktop. ----*/

    /* About */
		document.getElementById("about-fst-col").classList.add("fade-in");
    document.getElementById("about-sec-col").classList.add("fade-in-bottom");
    /* Education */
    document.getElementById("edu-fst-col").classList.add("fade-in");		
    document.getElementById("edu-sec-col").classList.add("fade-in-bottom");    
    /* Hobbies */
    document.getElementById("fst-col-fst").classList.add("fade-in");
    document.getElementById("sec-col-sec").classList.add("fade-in-bottom");
    document.getElementById("trd-col-fst").classList.add("fade-in");
    document.getElementById("fth-col-sec").classList.add("fade-in-bottom");

    document.getElementById("fst-col-sec").classList.add("fade-in-bottom");
    document.getElementById("sec-col-fst").classList.add("fade-in");
    document.getElementById("trd-col-sec").classList.add("fade-in-bottom");
    document.getElementById("fth-col-fst").classList.add("fade-in");
	}    
}  

const handleScrollAnimation = () => {
  scrollElements.forEach((el) => {
    if (elementInView(el, 1.25)) {
      switchAnimation();
      displayScrollElement(el);
    } 
  })
}

window.addEventListener("scroll", () => { 
  handleScrollAnimation();
});  

/*-------- Konec souboru scroll_animation.js --------*/
