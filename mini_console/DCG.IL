%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Definite Clause Grammar translator
%	from Clocksin, Mellish
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

:- op(251, fx,   { ).
:- op(250, xf,   } ).
:- op(255, xfx, -->).

translate((P0 --> Q0), (P :- Q)) :-
	left_hand_side(P0, S0, S, P),
	right_hand_side(Q0, S0, S, Q1),
	flatten(Q1, Q), !.

left_hand_side((NT, Ts), S0, _S, P) :- !,
	nonvar(NT),
	islist(Ts),
	tag(NT, S0, S1, P),
	append(Ts, S0, S1).
left_hand_side(NT, S0, S, P) :-
	nonvar(NT),
	tag(NT, S0, S, P).

right_hand_side((X1, X2), S0, S, P) :- !,
	right_hand_side(X1, S0, S1, P1),
	right_hand_side(X2, S1, S, P2),
	and(P1, P2, P).
right_hand_side((X1 ; X2), S0, S, (P1 ; P2)) :-
	or(X1, S0, S, P1),
	or(X2, S0, S, P2).
right_hand_side({P}, S, S, P) :- !.
right_hand_side(!, S, S, !) :- !.
right_hand_side(Ts, S0, S, true) :-
	islist(Ts),
	!, append(Ts, S, S0).
right_hand_side(X, S0, S, P) :-
	tag(X, S0, S, P).

or(X, S0, S, P) :-
	right_hand_side(X, S0a, S, Pa),
	( var(S0a), S0a = S, !, S0 = S0a, ! = Pa;
	P = (S0 = S0a, Pa) ).
	
tag(X, S0, S, P) :-
	X =.. [F | A],
	append(A, [S0, S], AX),
	P =.. [F | AX].

and(true, P, P) :- !.
and(P, true, P) :- !.
and(P, Q, (P, Q)).

flatten(A, A) :-
	var(A), !.
flatten((A, B), C) :- !,
	flatten1(A, C, R),
	flatten(B, R).
flatten(A, A).

flatten1(A, (A, R), R) :-
	var(A), !.
flatten1((A, B), C, R) :- !,
	flatten1(A, C, R1),
	flatten1(B, R1, R).
flatten1(A, (A, R), R).

islist([]) :- !.
islist([_|_]).

append([A|B], C, [A|D]) :- append(B, C, D).
append([], X, X).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% convert DCG rules to clauses
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
d2c :-
	clause((H-->T),true),
	translate((H-->T), Clause),
	assert(Clause),
	display(Clause), nl,
	fail.
d2c.
