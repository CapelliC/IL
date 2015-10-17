//--------------------------------------------
// tabella simboli per stringhe
//	basata su albero con porzioni di stringhe
//
ts_crea(Stringa, Valore, t(Stringa, Valore)).

ts_cerca(t(Stringa, Valore), Stringa, Valore).
ts_cerca(n(Iniz, Sin, Des), Str, Valore) :-
	append(Iniz, Coda, Str),
	(	ts_cerca(Sin, Coda, Valore) ;
		ts_cerca(Des, Coda, Valore) ).

ts_inserisci(t(S1, V1), S2, V2, n(SS, t(S11, V1), t(S22, V2))) :-
	primi_uguali(S1, S2, SS),
	append(SS, S11, S1),
	append(SS, S22, S2).
ts_inserisci(n(Str, Sin, Des), S, V, n(Str1, Sin1, Des1)) :-
n(SS, t(S11, V1), t(S22, V2))) :-

primi_uguali([X|T1], [X|T2], [X|T]) :-
	!, primi_uguali(T1, T2, T).
primi_uguali(_1, _2, []).

prefix([],_).
prefix([X | L],[X | N]) :- prefix(L,N).

append([],L,L).
append([X | L1],L2,[X | L3]) :- append(L1,L2,L3).

inslista(TS, [], TS).
inslista(TS, [H,V|T], TS2) :-
	ts_inserisci(TS, H, V, TS1),
	inslista(TS1, T, TS2).

t :- 	ts_crea("pippo", 88, TS),
		ts_inserisci(TS, "minni", 88, TS1),
	//	inslista(TS, ["piuma", 99, "minni", 100], TS1),
		write(TS1), nl.
:- tracefile(sch), trace.
