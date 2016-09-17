%-----------------------------------------------
% master mind playing program
%	do an exaustive search of all configurations
%
% from Sterling and Shapiro - The art of Prolog
%	page 334
%

mastermind(Code) :-
	cleanup, guess(Code), check(Code), announce.

guess([A,B,C,D]) :-
	selects([A,B,C,D], [1,2,3,4,5,6,7,8,9,0]).

check(Guess) :-
	not inconsistent(Guess), ask(Guess).

inconsistent(Guess) :-
	query(OldGuess, Bulls, Cows),
	not bulls_and_cows_match(OldGuess, Guess, Bulls, Cows).

bulls_and_cows_match(OldGuess, Guess, Bulls, Cows) :-
	exact_matches(OldGuess, Guess, Bulls),
	common_members(OldGuess, Guess, N2),
	Cows is  N2 - Bulls.

ask(Guess) :-
	repeat,
	writeln(['How many bulls and cows in ', Guess, ' ? ']),
	%	replace user input: old code was
	%	read((Bulls, Cows))
		readmatches(Guess, (Bulls, Cows)),
	sensible(Bulls, Cows), !,
	asserta(query(Guess, Bulls, Cows)),
	Bulls = 4.

sensible(Bulls, Cows) :-
	integer(Bulls),
	integer(Cows),
	N is  Bulls + Cows,
	N =< 4.

announce :-
	write(found), nl.

exact_matches(Xs, Ys, N) :- exact_matches(Xs, Ys, 0, N).

exact_matches([X|Xs], [X|Ys], K, N) :-
	K1 is  K+1, !, exact_matches(Xs, Ys, K1, N).
exact_matches([X|Xs], [Y|Ys], K, N) :-
	X \= Y, !, exact_matches(Xs, Ys, K, N).
exact_matches([], [], N, N).

common_members(Xs, Ys, N) :- common_members(Xs, Ys, 0, N).

common_members([X|Xs], Ys, K, N) :-
	member(X, Ys), K1 is  K+1, !, common_members(Xs, Ys, K1, N).
common_members([_X|Xs], Ys, K, N) :-
	!, common_members(Xs, Ys, K, N).
common_members([], _Ys, N, N).

cleanup :-
	retract(query(_,_,_)),
	fail.
cleanup.

selects([X|Xs], Ys) :- select(X, Ys, Ys1), selects(Xs, Ys1).
selects([], _Ys).

select(X, [X|Xs], Xs).
select(X, [Y|Ys], [Y|Zs]) :- select(X, Ys, Zs).

member(X, [X|_]).
member(X, [_|Xs]) :- member(X, Xs).

%-----------------
% test begin here
%
m :- initc(TimeStart), mastermind(_), endc(TimeStart).

initc(_) :-
	retract(code_to_guess(_)), fail.
initc(Time) :-
	write('Code : '),flush,
	read((A,B,C,D)),
	assert(code_to_guess([A,B,C,D])),
	timecurr(Time).
endc(TStart) :-
	timecurr(TCurr), timediff(TCurr, TStart, TDiff),
	writeln(['solved in ', TDiff, ' secs\n']).

% make the program respond counting matches
readmatches(Guess, (Bulls, Cows)) :-
	code_to_guess(Code),
	bulls_and_cows_match(Code, Guess, Bulls, Cows),
	writeln([Bulls, ',', Cows, '.']), nl.
