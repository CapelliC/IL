% mescolamento di liste
mescolamento(L, [], L).
mescolamento(L, [E|Rresto], S) :-
	conc(L1, Lresto, L),
	mescolamento(Lresto, Rresto, Sresto),
	conc(L1, [E|Sresto], S).

conc([], L, L).
conc([X|Xs], Ys, [X|Zs]) :-
	conc(Xs, Ys, Zs).

m(X,Y,Z) :- mescolamento(X,Y,Z).
