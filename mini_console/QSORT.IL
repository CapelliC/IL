:-	i_begin(quicksort),
	i_export(quicksort),
	i_import([less_equal/2, greater_than/2]).

quicksort([], []).
quicksort([H | T], S) :-
	split(H,T,A,B),
	quicksort(A, A1),
	quicksort(B, B1),
	append(A1, [H | B1], S).

append([], L, L).
append([H | T], L, [H | V]) :-
	!, append (T, L, V).

split(H, [A | X], [A | Y], Z) :- less_equal(A, H), !, split(H, X, Y, Z).
split(H, [A | X], Y, [A | Z]) :- greater_than(A, H), !, split(H, X, Y, Z).
split(_, [], [], []).

:-	i_end(quicksort).
