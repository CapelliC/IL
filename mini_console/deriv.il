:- op(10, yfx, ^).

derivata(X, X, 1) :- !.
derivata(C, _X, 0) :-
	atomic(C), !.
derivata(0-U, X, 0-A) :-
	derivata(U, X, A).
derivata(U+V, X, A+B) :-
	derivata(U, X, A),
	derivata(V, X, B).
derivata(U-V, X, A-B) :-
	derivata(U, X, A),
	derivata(V, X, B).
derivata(C*U, X, C*A) :-
	atomic(C), C \= X,
	derivata(U, X, A), !.
derivata(U*V, X, B*U+A*V) :-
	derivata(U, X, A),
	derivata(V, X, B).
derivata(U/V, X, A) :-
	derivata(U*V^(0-1), X, A).
derivata(U^C, X, C*U^(C-1)*W) :-
	atomic(C),
	C \= X,
	derivata(U, X, W).
derivata(log(U), X, A * U ^ (0-1)) :-
	derivata(U, X, A).
derivata(sin(X), X, cos(X)).
derivata(cos(X), X, -sin(X)).

