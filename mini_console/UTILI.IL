:-	consult(ansidrv), i_inherit(ansi_driver),
	consult(listproc), i_inherit(list_processing).

cls :-
	sfondo_default,
	cancella_schermo.

z(File) :-
	atom(File), 
	name(File, FileS),
	!, z(FileS).
z(Args) :-
	append("z ", Args, CmdS),
	name(CmdA, CmdS),
	!, os_command(CmdA).

ascii_table :-
	ascii_table(0, 255).
ascii_table(Low, High) :-
	ascii_table(Low, Low, High).
ascii_table(High, Base, High) :-
	show_ascii_char(High, Base), !.
ascii_table(Low, Base, High) :-
	Low < High,
	show_ascii_char(Low, Base),
	Low1 is Low + 1,
	!, ascii_table(Low1, Base, High).

show_ascii_char(Char, Base) :-
	shownum(Char),
	cput(Char),
	mktab(Char, Base).

pfixspace(Num) :- Num < 10, write('  '), !.
pfixspace(Num) :- Num < 100, write(' '), !.
pfixspace(_).

shownum(Num) :-
	pfixspace(Num),
	write(Num),
	put(32).

cput(0) :- write(nul).
cput(7) :- write(bel).
cput(8) :- write('bs ').
cput(9) :- write(tab).
cput(10) :- write('nl ').
cput(13) :- write('cr ').
cput(26) :- write(eof).
cput(27) :- write(esc).
cput(C) :- put(C), put(32), put(32).

mktab(C, B) :- NCOL = 11, NCOL is ((C - B) mod NCOL) + 1, nl.
mktab(_, _).
