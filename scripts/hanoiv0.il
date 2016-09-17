:- consult(ansi).

hanoi(N):-
	range(1, N, L),
	assert(poles(L/[]/[])),
	move(L, l, c, r),
	retract(poles(P)),
	show(P),
	!.

move([], _, _, _) :- !.
move([_|T], A, B, C) :-
	move(T, A, C, B),
	inform(A, B),
	!,
	move(T, C, B, A).

inform(A, B) :-
	retract(poles(P0)),
	show(P0),
	pop(P0, A, N, P1),
	push(P1, B, N, P2),
	assert(poles(P2)).

show(L/C/R) :-
	cancella_schermo,
	showpole(L, _, 10),
	showpole(C, _, 30),
	showpole(R, _, 50),
	nl.
	%getch(_).

showpole([], 20, _).
showpole([N|T], R1, C) :-
	showpole(T, RC, C),
	R1 is RC - 1,
	CC is C - N,
	pos_cursore(RC, CC),
	DD is N * 2,
	putn(220, DD).


pop([L|T]/C/R, l, L, T/C/R).
pop(L/[C|T]/R, c, C, L/T/R).
pop(L/C/[R|T], r, R, L/C/T).

push(L/C/R, l, N, [N|L]/C/R).
push(L/C/R, c, N, L/[N|C]/R).
push(L/C/R, r, N, L/C/[N|R]).

range(N, N, [N]) :- !.
range(N, M, [N | Ns]) :-
	N1 is N + 1,
	!,
	range(N1, M, Ns).
