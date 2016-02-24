// TBD debugging

read_word_list(Ws) :-
	get0(C),
	read_word_list(C,Ws).
read_word_list(C,[W|Ws]) :-
	word_char(C),
	read_word(C,W,C1),
	read_word_list(C1,Ws).
read_word_list(C,Ws) :-
	fill_char(C),
	get0(C1),
	read_word_list(C1,Ws).
read_word_list(C,[]) :-
	end_of_words_char(C).

read_word(C,W,C1) :-
	word_chars(C,Cs,C1),
	name(W,Cs).

word_chars(C,[C|Cs],C0) :-
	word_char(C),
	!,
	get0(C1),
	word_chars(C1,Cs,C0).
word_chars(C,[],C).

word_char(C) :- 97 =< C, C =< 122 ; 65 =< C, C =< 90.
word_char(95).

fill_char(32).
fill_char(10).
fill_char(44).

end_of_words_char(46).
