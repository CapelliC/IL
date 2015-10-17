/*
	gestione menu scelte
	con interfaccia a linea di comando
*/

:-	i_begin(menu),
	i_export([
		menu,
		menu_carattere,
		messaggio,
		prompt
	]),
	i_import([
		menu_azione/2,
		menu_titolo/1
	]).

menu(Codice, Scelte) :-
	repeat,
	titolo(Codice),
	mostra_scelte(Scelte),
	menu_carattere(Op),
	scegli_azione(Codice, Op, Scelte).

menu_carattere(Op) :-
	os_name(dos),
	getche(Op), nl.
menu_carattere(Op) :-
	( os_name(windows) ; os_name(win32) ), 
	get(Op), leggi_eol(Op), !.

messaggio(M, C) :-	write(M), nl, menu_carattere(C).
prompt(Mess, Valore) :- write(Mess), flush, read(Valore).

leggi_eol(10) :- !.
leggi_eol(X) :- X := -1, !.
leggi_eol(_) :- get(C), !, leggi_eol(C).

/////////////////////////////////////////////////////
// output titolo menu:
//	se non viene fornita la procedura, usa il codice
//
titolo(Codice) :-
	menu_titolo(Codice), !.
titolo(Codice) :-
	write(Codice), nl.

mostra_scelte([]) :- write('\n: '), flush.
mostra_scelte([Scelta=_|Scelte]) :-
	write(Scelta), nl,
	!, mostra_scelte(Scelte).

scegli_azione(_Menu, X, _Scelte) :-
	X := -1, !, usa_azione(_, menu_abort).
scegli_azione(_Menu, _Op, []) :-
	write('scelta non valida!'), put(7), nl,
	fail.
scegli_azione(Menu, Op, [Scelta=Azione|_Scelte]) :-
	primo_carattere(Op, Scelta),
	!, usa_azione(Menu, Azione).
scegli_azione(Menu, Op, [_Scelta|Scelte]) :-
	scegli_azione(Menu, Op, Scelte).

primo_carattere(Op, Comando) :-
	name(Comando, [PrimoCar|_]),
	( Op = PrimoCar ; Op := PrimoCar + 32 ; PrimoCar := Op + 32 ).

usa_azione(_, menu_fine).
usa_azione(_, menu_abort) :-
	write('menu interrotto!'), nl.
usa_azione(Menu, Azione) :-
	menu_azione(Menu, Azione), !, fail.
usa_azione(_, Azione) :-
	write(Azione), write(' e\' fallita!'), nl, get(_), fail.

:-	i_end(menu).
