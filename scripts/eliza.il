%-------------------------------------------------
% eliza:
%	simulazione di colloquio con stimolo/risposta
%	da Weizenbaum
%
%	Sterling and Shapiro: the art of prolog
%-------------------------------------------------

:-	consult(listproc), i_inherit(list_processing),
	consult(rwl).

eliza :-
	write('? '), read_word_list(Input), eliza(Input), !.

eliza([bye]) :-
	write('Goodbye. I hope I have helped you'), nl.
eliza(Input) :-
	pattern(Stimulus, Response),
	match(Stimulus, Dictionary, Input),
	match(Response, Dictionary, Output),
	reply(Output),
	!, eliza.

match([N|Pattern], Dictionary, Target) :-
	integer(N), lookup(N, Dictionary, LeftTarget),
	append(LeftTarget, RightTarget, Target),
	match(Pattern, Dictionary, RightTarget).
match([Word | Pattern], Dictionary, [Word | Target]) :-
	atom(Word), match(Pattern, Dictionary, Target).
match([], _Dictionary, []).

pattern([i,am,1],[how,long,have,you,been,1,'?']).
pattern([1,you,2,me],[what,makes,you,think,i,2,you,'?']).
pattern([i,like,1],[does,anyone,else,in,your,family,like,1,'?']).
pattern([i,feel,1],[do,you,often,feel,that,way,'?']).
pattern([1,X,2],[can,you,tell,me,more,about,your,X,'?']) :- important(X).
pattern([1],[please,go,on]).

important(father).
important(mother).
important(son).
important(sister).
important(brother).
important(daughter).

reply([Head | Tail]) :-
	write(Head), put(" "), reply(Tail).
reply([]) :- nl.

lookup(Key, [(Key, Value) | _Dict], Value).
lookup(Key, [(Key1, _Val1) | Dictionary], Value) :-
	Key \= Key1, lookup(Key, Dictionary, Value).
