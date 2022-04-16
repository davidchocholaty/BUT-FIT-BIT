/*
 * Soubor: menu.js
 * Popis: Javascript pro menu webove stranky.
 * Datum: 2021-03-31
 * Autor: David Chocholaty <xchoch09@stud.fit.vutbr.cz>
 * Verze: 1.0
 * Projekt: Projekt 2, predmet ITW
 */

/*
 * Zobrazeni menu pro mobilni zarizeni
 */
function switchNav(){	
	if (window.innerWidth < 960) {
		document.getElementsByClassName("navigation")[0].classList.toggle("active");
		// Pro zakaz scrollovani pri otevrenem menu
		document.getElementsByClassName("body")[0].classList.toggle("fixed-position");
	}    
}    	

/*
 * Sticky menu
 */
var stickyMenu = document.getElementById("sticky-menu")
var stickyMenuOffset = stickyMenu.offsetTop

window.addEventListener("scroll", function(e){
	requestAnimationFrame(function(){
		if (window.pageYOffset > stickyMenuOffset){
			stickyMenu.classList.add("sticky")			
		}
		else{
			stickyMenu.classList.remove("sticky")			
		}        
	})
})

/*-------------- Konec souboru menu.js --------------*/
