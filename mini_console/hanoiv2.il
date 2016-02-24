%----------------------------------
% towers of hanoi with display
%----------------------------------

hanoi(N) :-
	init(N, L, H),
	move(L, H, l, c, r, _).	% algorithmic solution apply

%-------------------------------------------
% move N-1 disks using target pole as spare
% move the disk to target
% move N-1 disks using source pole as spare
%
move([], H, _, _, _, H).
move([_|T], H, A, B, C, H3) :-
	move(T, H, A, C, B, H1),
	inform(H1, A, B, H2),
	!, move(T, H2, C, B, A, H3).

%----------------
% displayer code
%----------------

:- consult(ansidrv), i_inherit(ansi_driver).

inform(H, A, B, H2) :-		% update poles positions

	getp(H, A, N, TA, H1),	% move top disk from pole A
	movecurs(A, TA, N),		% remove from screen
	diskplace(off, N),

	getp(H2, B, N, TB, H1),	% place from screen
	movecurs(B, TB, N),
	diskplace(on, N),

	waituser.

init(N, L, p(L,[],[])) :-	% setup initial data
	range(1, N, L),			% build the counter list
	cancella_schermo,		% display left pole
	showpole(l, L),
	waituser.

range(N, N, [N]) :- !.
range(N, M, [N | Ns]) :-
	N1 is  N + 1,
	!, range(N1, M, Ns).

polerow(20).
polecol(l, 20).
polecol(c, 40).
polecol(r, 60).

getp(p([H|T],C,R), l, H, T, p(T,C,R)).
getp(p(L,[H|T],R), c, H, T, p(L,T,R)).
getp(p(L,C,[H|T]), r, H, T, p(L,C,T)).

showpole(P, [N|T]) :-
	movecurs(P, T, N),
	diskplace(on, N),
	!, showpole(P, T).
showpole(_, []).

movecurs(P, T, N) :-
	polerow(R),
	polecol(P, C),
	movecurs(T, N, R, C).
movecurs([], N, R, C) :-
	CC is  C - N,
	pos_cursore(R, CC).
movecurs([_|T], N, R, C) :-
	R1 is  R - 1,
	!, movecurs(T, N, R1, C).

diskplace(on, N) :-
	DD is  N * 2,
	putn(220, DD).
diskplace(off, N) :-
	DD is  N * 2,
	putn(32, DD).

waituser :-
	pos_cursore(23, 1),
	showstatus(base).
	%getch(_).				% wait user...
