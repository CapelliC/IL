//-----------------------------------------------
// nim player program
//	apply the best move strategy
//
// from Sterling and Shapiro - The art of Prolog
//	page 338
//

:- consult(play).

choose_move(_Position, opponent, Move) :-
	writeln(['please make your move : ']), read(Move).

choose_move(Ns, computer, Move) :-
	unsafe(Ns, Sum), safe_move(Ns, Sum, Move).
choose_move(Ns, computer, (1,1)) :-
	safe(Ns).

move((K,M), [N|Ns], [N|Ns1]) :-
	K > 1, K1 := K-1, move((K1,M), Ns, Ns1).
move((1,N), [N|Ns], Ns).
move((1,M), [N|Ns], [N1|Ns]) :-
	N > M, N1 := N-M.

display_game(Position,_X) :- write(Position), nl.

next_player(computer, opponent).
next_player(opponent, computer).

game_over([], Player, Player).

announce(computer) :- write('You won! Congratulations.'), nl.
announce(opponent) :- write('I won.'), nl.

initialize(nim, [1,3,5,7], opponent).

unsafe(Ns, Sum) :-
	nim_sum(Ns, [], Sum), not zero(Sum).
safe(Ns) :- not unsafe(Ns,_Sum).

nim_sum([N|Ns], Bs, Sum) :-
	binary(N, Ds), nim_add(Ds, Bs, Bs1), nim_sum(Ns, Bs1, Sum).
nim_sum([], _Sum, _Sum).

nim_add(Bs, [], Bs).
nim_add([], Bs, Bs).
nim_add([B|Bs], [C|Cs], [D|Ds]) :-
	D := (B+C) mod 2, nim_add(Bs, Cs, Ds).

binary(1, [1]).
binary(N, [D|Ds]) :-
	N > 1, D := N mod 2, N1 := N/2, binary(N1, Ds).

decimal(Ds, N) :- decimal(Ds, 0, 1, N).
decimal([], N,_T, N).
decimal([D|Ds], A, T, N) :-
	A1 := A+D*T, T1 := T*2, decimal(Ds, A1, T1, N).

zero([]).
zero([0|Zs]) :- zero(Zs).

safe_move(Piles, NimSum, Move) :-
	safe_move(Piles, NimSum, 1, Move).

safe_move([Pile|_Piles], NimSum, K, (K,M)) :-
	binary(Pile, Bs), can_zero(Bs, NimSum, Ds, 0), decimal(Ds, M).
safe_move([_Pile|Piles], NimSum, K, Move) :-
	K1 := K+1, safe_move(Piles, NimSum, K1, Move).

can_zero([], NimSum, [], 0) :-
	zero(NimSum).
can_zero([_B|Bs], [0|NimSum], [C|Ds], C) :-
	can_zero(Bs, NimSum, Ds, C).
can_zero([B|Bs], [1|NimSum], [D|Ds], C) :-
	D := 1-B*C, C1 := 1-B, can_zero(Bs, NimSum, Ds, C1).
