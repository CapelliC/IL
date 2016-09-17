%----------------------------------
% towers of hanoi with display
% modified using a list as counter
%----------------------------------

hanoi(N) :-
	initdisplay(N, L),
	move(L, l, c, r),	% algorithmic solution apply
	enddisplay.

%-------------------------------------------
% move N-1 disks using target pole as spare
% move the disk to target
% move N-1 disks using target pole as spare
%
move([_|T], A, B, C) :-
	move(T, A, C, B),
	inform(A, B),
	!,
	move(T, C, B, A).
move([], _, _, _).

%----------------
% displayer code
%----------------

:- consult(ansi).

inform(A, B) :-				% update poles positions
	pop(A, N),				% move top disk from pole A
	push(B, N),				% to pole B
	waituser.

initdisplay(N, L) :-		% setup initial display data
	range(1, N, L),			% build the counter list
	assert(pole(l, L)),
	assert(pole(c, [])),
	assert(pole(r, [])),
	cancella_schermo,		% display left pole
	showpole(l, L),
	waituser.

range(N, N, [N]) :- !.
range(N, M, [N | Ns]) :-
	N1 is N + 1,
	!,
	range(N1, M, Ns).

enddisplay :-
	retract(pole(l, _)),
	retract(pole(c, _)),
	retract(pole(r, _)),
	!.

polerow(20).
polecol(l, 20).
polecol(c, 40).
polecol(r, 60).

pop(P, N) :-					% update pole list and display change
	retract(pole(P, [N|T])),	% get current
	movecurs(P, T, N),			% remove from screen
	diskplace(off, N),
	assert(pole(P, T)).			% save updated

push(P, N) :-					% update pole list and display change
	retract(pole(P, T)),		% get current
	movecurs(P, T, N),			% place from screen
	diskplace(on, N),
	assert(pole(P, [N|T])).		% save updated

showpole(P, [N|T]) :-
	movecurs(P, T, N),
	diskplace(on, N),
	!,
	showpole(P, T).
showpole(_, []).

movecurs(P, T, N) :-
	polerow(R),
	polecol(P, C),
	movecurs(T, N, R, C).
movecurs([], N, R, C) :-
	CC is C - N,
	pos_cursore(R, CC).
movecurs([_|T], N, R, C) :-
	R1 is R - 1,
	!,
	movecurs(T, N, R1, C).

diskplace(on, N) :-
	DD is N * 2,
	putn(220, DD).
diskplace(off, N) :-
	DD is N * 2,
	putn(32, DD).

waituser :-
	pos_cursore(23, 1),
	showstatus([base]).
	%getch(_).				% wait user...
