/*
** problema delle N regine
**
**	- Sterling and Shapiro : The Art Of Prolog
*/

%------------------------------------------------------------
% inizia la ricerca creando la lista delle regine da provare
%
regine(N, Regine) :-
	intervallo(1, N, ListaN),
	regine(ListaN, [], Regine).

%---------------------------------------------
% ciclo di selezione e valutazione
%	attacca(), il valutatore di posizione,
%	scarta le permutazioni generate da
%	seleziona(), il generatore di mosse
%
regine(RegineNonPoste, RegineNonAtt, Regine) :-
	seleziona(Regina, RegineNonPoste, RegineNonPoste1),
	not attacca(Regina, RegineNonAtt),
	regine(RegineNonPoste1, [Regina | RegineNonAtt], Regine).
regine([], Regine, Regine).

%-------------------------------------------------
% seleziona() prende un elemento dalla lista
%	qui viene usato in modo (-,+,-), per estrarre gli elementi
%	generando permutazioni durante la riesecuzione
%
seleziona(X, [X | Xs], Xs).
seleziona(X, [Y | Ys], [Y | Zs]) :-
	seleziona(X, Ys, Zs).

%----------------------------------------------------------------------
% una regina ne attacca un' altra se si trovano sulla stessa diagonale.
%	Notare che il test su righe/colonne e' reso inutile
%	dalla rappresentazione dei dati
%
attacca(X, Xs) :-
	attacca(X, 1, Xs).

attacca(X, N, [Y | _Ys]) :-
	X is Y + N ; X is Y - N.
attacca(X, N, [_Y | Ys]) :-
	N1 is N + 1,
	attacca(X, N1, Ys).

%---------------------------------------------
% crea la lista di interi da M ad N, compresi
%
intervallo(M, N, [M | Ns]) :-
	M < N,
	M1 is M + 1,
	!,
	intervallo(M1, N, Ns).
intervallo(N, N, [N]).

%--------------------------
% test con visualizzazione
%
reg(N) :-
	iniz_visual(N, DatiVis),
	regine(N, L),				% cerca una soluzione
	visualizza_sol(L, DatiVis),
	fail.						% forza le soluzioni successive
reg(_) :-
	fine_visual.
 
:- consult(scacch).

% inizializza i parametri di visualizzazione
iniz_visual(N, dv(N, Pos, Dim, Col)) :-
	( retract(soluzione(_)), fail ;
	Px is 40 - N * 2,
	Py is (25 - N) / 2,
	Pos = Px - Py,
	Dim = 2 * 1,
	Col = blu + rosso ).
fine_visual :-
	cancella_schermo,
	write('scrivo le soluzioni sul file \'reg.sol\''), nl,
	findall(X, soluzione(X), L),
	tell('reg.sol'),
	fine_visual(L, 0, N),
	told,
	writeln(['trovate ', N, ' soluzioni. Bye!']), nl.

fine_visual([H|T], N, N2) :-
	write(H), nl, N1 is  N + 1, !, fine_visual(T, N1, N2).
fine_visual([], N, N).

% visualizza la soluzione trovata
visualizza_sol(L, dv(N, Pos, Dim, Col)) :-
	cancella_schermo,
	mostra_scacchiera(Pos, N*N, Dim, Col),
	mostra_regine(L, Pos, 1, Dim, Col),
	sfondo_default, nl, nl,
	assert(soluzione(L)),
	showstatus([base]), get0(_).

mostra_regine([],	   _Pos,_Y,_Dim,_Col).
mostra_regine([R | Rr],	Pos, Y, Dim, Col) :-
	colore_regina(R, Y, Col),
	scrivi_regina(R, Y, Pos, Dim),
	Y1 is Y + 1,
	!,
	mostra_regine(Rr, Pos, Y1, Dim, Col).

colore_regina(R, Y, Col) :-
	0 is (R + Y) mod 2,
	imposta_colori(Col).
colore_regina(_, _, C1) :-
	scambia_colori(C1, C2),
	imposta_colori(C2).

scrivi_regina(R, Y, Px - Py, N * M) :-
	Xc is Px + R * N - 1,
	Yc is Py + Y * M,
	pos_cursore(Yc, Xc),
	write('RR').
