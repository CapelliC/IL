:- consult(linee).

%-----------------------
% traccia un rettangolo
%
rettangolo(Pos, Dim, Car, Col) :-
	imposta_colori(Col),
	rettangolo(Pos, Dim, Car),
	!.

rettangolo(Pos, Dim, Col) :-
	imposta_colori(Col),
	[Car] = " ",
	rettangolo(Pos, Dim, Car),
	!.

% traccia con colore attuale, carattere
rettangolo(Pos, Dim) :-
	[Car] = " ",
	rettangolo(Pos, Dim, Car),
	!.

% traccia con colore attuale
rettangolo(Pos, Dim, Car) :-
	rettangolo(Pos, 0, Dim, Car).

% questa fa il vero lavoro...
rettangolo(_Pos, L, _N * L, _Car) :- !.
rettangolo(X - Y, L, N * M, Car) :-
	linea_orizz(X - Y, N, Car),
	L1 is L + 1,
	Y1 is Y + 1,
	!, rettangolo(X - Y1, L1, N * M, Car).

%-----------------------------------
% visualizzazione di una scacchiera
%
mostra_scacchiera(Pos, DimScac, DimCas, Col) :-
	mostra_scacchiera(Pos, 0, DimScac, DimCas, Col).

mostra_scacchiera(_PosRiga,  L, _N * L, _DimC, _Col) :- !.
mostra_scacchiera(X - YRiga, L, N * M, NC * MC, Col) :-
	mostra_linea_scacc(X - YRiga, 0, N, NC * MC, Col),
	YRiga1 is YRiga + MC,
	L1 is L + 1,
	scambia_colori(Col, Col1),
	!, mostra_scacchiera(X - YRiga1, L1, N * M, NC * MC, Col1).

mostra_linea_scacc(_Pos,  M, M, _DimCas, _Col) :- !.
mostra_linea_scacc(X - Y, L, M, NC * MC, Col) :-
	rettangolo(X - Y, NC * MC, Col),
	X1 is X + NC,
	L1 is L + 1,
	scambia_colori(Col, Col1),
	!, mostra_linea_scacc(X1 - Y, L1, M, NC * MC, Col1).
