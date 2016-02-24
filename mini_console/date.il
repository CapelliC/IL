/*
	this program is intended to provide a MEMO for remember	dates
	rewritten for CCProlog
*/

%--------------------
% convert to integer
%
atoi([C], V, 1) :- !,
	V is  C - 48.
atoi([C | Cs], V, F1) :- !,
	atoi(Cs, Vs, F),
	F1 is  F * 10,
	V is  Vs + (C - 48) * F1.
atoi(A, V) :-
	name(A, L),
	atoi(L, V, _).

%-----------------------------------
% get current date Year, Month, Day
%
getdate(Y, M, D) :-
	os_name(dos), !,
	timecurr(TC),
	timeform(TC, '%Y', FY), atoi(FY, Y),
	timeform(TC, '%m', FM), atoi(FM, M),
	timeform(TC, '%d', FD), atoi(FD, D), !.
getdate(Y, M, D) :-
	os_getdate(D, M, Y, _DOW).

%--------------------------------------------------------
% compute difference between current date and event data
%
date_distance(_/_/_, _/0/0, _) :-
	!, fail.
date_distance(_/M/Ga, _/M/Ge, GD) :-
	Ga =< Ge,
	!, GD is  Ge - Ga.
date_distance(_/Ma/Ga, _/Me/Ge, GD) :-
	month(Ma, Gma, Sma, _),
	DisM is  Gma - Ga + Ge,
	sum_days_months(Sma, Me, SDM),
	GD is  DisM + SDM.

%-------------------------------
% write atom spaced as required
%
write_spc(Atom, Len) :-
	write(Atom),
	name(Atom, List),
	listlen(List, LenList),
	(
		Len > LenList,
		!,
		LTab is  Len - LenList,
		tab(LTab)
	;
		true
	).

%---------------------
% compute list lenght
%
listlen([], 0).
listlen([_|L], N) :-
	listlen(L, NL),
	N is  NL + 1.

%-----------------------------------------------------------
% go from month A to month E adding month' days, excluded E
%
sum_days_months(M, M, 0) :- !.
sum_days_months(A, E, V) :-
	month(A, ND, Succ, _),
	sum_days_months(Succ, E, V1),
	V is  V1 + ND.

%-------------------------------------
% month(index, days, following, name)
%
month(1, 31, 2, 'Gennaio').
month(2, 28, 3, 'Febbraio').
month(3, 31, 4, 'Marzo').
month(4, 30, 5, 'Aprile').
month(5, 31, 6, 'Maggio').
month(6, 30, 7, 'Giugno').
month(7, 31, 8, 'Luglio').
month(8, 31, 9, 'Agosto').
month(9, 30, 10,'Settembre').
month(10,31, 11,'Ottobre').
month(11,30, 12,'Novembre').
month(12,31, 1, 'Dicembre').

%------------------
% events data base
%
data_nascita(0000/00/00,id('Abler','Marek')).
data_nascita(1929/01/05,id('Baruzzi','Flora')).
data_nascita(1934/11/05,id('Baruzzi','Rina')).
data_nascita(1925/12/13,id('Baruzzi','Valeria')).
data_nascita(0000/00/00,id('Bortolotti','Giulio')).
data_nascita(1981/08/24,id('Canipari','Loris')).
data_nascita(0000/00/00,id('Canipari','Paolo')).
data_nascita(1985/08/07,id('Capelli','Alessandro')).
data_nascita(1961/06/05,id('Capelli','Carlo')).
data_nascita(1957/03/04,id('Capelli','Dina')).
data_nascita(1953/10/01,id('Capelli','Ferruccio')).
data_nascita(1989/03/31,id('Capelli','Ivan')).
data_nascita(1959/12/26,id('Capelli','Maggiorina')).
data_nascita(1993/02/01,id('Capelli','Stella')).
data_nascita(1964/02/19,id('Cerutti','Antonia')).
data_nascita(1968/04/09,id('Cerutti','Cinzia')).
data_nascita(1986/08/25,id('Cerutti','Elisa')).
data_nascita(1953/02/21,id('Cerutti','Giancarla')).
data_nascita(1955/07/14,id('Cerutti','Giorgio')).
data_nascita(1927/03/30,id('Cerutti','Giovanni Battista')).
data_nascita(1961/04/17,id('Cerutti','Maria Angela')).
data_nascita(1956/08/24,id('Cerutti','Rosalia')).
data_nascita(1963/01/16,id('Favier','Gianfranco')).
data_nascita(1982/02/19,id('Ferrari','Dharma')).
data_nascita(1984/09/29,id('Ferrari','Nisa')).
data_nascita(1989/08/19,id('Ferrari','Tanita')).
data_nascita(1950/09/26,id('Ferrari','Fulvio')).
data_nascita(1920/09/25,id('Ferrari','Spartaco')).
data_nascita(1959/07/18,id('Gabusi','Varo')).
data_nascita(1934/03/30,id('Pellegrini','Teresa')).
data_nascita(1961/11/13,id('Righettini','Stella')).
data_nascita(1989/02/24,id('Signorini','Amira')).
data_nascita(0000/00/00,id('Signorini','Angelo')).
data_nascita(1921/09/20,id('Trivulzi','Enrico')).

data_decesso(1994/09/01,id('Baruzzi','Valeria')).
data_decesso(1995/00/00,id('Ferrari','Spartaco')).
data_decesso(1996/07/01,id('Abler','Marek')).

%--------------------
% calcolo dell' eta'
%
age_of(Nome, Cogn, Eta) :-
	date(Y, _, _),
	data_nascita(YN/_/_, id(Cogn, Nome)),
	Eta is  Y - YN.

%------------------------------
% format birthdate for display
%
show_birthday(ME, DE, Nome, Cogn) :-
	age_of(Nome, Cogn, Eta),
	month(ME, _, _, MName),
	write_spc(Nome, 23),
	writeln([' compie ', Eta, ' anni\til ', DE, ' di ', MName]),
	nl.

%-----------------------------------------
% events table scansion (failure driven loop)
%	check if event date is sufficently near to actual
%
birthdays(LookAHead) :-
	date(Y, M, D),
	data_nascita(YE/ME/DE, id(Cogn, Nome)),
	date_distance(Y/M/D, YE/ME/DE, GDist),
	GDist =< LookAHead,
	not data_decesso(_,id(Cogn,Nome)),
	show_birthday(ME, DE, Nome, Cogn),
	fail.
birthdays(_).

%-----------------
% initialize date
%
initdate :-
	getdate(Y, M, D),
	assert(date(Y, M, D)),
	month(M, _, _, Month),
	writeln(['oggi:\t\t\t', D, ' di ', Month, '\n\n']).

:- initdate, birthdays(45).
