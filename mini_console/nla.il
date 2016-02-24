:-	i_begin(infinite_integers),
	i_export([

		ii_add/3,
		ii_mul/3,
		ii_sub/3,
		ii_div/3,
		ii_write/1,

		ar_add,
		ar_mul,
		ar_sub
	]).

ii_write([]).
ii_write([N|Ns]) :- put(N), ii_write(Ns).

ii_add(N1, N2, R) :-
	revm(N1, [], R1),
	revm(N2, [], R2),
	ar_add(R1, R2, 0, RR),
	revp(RR, [], R), !.

ii_mul(N1, N2, R) :-
	revm(N1, [], R1),
	revm(N2, [], R2),
	ar_mul(R1, R2, [], RR),
	revp(RR, [], R), !.

ii_sub(N1, N2, R) :-
	revm(N1, [], R1),
	revm(N2, [], R2),
	ar_sub(R1, R2, 0, RR),
	revp(RR, [], R), !.

%%%%% ADDs %%%%%

ar_add([N|Ns], [M|Ms], Rip, [R|Rs]) :-
	Rt is  N + M + Rip,
	addrip(Rt, R, Rip1),
	ar_add(Ns, Ms, Rip1, Rs).
ar_add([], N2, 0, N2).
ar_add(N1, [], 0, N1).
ar_add(N1, [], 1, R) :- add1(N1, R).
ar_add([], N2, 1, R) :- add1(N2, R).
ar_add([], [], 0, []).
ar_add([], [], 1, [1]).

add1([9|Ns], [0|Rs]) :- add1(Ns, Rs).
add1([N|Ns], [R|Ns]) :- R is  N+1.
add1([], [1]).

addrip(Rt, R, 1) :-
	Rt >= 10, R is  Rt - 10.
addrip(Rt, Rt, 0).

%%%/ MULs %%%/

ar_mul(N, [M], P, R) :-
	mul1(N, M, 0, T),
	ar_add(T, P, 0, R).
ar_mul(N, [M|Ms], P, R) :-
	mul1(N, M, 0, T),
	ar_add(P, T, 0, P1),
	ar_mul([0|N], Ms, P1, R).

mul1([N|Ns], M, R, [X|Xs]) :-
	T is  N * M + R,
	X is  T mod 10,
	R1 is  T / 10,
	mul1(Ns, M, R1, Xs).
mul1([], _M, 0, []).
mul1([], _M, R, [R]).

%%%%% SUBs %%%%/

ar_sub([X|Xs], [Y|Ys], R, [Z|Zs]) :-
	T is  X - Y - R,
	subrip(T, Z, R1),
	ar_sub(Xs, Ys, R1, Zs).
ar_sub([X|Xs], [], R, [Z|Xs]) :-
	T is  X - R,
	subrip(T, Z, _).
ar_sub([], [], 0, []).

subrip(T, Z, 1) :-
	T < 0, Z is  10 + T.
subrip(T, T, 0).

%%%%/ REVs %%%/

revm([], L, L).
revm([X|Xs], L1, L2) :- Y is  X-48, revm(Xs, [Y|L1], L2).

revp([], L, L).
revp([X|Xs], L1, L2) :- Y is  X+48, revp(Xs, [Y|L1], L2).

:-	i_end(infinite_integers).
