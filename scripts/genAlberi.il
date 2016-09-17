/*
	generazione alberi VRML
*/
:-	consult(listproc),
	consult(vectuty),
	consult(vrmluty),
	i_inherit(vector_utility),
	i_inherit(vrml_utility).

/*
	una 'corona' di foglie immette dei fattori casuali per simulare
	la distribuzione naturale
*/
corona(NFoglie, Raggio, group(LFoglie)) :-
	corona(0, NFoglie, Raggio, LFoglie).

corona(N, Max, Raggio, [separator([
		rotation(0,1,0, A), translation(TX, TY, Raggio), use(foglia)])|Cs]) :-
	N < Max,
	A is ((3.141593 * 2) / Max) * N,
	casuale(4, Raggio, TX),
	casuale(4, Raggio, TY),
	N1 is N + 1,
	!, corona(N1, Max, Raggio, Cs).
corona(Max, Max, _Raggio, []).

casuale(N, V, C) :-	random(0, N, T), C is (V * (T - N / 2)) / 10.

/*
	lista di tutte le corone, separate sulla verticale
*/
lista_corone([], []).
lista_corone([(Foglie, Diam, DistCorona)|Cs], [translation(0,DistCorona,0),C|Corone]) :-
	corona(Foglie, Diam, C),
	!, lista_corone(Cs, Corone).

/*
	definizione parametri dimensione delle 'corone'
*/
corone_a_cono(N, DistCorona, Foglie, DeltaFoglie, Diam, DeltaDiam,
		[(Foglie, Diam, DistCorona)|Rs]) :-
	N > 0,
	Foglie1 is Foglie - DeltaFoglie,
	Diam1 is Diam - DeltaDiam,
	M is N - 1,
	!, corone_a_cono(M, DistCorona, Foglie1, DeltaFoglie, Diam1, DeltaDiam, Rs).
corone_a_cono(0, _DistCorona, _Foglie, _DeltaFoglie, _Diam, _DeltaDiam, []).

/*
	calcola i parametri per ogni 'corona' di una chioma a sfera
*/
corone_a_sfera(NumCorone, FoglieEquatore, Raggio, ListaRaggi) :-
	AngoloDiv is 3.141563 / (NumCorone + 1),
	corone_a_sfera(0.0, AngoloDiv, FoglieEquatore, Raggio, ListaRaggi).

corone_a_sfera(AngoloCorr, AngoloDiv, FoglieEq, Raggio,
		[(Foglie, RaggioCorona, DistCorona)|ListaRaggi]) :-
	AngoloSucc is AngoloCorr + AngoloDiv,
	AngoloSucc < 3.141563,

	sin(3.141563/2 - AngoloSucc, SinS),
	sin(3.141563/2 - AngoloCorr, SinC),
	fabs(SinS-SinC, AbSin),
	cos(3.141563/2 - AngoloSucc, Cos),

	FoglieF is FoglieEq * Cos, cast(FoglieF, integer, Foglie),
	RaggioCorona is Raggio * Cos,
	DistCorona is Raggio * AbSin,

	!, corone_a_sfera(AngoloSucc, AngoloDiv, FoglieEq, Raggio, ListaRaggi).
corone_a_sfera(_AngoloCurr, _AngoloDiv, _FoglieEq, _Raggio, []).

dimensioni(4.0, 0.8, 0.2).

foglia(def(foglia, separator([
	rotation(1,0,0, -0.2),
	literal(['WWWInline { name "foglia1.wrl" }'])
]))).
corteccia(separator([
	def(corteccia, texture('h:/vrml1/release/wall.bmp'))
])).

genera_sfera(separator([
	Foglia,
	Corteccia,
	separator([
		use(corteccia),
		translation(0, AltAlbero/2, 0),
		cone(DiamTronco, AltAlbero)
	]),translation(0,AltPrimaCorona,0)|ListaCorone
])) :-
	foglia(Foglia),
	corteccia(Corteccia),
	dimensioni(AltAlbero, AltPrimaCorona, DiamTronco),
	Raggio is (AltAlbero-AltPrimaCorona)/2,
	corone_a_sfera(20, 150, Raggio, ListaRaggi),
	lista_corone(ListaRaggi, ListaCorone).

genera_cono(separator([
	Foglia,
	Corteccia,
	separator([
		use(corteccia),
		translation(0, AltAlbero/2, 0),
		cone(DiamTronco, AltAlbero)
	]),translation(0,AltPrimaCorona,0)|ListaCorone
])) :-
	foglia(Foglia),
	corteccia(Corteccia),
	dimensioni(AltAlbero, AltPrimaCorona, DiamTronco),
	NCorone = 20,
	DistCorona is (AltAlbero-AltPrimaCorona)/NCorone,
	corone_a_cono(NCorone, DistCorona, 200, 9, 2.0, 0.1, ListaRaggi),
	lista_corone(ListaRaggi, ListaCorone).

genera_cono_rami(separator([
	Foglia,
	Corteccia,
	separator([
		use(corteccia),
		Albero
	]),translation(0,AltPrimaCorona,0)|ListaCorone
])) :-
	foglia(Foglia),
	corteccia(Corteccia),
	dimensioni(AltAlbero, AltPrimaCorona, DiamTronco),
	NCorone = 20,
	DistCorona is (AltAlbero-AltPrimaCorona)/NCorone,
	corone_a_cono(NCorone, DistCorona, 200, 9, 2.0, 0.06, ListaRaggi),
	lista_corone(ListaRaggi, ListaCorone),
	genera_rami(2, cono, AltAlbero/2, DiamTronco, Albero).

genera_sfera_rami(separator([
	Foglia,
	Corteccia,
	separator([
		use(corteccia),
		Albero
	]),
	translation(0,AltPrimaCorona/2,0)
	|ListaCorone
])) :-
	foglia(Foglia),
	corteccia(Corteccia),
	dimensioni(AltAlbero, AltPrimaCorona, DiamTronco),
	Raggio is (AltAlbero-AltPrimaCorona)/2,
	corone_a_sfera(25, 160, Raggio, ListaRaggi),
	lista_corone(ListaRaggi, ListaCorone),
	genera_rami(2, sfera, AltAlbero/2, DiamTronco, Albero).

genera_ramificato(separator([
	Foglia,
	Corteccia,
	use(corteccia),
	Albero
])) :-
	foglia(Foglia),
	corteccia(Corteccia),
	genera_rami(2, sfera, 2.0, 0.2, Albero).

forma_rami(sfera, Lm, [
	(+Lm,+Lm,+Lm),
	(+Lm,+Lm,-Lm),
	(+Lm,-Lm,+Lm),
	(+Lm,-Lm,-Lm),
	(-Lm,+Lm,+Lm),
	(-Lm,+Lm,-Lm),
	(-Lm,-Lm,+Lm),
	(-Lm,-Lm,-Lm)
]).
forma_rami(cono, Lm, [
	(+Lh,+Lh,+Lh),
	(+Lh,+Lh,-Lh),
	(+Lm,-Lm,+Lm),
	(+Lm,-Lm,-Lm),
	(-Lh,+Lh,+Lh),
	(-Lh,+Lh,-Lh),
	(-Lm,-Lm,+Lm),
	(-Lm,-Lm,-Lm)
]) :- Lh is Lm / 2.

genera_rami(Livello, Forma, Lung, Raggio, separator([
		translation(0, Lung/2, 0),
		cylinder(Raggio, Lung),
		translation(0, Lung/2, 0)|Rami
])) :-
	Livello > 0,
	Livello1 is Livello - 1,
	Raggio1 is Raggio / 2, % * 2.0/3.0,
	Lm is Lung * 6.0 / 11.0,
	forma_rami(Forma, Lm, Pattern),
	filtra_delta(Pattern, SottoInsieme),
	genera_sottorami(Livello1, Forma, Lm, Raggio1, SottoInsieme, Rami).
genera_rami(0, _Forma, Lung, Raggio, separator([
		translation(0, Lung/2, 0),
		cylinder(Raggio, Lung)
])).

% come sopra, ma fa un 'ciuffo' di foglie al termine del ramo
genera_rami_con_foglie(0, Lung, Raggio, separator([
		translation(0, Lung/2, 0),
		cylinder(Raggio, Lung),
		translation(0, Lung/2, 0),
		texture(''),
		group(ListaCorone)
])) :-
	RaggioC is Lung * 0.8,
	corone_a_sfera(5, 20, RaggioC, ListaRaggi),
	lista_corone(ListaRaggi, ListaCorone).
	
genera_sottorami(Livello, Forma, Lung, Raggio, [Diff|Deltas],
		[separator([rotation(Xr,Yr,Zr,A),Ramo])|Rami]) :-
	vectLen(Diff, Dist),
	vectDiv(Diff, Dist, UnitDiff),
	YAxis = (0.0, 1.0, 0.0),
	vectProd(YAxis, Diff, (Xr, Yr, Zr)),
	vectScalar(YAxis, UnitDiff, CosAngle),
	acos(CosAngle, A),

%	random(10, 20, R),
%	Lr is (Lung * R) / 15,
	genera_rami(Livello, Forma, /*Lr*/Lung, Raggio, Ramo),
	!, genera_sottorami(Livello, Forma, Lung, Raggio, Deltas, Rami).
genera_sottorami(_Livello, _Forma, _Lung, _Raggio, [], []).

filtra_delta([_|Ds], Zs) :-
	random(0,5,X), X >= 4, !, filtra_delta(Ds, Zs).
filtra_delta([D|Ds], [D|Zs]) :-
	!, filtra_delta(Ds, Zs).
filtra_delta([], []).

t :- genera_cono_rami(Scene),
	File = 'sch.wrl',
	 write('scrittura del file':File),nl,
	fmtfile(Scene, File),
	 showstatus(base).
