/*
	Basic vector operation

	Ing. Capelli Carlo
	HOME 1996 - VRML Project
*/

:-	i_begin(vector_utility),
	i_export([
		vectAdd,
		vectSub,
		vectDiv,
		vectLen,
		vectUnit,
		vectProd,
		vectScalar
	]).

% subtract V2 from V1, yield V3
%
vectSub((X1,Y1,Z1), (X2,Y2,Z2), (X3,Y3,Z3)) :-
	X3 is  X1 - X2, Y3 is  Y1 - Y2, Z3 is  Z1 - Z2.

% subtract N from V1 components, yield V3
%
vectSub((X1,Y1,Z1), N, (X3,Y3,Z3)) :-
	number(N),
	X3 is  X1 - N, Y3 is  Y1 - N, Z3 is  Z1 - N.

% add V2 to V1, yield V3
%
vectAdd((X1,Y1,Z1), (X2,Y2,Z2), (X3,Y3,Z3)) :-
	X3 is  X1 + X2, Y3 is  Y1 + Y2, Z3 is  Z1 + Z2.

% add N to V1 components, yield V3
%
vectAdd((X1,Y1,Z1), N, (X3,Y3,Z3)) :-
	number(N),
	X3 is  X1 + N, Y3 is  Y1 + N, Z3 is  Z1 + N.

vectDiv((X1,Y1,Z1), N, (X3,Y3,Z3)) :-
	X3 is  X1 / N, Y3 is  Y1 / N, Z3 is  Z1 / N.

% compute length
%
vectLen((X,Y,Z), Dist) :-
	DistSquare is  (X*X + Y*Y + Z*Z),
	sqrt(DistSquare, Dist).

% build unit vector
%
vectUnit(V, U) :-
	vectLen(V, L),
	vectDiv(V, L, U).

% vectorial (or cross) product
%
vectProd((PX,PY,PZ), (QX,QY,QZ), (VX,VY,VZ)) :-
	VX is  PY * QZ - PZ * QY,
	VY is  PZ * QX - PX * QZ,
	VZ is  PX * QY - PY * QX.

% scalar (or dot) product
%
vectScalar((X1,Y1,Z1), (X2,Y2,Z2), Scalar) :-
	Scalar is  X1 * X2 + Y1 * Y2 + Z1 * Z2.

:-	i_end(vector_utility).
