:-  i_begin(between),
    i_export(between).

between(N, M, K) :- N < M, K = N.
between(N, M, K) :- N == M, !, K = N.
between(N, M, K) :- N < M, N1 is N+1, between(N1, M, K).

:-  i_end(between).
:-  i_inherit(between).

t(N,L) :- findall(X,between(1,N,X),L).
/* ?-t(681,X). */
