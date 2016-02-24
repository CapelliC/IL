/*
	Keyboard driver
	Translate characters to symbols
*/
:-	i_begin(keyboard_driver),
	i_export([
		read_key,
		undo_key
	]).

// first check for undone
read_key(Key) :-
	retract(ukey(Key)), !.

// get from keyboard
read_key(Key) :-
	getch(C),
	( C = 0, !,
		getch(C1), extended(C1, Key)
	;
		shortcode(C, Key)
	), !.

// store key as undone
undo_key(Key) :-
	asserta(ukey(Key)).

/****** LOCALs *******/

shortcode(27, esc).
shortcode(13, enter).
shortcode(9, tab).
shortcode(8, backspace).
shortcode(N, ctrl(N)) :-
	N >= 1, N =< 26, !.
shortcode(N, eof) :-
	N := 0 - 1.
shortcode(N, ascii(N)) :-
	N >= 32, N < 128.
shortcode(N, short(N)).

extended(72, up).
extended(80, down).
extended(75, left).
extended(77, right).

extended(71, home).
extended(79, end).
extended(82, ins).
extended(83, del).
extended(73, pgup).
extended(81, pgdown).
extended(15, backtab).

extended(119, ctrl(home)).
extended(117, ctrl(end)).
extended(146, ctrl(ins)).
extended(147, ctrl(del)).
extended(132, ctrl(pgup)).
extended(118, ctrl(pgdown)).

extended(141, ctrl(up)).
extended(145, ctrl(down)).
extended(115, ctrl(left)).
extended(116, ctrl(right)).

extended(30, alt(a)).
extended(48, alt(b)).
extended(46, alt(c)).
extended(32, alt(d)).
extended(18, alt(e)).
extended(33, alt(f)).
extended(34, alt(g)).
extended(35, alt(h)).
extended(23, alt(i)).
extended(36, alt(j)).
extended(37, alt(k)).
extended(38, alt(l)).
extended(50, alt(m)).
extended(49, alt(n)).
extended(24, alt(o)).
extended(25, alt(p)).
extended(16, alt(q)).
extended(19, alt(r)).
extended(31, alt(s)).
extended(20, alt(t)).
extended(22, alt(u)).
extended(47, alt(v)).
extended(17, alt(w)).
extended(45, alt(x)).
extended(21, alt(y)).
extended(44, alt(z)).

extended(129, alt(0)).
extended(120, alt(1)).
extended(121, alt(2)).
extended(122, alt(3)).
extended(123, alt(4)).
extended(124, alt(5)).
extended(125, alt(6)).
extended(126, alt(7)).
extended(127, alt(8)).
extended(128, alt(9)).

extended(N, f(Fn)) :-
	N >= 59, N =< 68, !, Fn := N - 58.
extended(N, ctrl(f(Fn))) :-
	N >= 94, N =< 103, Fn := N - 93.
extended(N, alt(f(Fn))) :-
	N >= 104, N =< 113, Fn := N - 103.

extended(C, ext(C)).

:-	i_end(keyboard_driver).
