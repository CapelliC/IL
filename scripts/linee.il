:- consult(ansidrv), i_inherit(ansi_driver).

%-------------------------------
% traccia una linea orizzontale
%
linea_orizz(X - Y, Lung, Car) :-
	X1 is X + 1,
	Y1 is Y + 1,
	pos_cursore(Y1, X1),
	putn(Car, Lung).

%-----------------
% linea verticale
%
linea_vert(_ - Yf, Yf, _Car) :- !.
linea_vert(X - Yi, Yf, Car) :-
	pos_cursore(Yi, X),
	put(Car),
	Y1 is Yi + 1,
	!, linea_vert(X - Y1, Yf, Car).
