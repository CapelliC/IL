/***********************************************************

	Sequenze escape ANSI per lo spostamento del cursore,
	per i grafici e per le impostazioni della tastiera
	 
	----------------------------------------------------
	Convenzioni sui parametri utilizzati nelle sequenze:
	 
	Pn	Parametro numerico. Indica un numero decimale.
	 
	Ps	Parametro selettivo. Indica il numero decimale che
	    viene utilizzato per selezionare una funzione. E'
	    possibile utilizzare piu' di una funzione separando i
	    parametri con il punto e virgola.
	 
	Pr	Parametro di riga. Indica il numero decimale che
	    rappresenta una delle righe dello schermo o di
	    un'altra periferica.
	 
	Pc	Parametro di colonna. Indica il numero decimale che
	    rappresenta una delle colonne dello schermo o di
	    un'altra periferica.

***********************************************************/

:-	i_begin(ansi_driver),
	i_export([
		pos_cursore / 2,
		pos_cursore / 0,
		cursore_su / 1,
		cursore_giu / 1,
		cursore_avanti / 1,
		cursore_indietro / 1,
		salva_pos_cursore / 0,
		ripristina_pos_cursore / 0,
		cancella_schermo / 0,
		cancella_riga / 0,
		imposta_colori / 1,
		scambia_colori / 2,
		imposta_grafica / 1,
		sfondo_default / 0,
		imposta_modalita / 1,
		imposta_modalita / 2,
		reimposta_modalita / 1,
		reimposta_modalita / 2
	]).

/*
	sposta il cursore su un punto specificato da coordinate.
	Se non viene specificata alcuna posizione, il cursore si sposta
	nella posizione iniziale corrispondente all'angolo superiore
	sinistro dello schermo (riga 1, colonna 1).
*/
pos_cursore(Pr, Pc) :-
	writeln(['[', Pr, ';', Pc, 'H']).
pos_cursore :-
	write('[;H').
 
/*
	sposta il cursore verso l'alto del numero di righe
	specificato senza cambiare colonna.
	Se il cursore si trova gia' sulla prima riga,
	ANSI.SYS ignorera' questa sequenza.
*/
cursore_su(Pn) :-
	writeln(['[', Pn, 'A']).
 
/*
	sposta il cursore verso il basso del numero di righe
	specificato senza cambiare colonna.
	Se il cursore si trova gia' sull'ultima
	riga, ANSI.SYS ignorera' questa sequenza.
*/ 
cursore_giu(Pn) :-
	writeln(['[', Pn, 'B']).
 
/*
	sposta il cursore in avanti del numero di colonne
	specificato senza cambiare riga.
	Se il cursore si trova gia' sulla colonna
	all'estrema destra, ANSI.SYS ignorera' questa sequenza.
*/
cursore_avanti(Pn) :-
	writeln(['[', Pn, 'C']).

/*
	sposta il cursore indietro del numero di colonne
	specificato senza cambiare riga.
	Se il cursore si trova gia' sulla colonna
	all'estrema sinistra, ANSI.SYS ignorera' questa sequenza.
*/
cursore_indietro(Pn) :-
	writeln(['[', Pn, 'D']).
 
/*
	memorizza la posizione attuale del cursore.
	Per spostare il cursore nella posizione memorizzata, 
	e' possibile utilizzare ripristina_pos_cursore.
*/
salva_pos_cursore :-
	write('[s').
 
/*
	riporta il cursore nella posizione
	memorizzata dalla sequenza Salva la posizione del cursore.
*/ 
ripristina_pos_cursore :-
	write('[u').

/*
	libera lo schermo e riporta il cursore alla posizione
	iniziale (riga 0, colonna 0).
*/ 
cancella_schermo :-
	write('[2J').
 
/* 
	cancella tutti i caratteri iniziando dalla posizione del
	cursore fino alla fine della riga (compreso il carattere sul quale 
	e' posizionato il cursore).
*/
cancella_riga :-
	write('[K').
 
/*
	modifica i colori correnti
*/
imposta_colori(Col_pp + Col_sf) :-
	imposta_grafica([col_pp = Col_pp, col_sf = Col_sf]).

scambia_colori(Col_pp + Col_sf, Col_sf + Col_pp).

/*
	richiama le funzioni grafiche specificate dai valori che seguono.
	Tali funzioni rimangono attive fino alla successiva
	occorrenza di questa sequenza escape.
	La modalita' grafica modifica i colori e gli attributi
	(come grassetto e sottolineatura) del testo visualizzato sullo schermo.
*/

imposta_grafica(Comandi) :-
	write('['), lcmd(Comandi), !.

lcmd([C]) :- cmd(C), write('m'), !.
lcmd([C|Cs]) :- cmd(C), write(';'), !, lcmd(Cs).

% Controllo attributi
cmd(attr = disattiva)	:- write(0).
cmd(attr = grassetto)	:- write(1).
cmd(attr = sottolineato):- write(4).
cmd(attr = intermitt)	:- write(5).
cmd(attr = inverso)		:- write(7).
cmd(attr = nascosto)	:- write(8).
 
% Colori in primo piano
cmd(col_pp = nero)		:- write(30).
cmd(col_pp = rosso)		:- write(31).
cmd(col_pp = verde)		:- write(32).
cmd(col_pp = giallo)	:- write(33).
cmd(col_pp = blu)		:- write(34).
cmd(col_pp = magenta)	:- write(35).
cmd(col_pp = azzurro)	:- write(36).
cmd(col_pp = bianco)	:- write(37).

% Colori di sfondo
cmd(col_sf = nero)		:- write(40).
cmd(col_sf = rosso)		:- write(41).
cmd(col_sf = verde)		:- write(42).
cmd(col_sf = giallo)	:- write(43).
cmd(col_sf = blu)		:- write(44).
cmd(col_sf = magenta)	:- write(45).
cmd(col_sf = azzurro)	:- write(46).
cmd(col_sf = bianco)	:- write(47).

sfondo_default :- imposta_grafica([col_sf=nero,col_pp=bianco]).

/*
	modifica l'ampiezza o il tipo di schermo
*/
imposta_modalita( 40 * 25,	mono)			:- imposta_modalita(0).
imposta_modalita( 40 * 25,	colore)			:- imposta_modalita(1).
imposta_modalita( 80 * 25,	mono)			:- imposta_modalita(2).
imposta_modalita( 80 * 25,	colore)			:- imposta_modalita(3).
imposta_modalita(320 * 200, colore)			:- imposta_modalita(4).
imposta_modalita(320 * 200, mono)			:- imposta_modalita(5).
imposta_modalita(640 * 200, mono)			:- imposta_modalita(6).
imposta_modalita(320 * 200, colore(16))		:- imposta_modalita(13).
imposta_modalita(640 * 200,	mono)			:- imposta_modalita(14).
imposta_modalita(640 * 350, mono)			:- imposta_modalita(15).
imposta_modalita(640 * 350, colore)			:- imposta_modalita(16).
imposta_modalita(640 * 480, mono)			:- imposta_modalita(17).
imposta_modalita(640 * 480, colore)			:- imposta_modalita(18).
imposta_modalita(320 * 200, colore(256))	:- imposta_modalita(19).
imposta_modalita(			a_capo)			:- imposta_modalita(7).

imposta_modalita(			Ps)				:- writeln(['[=', Ps, 'h']).

/*
	reimposta la modalita' utilizzando gli stessi valori
	della sequenza Imposta modalita', ad eccezione del 7
	che disattiva il ritorno a capo automatico.
*/
reimposta_modalita(a_capo) :-
	!, write('[=7l').
reimposta_modalita(Ps) :-
	!, writeln(['[=', Ps, 'l']).
reimposta_modalita(Modo, Spec) :-
	imposta_modalita(Modo, Spec).

:-	i_end(ansi_driver).
