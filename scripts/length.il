:-  i_begin(length),
    i_export(length).

length([],0).
length([_|T],L) :- var(L), length(T,L1), L is L1+1.
length([_|T],L) :- nonvar(L), L > 0, L1 is L-1, length(T,L1).

:-  i_end(length).
:-  i_inherit(length).
