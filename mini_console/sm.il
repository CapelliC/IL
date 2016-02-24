:-	consult('listproc.il'), i_inherit(list_processing).

mkall(Cmd) :-
	os_command('cd c:\\cardll'), call(Cmd),
	os_command('cd c:\\bridgest'), call(Cmd),
	os_command('cd c:\\il\\iafx'), call(Cmd),
	os_command('cd c:\\il\\ilde'), call(Cmd).

/*
	change OBJs files location
*/
d2r :- movefiles(debug, release).
d2rd :- movefiles(debug, reldebug).
r2d :- movefiles(release, debug).
r2rd :- movefiles(release, reldebug).
rd2d :- movefiles(reldebug, debug).
rd2r :- movefiles(reldebug, release).

location(debug, "objdeb").
location(release, "objrel").
location(reldebug, "objreld").

movefiles(SymPutDir, SymGetDir) :-

//	checkdirs(SymPutDir),

	location(SymPutDir, PutDir),
	append("move *.obj,*.pch,*.pdb,*.map,*.res,*.sbr,*.bsc,*.dll,*.exe,*.lib,*.def ",
			PutDir, StrCmd1),
	name(Cmd1, StrCmd1),
	try_cmd(Cmd1),

	location(SymGetDir, GetDir),
	append("move ", GetDir, S1),
	append(S1, "\\*.* .", StrCmd2),
	name(Cmd2, StrCmd2),
	try_cmd(Cmd2).

try_cmd(Cmd) :-
	write(Cmd), nl,
	os_command(Cmd), !.
try_cmd(Cmd) :-
	write('OS command failed: '), write(Cmd), nl.

checkdirs :-
	listnamedirs(AllDirs, ""),
	checkexist(AllDirs).

checkdirs(MustBeEmpty) :-
	checkdirs,
	location(MustBeEmpty, DirString),
	append(DirString, "\\*.*", S1),
	name(L1, S1),
	listfiles(L1, [], Files),
	!, checkempty(Files).

checkexist([]).
checkexist([Dir|Dirs]) :-
	listfiles(Dir, subdir, [_DIR]), !, checkexist(Dirs).
checkexist([Dir|_Dirs]) :-
	write('directory not found: '), write(Dir), nl.

listnamedirs(AllDir, Ext) :-
	findall(DirSym, (
		location(_, DirStr),
		append(DirStr, Ext, DirStrExt),
		name(DirSym, DirStrExt)
	), AllDir).

findempty([], none).
findempty([Dir|_], Dir) :-
	listfiles(Dir, [], []).
findempty([_Dir|Dirs], Dir) :- !, findempty(Dirs, Dir).

checkempty([]) :- !.
checkempty(_) :-
	write('target directory not empty!\n'),
	fail.

showempty(none) :- write('no empty directory!'), nl.
showempty(Dir) :- write('empty directory:'), write(Dir), nl.

findcurr :-
	listnamedirs(AllDir, "\\*.*"),
	findempty(AllDir, Dir),
	showempty(Dir).

:- checkdirs, findcurr.

/*
	make a global ZIP file with all sources
*/
zipsrc :- !,
	default(target, Dst), zipsrc(Dst).
zipsrc(Dst) :- !,
	default(command, Cmd), zipsrc(Dst, Cmd).
zipsrc(Dst, Cmd) :- !,
	default(projects, Dirs), zipsrc(Dst, Cmd, Dirs).
zipsrc(Dst, Cmd, Dirs) :- !,
	default(extensions, Exts), zipsrc(Dst, Cmd, Dirs, Exts).
zipsrc(Dst, Cmd, Dirs, Exts) :-
	append(Cmd, Dst, CmdStr),
	catlist(Exts, CmdStr, ZipStr),
	name(ZipCmd, ZipStr),
	scandir(Dirs, ZipCmd).

scandir([], _Exts).
scandir([Dir|Dirs], Exts) :-
	append("cd c:\\", Dir, Str0),
	name(CmdCD, Str0),
	try_cmd(CmdCD),
	try_cmd(Exts),
	!, scandir(Dirs, Exts).

catlist([], L, L).
catlist([S|Ss], A0, R) :-
	append(A0, S, A1),
	!, catlist(Ss, A1, R).

default(command, "pkzip -arp ").
default(target, "c:\\zipall").
default(projects, [
	"il",
	"devil",
	"bridge",
	"app"
]).
default(extensions, [
	" *. *.cpp *.c *.h *.hpp *.inl *.mak *.rc *.ico *.bmp",
	" *.rtf *.ecl *.clw *.def *.cur *.hpj *.hm *.bat *.txt"
]).

% t :- zipsrc, showstatus(base).
