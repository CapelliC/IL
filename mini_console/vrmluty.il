/*
	VRML formatting utilities

	Ing. Capelli Carlo
	HOME 1996 - VRML Project
*/

:-	i_begin(vrml_utility),
	i_export([
		fmtfile,
		fmtscene,
		connect_points,
		make_server,
		disp_server/2,
		chkcall,
		chkcalls
	]),
	i_inherit([
		list_processing,
		vector_utility
	]).

% static scene display text
%	build a memory file (could as well write to file or stdout...)
%
fmtscene(Deep, Scene, Text) :-
	set_buffer(#), tell(#),
	fmtobject(Deep, Scene),
	told,
	get_buffer(#, Text).
fmtfile(Scene, FileName) :-
	tell(FileName),
	write('#VRML V1.0 ascii'), nl,
	fmtobject(0, Scene), nl,
	told.

connect_points(P1, P2, Radius, separator([
	translation(Xm, Ym, Zm),
	rotation(Xr, Yr, Zr, A),
	cylinder(Radius, Dist)
])) :-
	vectAdd(P1, P2, PSum),
	vectDiv(PSum, 2.0, (Xm, Ym, Zm)),

	vectSub(P2, P1, Diff),

	vectLen(Diff, Dist),
	vectDiv(Diff, Dist, UnitDiff),

	YAxis = (0.0, 1.0, 0.0),
	vectProd(YAxis, Diff, (Xr, Yr, Zr)),

	vectScalar(YAxis, UnitDiff, CosAngle),
	acos(CosAngle, A).

% attempt to create a VRML Browsing server
%
make_server(Doc, Curs) :-

	% place in remote server object
	oa_create('Br3dvrml.Document', Serv),

	chkcalls(Serv, [
		openFile(Doc),
		openEditor,
		showBrowser,
		openCursor=object(Curs)
	]),

	assert(disp_server(Serv, Curs)).

% issue a call sequence
%
chkcalls(_Obj, []).
chkcalls(Obj, [Call|Calls]) :- chkcall(Obj, Call), !, chkcalls(Obj, Calls).

% issue a call (but filter and compute arithmetic expressions)
%
chkcall(_Obj, V := E) :- V := E.
chkcall(Obj, Call) :-
	( Call = (Func = Rc) ; Func = Call, Rc = true ),
	( oa_call(Obj, Func = Rc) ; messageBox(failed(oa_call(Obj, Call)), 1, Ok), Ok = 1 ).

indent(Deep) :- write('\n'), putn("\t", Deep).

% object formatting: grouping
%
fmtobject(Deep, group(Parts)) :- fmtgroup(Deep, 'Group { ', Parts).
fmtobject(Deep, separator(Parts)) :- fmtgroup(Deep, 'Separator { ', Parts).
fmtobject(Deep, transformSeparator(Parts)) :- fmtgroup(Deep, 'TransformSeparator { ', Parts).
fmtobject(Deep, wwwAnchor(Name, Description, Parts)) :-
	indent(Deep),
	writes(['WWWAnchor { name "', Name, '" description "', Description, '"']),
	Deep1 := Deep + 1,
	findall(Obj, (member(Obj, Parts), writepart(Deep1, Obj)), _),
	indent(Deep),
	write('}').

fmtgroup(Deep, NodeHdr, Parts) :-
% showstatus(base),nl,
	indent(Deep),
	writes(NodeHdr),
	Deep1 := Deep + 1,
	findall(Obj, (member(Obj, Parts), writepart(Deep1, Obj)), _),
	indent(Deep),
	write('}').

writepart(Deep, Obj) :- fmtobject(Deep, Obj), !.

% object formatting: basic transforms
% 
fmtobject(Deep, translation(X,Y,Z)) :-
	indent(Deep),
	writes(['Translation { translation ', X, ' ', Y, ' ', Z, ' }']).
fmtobject(Deep, rotation(X,Y,Z,A)) :-
	indent(Deep),
	writes(['Rotation { rotation ', X, ' ', Y, ' ', Z, ' ', A, ' }']).
fmtobject(Deep, scale(X,Y,Z)) :-
	indent(Deep),
	writes(['Scale { scaleFactor ', X, ' ', Y, ' ', Z, ' }']).
fmtobject(Deep, coords(Coords)) :-
	indent(Deep),
	write('Coordinate3 { point ['),
	Deep1 := Deep + 1,
	fmtpoints(Deep1, Coords),
	indent(Deep),
	write(']}').
fmtobject(Deep, lines(Ind)) :-
	indent(Deep),
	write('IndexedLineSet { coordIndex ['),
	Deep1 := Deep + 1,
	fmtidxlist(Deep1, Ind),
	indent(Deep),
	write(']}').
fmtobject(Deep, faces(Ind)) :-
	indent(Deep),
	write('IndexedFaceSet { coordIndex ['),
	Deep1 := Deep + 1,
	fmtidxlist(Deep1, Ind),
	indent(Deep),
	write(']}').
fmtobject(Deep, texture(File)) :-
	indent(Deep),
	writes(['Texture2 { filename "', File, '"}']).

fmtobject(_Deep, include_file(Name)) :- include_file(Name).

% object formatting: object primitives
%
fmtobject(Deep, cylinder(R, H)) :-
	indent(Deep),
	writes(['Cylinder { radius ', R, ' height ', H, ' }']).
fmtobject(Deep, cone(R, H)) :-
	indent(Deep),
	writes(['Cone { bottomRadius ', R, ' height ', H, ' }']).
fmtobject(Deep, cube(W, H, D)) :-
	indent(Deep),
	writes(['Cube { width ', W, ' height ', H, ' depth ', D, ' }']).
fmtobject(Deep, sphere(R)) :-
	indent(Deep),
	writes(['Sphere { radius ', R, ' }']).
fmtobject(Deep, text(Spec)) :- Spec = [_|_],
	indent(Deep), write('AsciiText { '),
	fmtparams([], Spec),
	write('}').
fmtobject(Deep, text(String)) :-
	indent(Deep),
	writes(['AsciiText { string "', String, '" }']).
fmtobject(Deep, font(Size, Family, Style)) :-
	indent(Deep),
	writes(['FontStyle { size ', Size, ' family ', Family, ' style ', Style, ' }']).

fmtobject(Deep, material(Mats)) :-
	indent(Deep), write('Material { '),
	fmtparams([
		ambient=ambientColor,
		diffuse=diffuseColor,
		specular=specularColor,
		emissive=emissiveColor
	], Mats),
	write('}').

fmtobject(Deep, use(Label)) :-
	indent(Deep),
	writes(['USE ', Label]).
fmtobject(Deep, def(Label, Object)) :-
	indent(Deep),
	writes(['DEF ', Label]), fmtobject(Deep, Object).

fmtobject(_Deep, literal([])).
fmtobject(Deep, literal([L|Ls])) :-
	indent(Deep),
	writes(L), !, fmtobject(Deep, literal(Ls)).

fmtobject(Deep, Obj) :-
	write('failed to format:'(Deep, Obj)),nl.

% formatting support code
%
fmtpoint((X,Y,Z)) :-
	writes([X, ' ', Y, ' ', Z]).
fmtpoints(_Deep, []).
fmtpoints(Deep, [P1,P2,P3,P4|Ps]) :-
	indent(Deep),
	fmtpoint(P1), write(','),
	fmtpoint(P2), write(','),
	fmtpoint(P3), write(','),
	fmtpoint(P4), write(','),
	!, fmtpoints(Deep, Ps).
fmtpoints(Deep, [P|Ps]) :-
	indent(Deep),
	fmtpoint(P), write(','),
	!, fmtpoints(Deep, Ps).

fmtidxlist(_Deep, []).
fmtidxlist(Deep, [I]) :-
	indent(Deep), fmtindex(I).
fmtidxlist(Deep, [I1,I2,I3,I4|Is]) :-
	indent(Deep),
	fmtindex(I1), write(','),
	fmtindex(I2), write(','),
	fmtindex(I3), write(','),
	fmtindex(I4), write(','),
	!, fmtidxlist(Deep, Is).
fmtidxlist(Deep, [I|Is]) :-
	indent(Deep),
	fmtindex(I), write(','), !, fmtidxlist(Deep, Is).

fmtindex([]) :- write('-1').
fmtindex([I|Is]) :- writes([I, ',']), !, fmtindex(Is).

% qualified parameters formatting
%
fmtparams(Keys, [P|Ps]) :-
	P =.. [F|A],
	( member(F = T, Keys) ; T = F ),
	writeln([T, ' ']), fmtpvals(A),
	!, fmtparams(Keys, Ps).
fmtparams(_Keys, []).

fmtpvals([]).
fmtpvals([V|Vs]) :- writes([V, ' ']), !, fmtpvals(Vs).

% output with expression value computing
%
writes([E|Es]) :- writev(E), !, writes(Es).
writes([]).
writes(E) :- writev(E).

writev(V) :- ( atomic(V), !, write(V) ; R := V, write(R) ).

% include a file by physical copy
%
include_file(Name) :-
	see(Name),
	repeat, get(C), (C := -1; put(C), fail),
	seen.

:-	i_end(vrml_utility).
