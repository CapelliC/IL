:- consult(linee).

caratteri_bordo(11, [218, 196, 191, 179, 217, 196, 192, 179]).
caratteri_bordo(22, [201, 205, 187, 186, 188, 205, 200, 186]).
caratteri_bordo(12, [214, 196, 183, 186, 189, 196, 211, 186]).
caratteri_bordo(21, [213, 205, 184, 179, 190, 205, 212, 179]).

bordo(ASX-ASY, BDX-BDY, TipoL) :-
	caratteri_bordo(TipoL, [AAS, LOA, AAD, LVD, ABD, LOB, ABS, LVS]),
	linea_bordo(ASX-ASY, BDX, AAS, LOA, AAD),
	linea_bordo(ASX-BDY, BDX, ABS, LOB, ABD),
	Yi is ASY + 1,
	linea_vert(ASX-Yi, BDY, LVS),
	linea_vert(BDX-Yi, BDY, LVD).

linea_bordo(XL-Y, XH, CS, CL, CD) :-
	pos_cursore(Y, XL),
	put(CS),
	NC is XH-XL-1,
	putn(CL, NC),
	put(CD).
