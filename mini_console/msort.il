:-	i_begin(mergesort),
	i_export(mergesort),
	i_import(less_than/3).

mergesort(_Key, [], []). % :- !.
mergesort(_Key, [A], [A]) :- !.
mergesort(Key, L, Ordinata) :-
	split(L, L1, L2),
	mergesort(Key, L1, O1),
	mergesort(Key, L2, O2),
	merge(Key, O1, O2, Ordinata).

split([], [], []).
split([A], [A], []).
split([A,B|Resto], [A|RestoA], [B|RestoB]) :-
	!, split(Resto, RestoA, RestoB).

merge(_Key, A, [], A) :- !.
merge(_Key, [], A, A) :- !.
merge(Key, [A|RestoA], [B|RestoB], [A|Merged]) :-
	less_than(Key, A, B),
	!, merge(Key, RestoA, [B|RestoB], Merged).
merge(Key, A, [B|RestoB], [B|Merged]) :-
	!, merge(Key, A, RestoB, Merged).

:-	i_end(mergesort).
