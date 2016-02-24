/*
	some list processing predicates
*/
:-	i_begin(list_processing),
	i_export([
		member,
		append,
		last,
		select,
		nextto,
		efface,
		delete,
		subst,
		reverse,
		rotate,

		union,
		intersection,
		difference,

		listlen,
		sumlist,

		atoi,
		itoa,

		element_at,
		index_of_element,
		get_nth_element,
		set_nth_element,
		split_list_at,

		list_elements_at,
		gen_int_range,
		gen_list_elements,

		transpose_col_row
	]).

%%%%%%%%%%%%%%/
% not deterministic member
%
member(X, [X|_]).
member(X, [_|Ys]) :- member(X, Ys).

%%%%%%%%%%%%%%/
% not deterministic append
%
append([], L, L).
append([X|Xs], L, [X|Ys]) :- append(Xs, L, Ys).

%%%%%%%%%%%%%%%%%
% fast reverse with accumulator
%
reverse(L, R) :- revzap(L, [], R).
revzap([], L, L).
revzap([X|Xs], L1, L2) :- revzap(Xs, [X|L1], L2).

%%%%%%%%%%%%%%%/
% return last element of list
%
last([X], X).
last([_|Xs], Y) :- last(Xs, Y).

%%%%%%%%%%%%%%%/
% get an element from a list
%
select(X, [X|Xs], Xs).
select(X, [Y|Ys], [Y|Zs]) :- select(X, Ys, Zs).

%%%%%%%%%%%%%%%%%%
% get the element following a key
%
nextto(X, Y, [X, Y|_]).
nextto(X, Y, [_|Z]) :- !, nextto(X, Y, Z).

%%%%%%%%%%%%%%%%%%%
% remove first occurrence from list
%
efface(X, [X|Xs], Xs).
efface(X, [Y|Ys], [Y|Zs]) :- !, efface(X, Ys, Zs).

%%%%%%%%%%%%%%%%%%
% remove all occurrences from list
%
delete(_, [], []).
delete(X, [X|Xs], L) :- !, delete(X, Xs, L).
delete(X, [Y|L1], [Y|L2]) :- !, delete(X, L1, L2).

%%%%%%%%%%%%%%%%%%%
% substitute all occurrences in list
%
subst(_, [], _, []).
subst(X, [X|L], A, [A|M]) :- !, subst(X, L, A, M).
subst(X, [Y|L], A, [Y|M]) :- subst(X, L, A, M).

%%%%%%%%%%%/
% compute list lenght
%
listlen(L, LL) :- llen(L, 0, LL).
llen([], L, L).
llen([_|T], L, LTot) :- L1 is  L+1, !, llen(T, L1, LTot).

%%%%%%%%%%%%%%%%%
% insiemistic union of elements
%
union([], L, L).
union([X|Xs], S, T) :- member(X, S), !, union(Xs, S, T).
union([X|Xs], S, T) :- !, union(Xs, [X|S], T).

%%%%%%%%%%%%%%%%%%%%/
% insiemistic intersection of elements
%
intersection([], _, []).
intersection([X|Xs], L, [X|Ys]) :- member(X, L), !, intersection(Xs, L, Ys).
intersection([_|Xs], L, Ys) :- !, intersection(Xs, L, Ys).

%%%%%%%%%%%%%%%%%%%%%/
% find all elements of X that aren't in Y
%
difference([], _, []).
difference([X|Xs], Ys, Zs) :- member(X, Ys), !, difference(Xs, Ys, Zs).
difference([X|Xs], Ys, [X|Zs]) :- !, difference(Xs, Ys, Zs).

%%%%%%%%%%%%%/
% search index of element
%
element_at(El, List, Index) :-
	var(Index),
	index_of_element(El, List, 1, Index).
element_at(El, List, Index) :-
	integer(Index), 
	get_nth_element(List, Index, El).

%%%%%%%%%%%%%%%%%%%/
% separe the list in two segments
%
split_list_at(L, 0, [], L).
split_list_at([H|T], N, [H|Th], After) :-
	N > 0,
	N1 is  N - 1,
	!, split_list_at(T, N1, Th, After).

%%%%%%%%%%%%%
% find index of element
%
index_of_element(El, [El|_List], OffList, OffList).
index_of_element(El, [_|List], OffList, OffList2) :-
	OffList1 is  OffList+1,
	!, index_of_element(El, List, OffList1, OffList2).

%%%%%%%%%%%%%%%%%/
% get element (index begin at 1)
%
get_nth_element([El|_List], 1, El).
get_nth_element([_|List], Index, El) :-
	Index > 1, Index1 is  Index - 1,
	!, get_nth_element(List, Index1, El).

%%%%%%%%%%%%%%%%%%/
% change element (index begin at 1)
%
set_nth_element([_|T], 1, E, [E|T]).
set_nth_element([H|T], Index, E, [H|R]) :-
	Index > 1, Index1 is  Index - 1,
	!, set_nth_element(T, Index1, E, R).

%%%%%%%%%%%%%/
% sum all numbers in list
%
sumlist([N|Ns], S1, S3) :- S2 is  S1 + N, !, sumlist(Ns, S2, S3).
sumlist([], Tot, Tot).

%%%%%%%%%%%%%%%%%%%%%%%%%%%
% build a list with all elements found at positions
%
list_elements_at([Col|Cols], Pos, [El|Acc]) :-
	get_nth_element(Col, Pos, El),
	!, list_elements_at(Cols, Pos, Acc).
list_elements_at([], _Pos, []).

%%%%%%%%%%%%%%%
% row/columns transposition
%
transpose_col_row([], []).
transpose_col_row([U], B) :- gen(U, B).
transpose_col_row([H|T], R) :- transpose_col_row(T, TC), splash(H, TC, R).

gen([H|T], [[H]|RT]) :- gen(T,RT).
gen([], []).

splash([], [], []).
splash([H|T], [R|K], [[H|R]|U]) :-
	splash(T,K,U).

%%%%%%%%%%%%%%%%%%%%%%%
% generate list of integers L..H, inclusive
%
gen_int_range(M, N, [M|Ns]) :-
	M < N, M1 is  M + 1,
	!, gen_int_range(M1, N, Ns).
gen_int_range(N, N, [N]).

%%%%%%%%%/
% rotate the list
%
rotate(right, L, [T|H]) :- append(H, [T], L).
rotate(left, [H|T], L) :- append(T, [H], L).

%%%%%%%%%%%%%%%%%%%/
% make a list of N repeated elements
%
gen_list_elements(0, _El, []).
gen_list_elements(N, H, [H|T]) :-
	N1 is  N - 1,
	!, gen_list_elements(N1, H, T).
	
%%%%%%%%%%%/
% convert to integer
%
atoi([], 0, 0) :- !.
atoi([C], V, 1) :- !,
	V is  C - 48.
atoi([C | Cs], V, F1) :- !,
	atoi(Cs, Vs, F),
	F1 is  F * 10,
	V is  Vs + (C - 48) * F1.

%%%%%%%%%%/
% convert to string
%
itoa(N, S) :-
	N < 0, !,
	NN is  0 - N,
	iptoa(NN, SR, _),
	reverse(SR, SN),
	append("-", SN, S).
itoa(N, S) :-
	iptoa(N, SR, _),
	reverse(SR, S).

iptoa(V, [C], 1) :-
	V < 10, !,
	C is  V + 48.
iptoa(V, [C|S], Y) :-
	M is  V / 10,
	iptoa(M, S, X),
	Y is  X * 10,
	C is  V - M * 10 + 48.

:-	i_end(list_processing).
