/*
	sentences to predicate calculus
*/

:- op(100,xfy,&).
:- op(150,xfy,->).

sentence(P) --> noun_phrase(X,P1,P), verb_phrase(X,P1).

noun_phrase(X,P1,P) --> determiner(X,P2,P1,P), noun(X,P3), rel_clause(X,P3,P2).
noun_phrase(X,P,P) --> proper_noun(X).

verb_phrase(X,P) --> trans_verb(X,Y,P1), noun_phrase(Y,P1,P).
verb_phrase(X,P) --> intrans_verb(X,P).

rel_clause(X,P1,(P1&P2)) --> [that], verb_phrase(X, P2).
rel_clause(_, P, P) --> [].

determiner(X,P1,P2, all(X,(P1->P2))) --> [every].
determiner(X,P1,P2, exists(X,(P1&P2))) --> [a].

noun(X, man(X)) --> [man].
noun(X, woman(X)) --> [woman].

proper_noun(john) --> [john].

trans_verb(X,Y,loves(X,Y)) --> [loves].
intrans_verb(X,lives(X)) --> [lives].
