; Vernamova sifra na architekture DLX
; David Chocholaty xchoch09

        .data 0x04          ; zacatek data segmentu v pameti
login:  .asciiz "xchoch09"  ; login
cipher: .space 9 	    ; pro ukladani sifrovanych znaku (koncici 0)

        .align 2            ; dale zarovnavej na ctverice (2^2) bajtu
laddr:  .word login         ; 4B adresa vstupniho textu (pro vypis)
caddr:  .word cipher        ; 4B adresa sifrovaneho retezce (pro vypis)

        .text 0x40          ; adresa zacatku programu v pameti
        .global main        ; 

main:
	; registry: xchoch09-r1-r10-r17-r18-r21-r0
	; sifra: xchoch09 -> aukgfz


	addi r1, r0, login     ; nacteni adresy retezce login do registru r1
	                       ; r1 -> adresa aktualne sifrovaneho znaku retezce login

	add r10, r0, r0        ; r10 -> urceni licheho/sudeho sifrovaciho znaku - 0 (lichy), 1 (sudy)
	
	addi r18, r0, cipher   ; r18 -> adresa, kam se bude ukladat zasifrovany znak
	
while:
	lb r17, 0(r1) 	       ; nacteni sifrovaneho znaku
	                       ; r17 -> sifrovany znak

	nop		       ; z duvodu nacitani
 
	; test, zda se nejedna o cislici - hodnota je mensi nez znak 'a' (ascii 97)
	slti r21, r17, 97
	bnez r21, wend	

	; test, zda se jedna o lichy nebo sudy znak sifrovaciho klice
	seqi r21, r10, 1
	bnez r21, even
	
	nop
	nop

	; LICHY znak sifrovaciho klice -> 'c'
	; pricteni hodnoty znaku sifrovaciho klice k sifrovanemu znaku
	addi r17, r17, 3

	; test, zda hodnota zasifrovaneho znaku je mensi nebo rovna hodnote znaku 'z'
	slei r21, r17, 122
	bnez r21, save
	
	nop
	nop

	; zasifrovany znak presahl rozsah
	; prepocet od zacatku abecedy je pomoci odecteni poctu znaku en abecedy (26)
	subi r17, r17, 26

	j save	

	nop
	nop

even:
	; SUDY znak sifrovaciho klice -> 'h'
	; odecteni hodnoty znaku sifrovaciho klice od sifrovaneho znaku
	subi r17, r17, 8

	; test, zda hodnota zasifrovaneho znaku je vetsi nebo rovna hodnote znaku 'a'
	sgei r21, r17, 97
	bnez r21, save

	nop
	nop

	; zasifrovany znak je pod rozsahem
	; prepocet na konec abecedy je pomoci pricteni poctu znaku en abecedy (26)
	addi r17, r17, 26

save:
	
	sb 0(r18), r17	       ; ulozeni zasifrovaneho znaku

	; posun o jeden znak dale
	addi r1, r1, 1
	addi r18, r18, 1

	; zmena licheho/sudeho sifrovaciho znaku znaku
	; r10 muze obsahovat dekadickou hodnotu 0 nebo 1 -> pomoci xor s 1 se meni dana hodnota
	xori r10, r10, 1
	
	; opakovani cyklu
	j while

	nop
	nop

wend:
	; pridani ukoncovaciho znaku retezce 0
	addi r21, r0, 0 
	sb 0(r18), r21	
	

end:    addi r14, r0, caddr ; <-- pro vypis sifry
        trap 5  ; vypis textoveho retezce (jeho adresa se ocekava v r14)
        trap 0  ; ukonceni simulace
