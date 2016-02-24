/*
:-tracemode([on,file/sch]).
:-translate(( a --> b, {(x,y,z)}, c), X), write(X), nl.
*/

tgf(FileSrc, FileDst) :-
	translate_grammar_file(FileSrc, FileDst).

translate_grammar_file(FileSrc, FileDst) :-

	see(FileSrc),
	read(Rule),
	read_rules(Rule, Clauses),
	seen,

	tell(FileDst),
	write_clauses(Clauses),
	told.

read_rules('$end_of_file', []).
read_rules(Rule, [RuleT|Rules]) :-
	translate_rule(Rule, RuleT),
	read(Rule1),
	!, read_rules(Rule1, Rules).

translate_rule(R, C) :-
	translate(R, C), !.
translate_rule(C, C) :-
	use_rule(C).
use_rule(:-Q) :- call(Q).
use_rule(_).

write_clauses([C|Cs]) :-
	write(C), write('.\n'),
	!, write_clauses(Cs).
write_clauses([]).


write_tree(Lev, Struct) :-
	Struct =.. [Funct|Args],
	tab(Lev), write(Funct),
	check_args(Lev, Args), nl.

check_args(_Lev, []).
check_args(Lev, Args) :-
	write('(\n'),
	Lev1 is Lev+4,
	write_args(Lev1, Args),
	tab(Lev), write(')').

write_args(_Lev, []).
write_args(Lev, [A|As]) :-
	write_tree(Lev, A),
	!, write_args(Lev, As).
