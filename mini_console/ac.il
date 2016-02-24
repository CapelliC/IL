:- op(100, xf, #).

semitono(do, do#).
semitono(do#, re).
semitono(re, re#).
semitono(re#, mi).
semitono(mi, fa).
semitono(fa, fa#).
semitono(fa#, sol).
semitono(sol, sol#).
semitono(sol#, la).
semitono(la, la#).
semitono(la#, si).
semitono(si, do).

distanza(X, X, 0) :- !.
distanza(X, Y, Z) :-
	semitono(K, Y),
	distanza(X, K, M),
	Z is  M + 1.

terza_min(X, Y) :- distanza(X, Y, 3).
terza_mag(X, Y) :- distanza(X, Y, 4).
quinta_min(X, Y) :- distanza(X, Y, 6).
quinta_mag(X, Y) :- distanza(X, Y, 7).
quinta_ecc(X, Y) :- distanza(X, Y, 8).
sesta_mag(X, Y) :- distanza(X, Y, 9).
settima_min(X, Y) :- distanza(X, Y, 10).
settima_mag(X, Y) :- distanza(X, Y, 11).
nona_mag(X, Y) :- distanza(X, Y, 1).
nona_mag(X, Y) :- distanza(X, Y, 2).
nona_ecc(X, Y) :- distanza(X, Y, 3).

acc_mag(X, [X, Y, Z]) :-
	terza_mag(X, Y), quinta_mag(X, Z).

acc_min(X, [X, Y, Z]) :-
	terza_min(X, Y), quinta_mag(X, Z).

acc_qui_ecc(X, [X, Y, Z]) :-
	terza_mag(X, Y), quinta_ecc(X, Z).

acc_qui_min(X, [X, Y, Z]) :-
	terza_mag(X, Y), quinta_min(X, Z).

acc_mag_ses(X, [X, Y, Z, K]) :-
	terza_mag(X, Y), quinta_mag(X, Z), sesta_mag(X, K).

acc_min_ses(X, [X, Y, Z, K]) :-
	terza_min(X, Y), quinta_mag(X, Z), sesta_mag(X, K).

acc_set_dom(X, [X, Y, Z, K]) :-
	terza_mag(X, Y), quinta_mag(X, Z), settima_min(X, K).

acc_set_ecc(X, [X, Y, Z, K]) :-
	terza_mag(X, Y), quinta_mag(X, Z), settima_mag(X, K).

acc_min_set(X, [X, Y, Z, K]) :-
	terza_min(X, Y), quinta_mag(X, Z), settima_min(X, K).

acc_set_dim(X, [X, A, B, C]) :-
	terza_mag(X, Y), semitono(A, Y),
	quinta_mag(X, Z), semitono(B, Z),
	settima_min(X, K), semitono(C, K).
