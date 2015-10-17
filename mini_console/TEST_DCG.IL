/*
	test grammar rules
*/
sentence(sentence(NP, VP), X) --> noun_phrase(NP, X), verb_phrase(VP, X).

noun_phrase(noun_phrase(D, N), X) --> determiner(D, X), noun(N, X).

verb_phrase(verb_phrase(V), X) --> verb(V, X).
verb_phrase(verb_phrase(V, NP), X) --> verb(V, X), noun_phrase(NP, X).

noun(noun(boy), singular) --> [boy].
noun(noun(boys), plural) --> [boys].
noun(noun(man), singular) --> [man].
noun(noun(men), plural) --> [men].
noun(noun(apple), singular) --> [apple].
noun(noun(apples), plural) --> [apples].

determiner(determiner(the), _) --> [the].
determiner(determiner(a), singular) --> [a].
determiner(determiner(some), plural) --> [some].

verb(verb(eats), singular) --> [eats].
verb(verb(eat), plural) --> [eat].
verb(verb(sings), singular) --> [sings].
verb(verb(sings), plural) --> [sing].
