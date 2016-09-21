%
%
%	inriasuite.pl
%
%    Author J.P.E. Hodgson
%	date 9 february 1999
%
%    Version 0.9
%
%   This is to be a batch version of
%   Ali's tests. It will read lines from a file
%   that are in the form [ Goal, Substs].
%
%
%   Modified 1999/02/24 to read several files and
%   report on them one by one. Output results
%   only when the result is not the expected one.
%
%    A more readable output can be obtained if the processor
%   supports numbervars/3 by  restoring the commented out
%   line in  write_if_wrong/5.
%
%    Revised April 8 1999.
%
%   Matching of solutions is not yet  perfected.
%


%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Operators required for the
%  tests.
%


:- op( 20, xfx, <-- ).



%%%%%%%%%%%%%%%%%%%%
%
%  score/3 is dynamic.
% 
%  score(File, total(Tests), wrong(PossibleErrors)
%



:- dynamic(score/3).


%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  dynamic directives needed for the compiled
%  version of the tests.
%

:- dynamic(bar/1).          % for asserta
:- dynamic(foo/1).          % for assertz

%%%%%%%%%%%%%%%%
%
%  run_all_tests/0
%
%   Driver.


run_all_tests :-
	findall(F, file(F), Files),
        test_all(Files),
        write_results, !.


test_all([]).
test_all([F|Fs]) :-
	run_tests(F),
        test_all(Fs).

%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   write_results/0.
%

write_results :-
	findall(F, inerror(F), ErrorBips),
        write('--------------------'), nl,
        ( 
        ErrorBips = []
        ->
        (
        write('All bips passed -------------'), nl
        )
        ;
        (nl, write('The following BIPs gave unexpected answers:'),nl,
         write('The results should be examined carefully.'), nl,
        nl,
        display_list(ErrorBips))
        ).




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
%
%    result(+Goal, -Result)
%
%   evaluates the Goal and gives all the substitutions
%

result(G, Res) :-
      get_all_subs(G, Subs),
      special_ans_forms(Subs,Res).



%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   certain substitutions appear in
%   a simplified form.
%


special_ans_forms([success], success) :- !.
special_ans_forms([failure], failure) :- !.
special_ans_forms([Error], Error) :-
	Error =..[E |_],error_type(E), !.
special_ans_forms(X,X).
	
%%%%%%%%%%%%%%%%
%
%   error_type(+E).
%

error_type(instantiation_error).
error_type(type_error).
error_type(domain_error).
error_type(existence_error).
error_type(permission_error).
error_type(representation_error).
error_type(evaluation_error).
error_type(resource_error).
error_type(syntax_error).
error_type(system_error).
error_type(unexpected_ball).          % for uncaught errors.





%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Extract  the variables from a term
%
%   vars_int_term(+Term, -Vars)
%

vars_in_term(T,V) :-
	vars_in_term3(T, [], V).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   vars_in_term3(+Term, +AlreadyCollected, -Variables).
%

%  atoms (includes []).

vars_in_term3(Term,VarsIn, VarsOut) :-
      atomic(Term), !, VarsOut= VarsIn.
% Term  is a variable

vars_in_term3(Term, VarsIn, VarsOut) :-
	var(Term) ,!, 
        (already_appears(Term, VarsIn)
        ->
        VarsOut=VarsIn
        ;
        append(VarsIn, [Term], VarsOut)
        ). 

% Term is a list.

vars_in_term3([A|B], VarsIn, Vars) :-
        !, 
        vars_in_term3(A, VarsIn, V1),
        vars_in_term3(B, V1, Vars).

% Term is a functor.

vars_in_term3(T,VarsIn, VarList) :-
       T =.. [_F,A|Args],
       vars_in_term3(A, VarsIn, Inter),
       vars_in_term3(Args, Inter, VarList).

%%%%%%%
%
%  already_appears(+Var,+VarList)
%
%  The variable Var is in the list VarList
%
 

already_appears(Var, [V1 | _Vlist] ) :-
	Var == V1.
already_appears(Var, [_V1 | Vlist] ) :-
        already_appears(Var, Vlist).
 


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%   call_goal_get_subs(+Goal, -Sub)
%
%   call a goal Goal and get the substitutions
%   associated to success.
%


call_goal_get_subs(G, Sub) :-
        copy_term(G,GT),
        vars_in_term(G,Vars),
        vars_in_term(GT, GVars),
        call(GT),
        make_subs_list1(Vars, GVars, Sub).




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  make_subs_list1(+OldVars, +Result, -Sub)
%
%   handles the speical cases else hands off
%  to make_subs_list(OldVars, Result, Sub)
%   and compress to handle [X <-- A, Y <-- A].
%

% special cases

make_subs_list1(_V, success, success).
make_subs_list1(_V, failure, failure).
make_subs_list1(_V, impl_def, impl_def).
make_subs_list1(_V, undefined, undefined).
make_subs_list1(_V, Error, Error) :-
	Error =.. [E|_],
        error_type(E), !.

make_subs_list1(Vs,GVs,Sub) :-
      make_subs_list(Vs, GVs, S),
      compress_sub_list(Vs, S, Sub).



%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   make_subs_list(+Vars, +Result, -Subs).


make_subs_list([],[], []).

% no instantiation.

make_subs_list([V | Rest], [Ans |ARest], Subs) :-
	V == Ans , !,
        make_subs_list(Rest, ARest, Subs).

% Instantiation.

make_subs_list([V | Rest], [Ans |ARest], [ V <-- Ans | SubsRest]) :-
         make_subs_list(Rest, ARest, SubsRest).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   list_make_subs(+Vars, +GTVars, -Subs).
%
%   Make substitution lists for Vars according to 
%  the set of instantiations given in GTVars.
%

list_make_subs_list(_, [], [failure]) :- !.
list_make_subs_list(V, GTV,S) :-
      list_make_subs_list_aux(V,GTV, S).

list_make_subs_list_aux(_Vars, [], []).
list_make_subs_list_aux(Vars, [GV1 |GVRest], [Sub1 |SubRest]) :- 
         make_subs_list1(Vars, GV1, Sub1),
         list_make_subs_list_aux(Vars, GVRest, SubRest).
	

%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%   call_with_result(G,R)
%

call_with_result(G,R ) :-
	call_goal_get_subs(G, Sub), 
        ( Sub = [] -> R = success; R = Sub).
call_with_result(_G, failure).


%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  protected_call_results(G,R)
%

protected_call_results(G,R) :-
      catch(call_with_result(G,R), B, R = B). 


%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
%   get_all_subs(G, AllSubs)
%
%  No errors
%
%  Find all the substitutions for the goal G.
%

get_all_subs(G, AllSubs) :-
	copy_term(G,GT),
        vars_in_term(G, GVars),
        findall(GTAns, protect_call_result(GT, GTAns), GTAnsList),
        list_make_subs_list(GVars, GTAnsList, AllSubs).



%%%%%%%%%%%%%%
%
%  call_result(+Goal, -VarsAfterCall).
%   instantiates VarsAfterCall to the values
%   of the variables in the goal after a call of the goal.
%

call_result(G,R) :-
       
	vars_in_term(G, GVars),
	call(G),
        
        R = GVars.

%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  protect_call_result(G,R)
%
%  protected version of call_result/2.
%

protect_call_result(G,R) :-
	catch(call_result(G,R), B, extract_error(B,R)).


%%%%%%%%%%%%%%%
%
%  extract_error(+Ball, -Error)
%

extract_error(error(R, _), R) :- !.
extract_error(B, unexpected_ball(B)).




%%%%%%%%%%%%%%%%%%%%%
%
%  compress_sub_list(+Vars, +LIn, -LOut)
%
%   to replace pairs [X <--A, Y <-- A] by [Y <-- X]
%   when A is not one of the original variables.


/* I think the author intended to cut, so we don't do (_,L,L) below. - jefft0
compress_sub_list(_, [], success).
 */
compress_sub_list(_, [], success) :- !.
compress_sub_list(Vars, [X <-- A], [X <-- A]) :- X \== A, in_vars(A, Vars).
compress_sub_list(Vars,LIn, LOut) :-
      split_list(X <-- A, Before, After, LIn), 
      var(A),!,
      sub(X <-- A, Before, BN),
      sub(X <-- A, After, AN),
      append(BN,AN, L1),
      compress_sub_list(Vars, L1, LOut).

compress_sub_list(_,L,L).
      
%%%%%%%%%%%%%%%%%%%%%%%%
%
%   in_vars(Var, VarList)
%

in_vars(V, [V1 |_Vs]) :-
       V == V1, !.
in_vars(V, [_V1 |Vs]) :-
      in_vars(V, Vs).	



%%%%%%%%%%%%%%%%%%%
%
%  sub(X <-- A, OldList, NewList)
%
%  substitute A for X in OldList giving NewList.
%

sub(_X <-- _A, [], []).
sub(X <-- A, [H|T], [H1|T1]) :-
	sub1(X <-- A, H,H1),
        sub( X <-- A, T,T1).

%%%%%%%%%%%%%%%%%%%%%
%
%
%   sub1(X <-- A, Y <-- Old, Y <-- New)
%
%  perform a single substitution.
%

sub1(X <-- A, Y <-- Old, Y <-- New) :-
	exp_sub(X<-- A, Old, New).

exp_sub(X <-- A, B, New) :-
	var(B), B== A, !,
        New = X.
exp_sub(_X <-- _A, B, New) :-
        var(B), !,
        New = B.
exp_sub(_X <-- _A, B, New) :-
	atomic(B), !,
        New = B.
exp_sub(X <-- A, B, New) :-
       B = [_|_],!,
       list_exp_sub(X <-- A, B, New).
exp_sub(X <-- A, B, New) :-
	B =.. [F|L],
        list_exp_sub(X <-- A, L,L1),
        New =.. [F|L1].

list_exp_sub(_S, [],[]).
list_exp_sub(S, [E|ER], [EN|ERN]) :-
	exp_sub(S, E, EN),
        list_exp_sub(S, ER, ERN).



%%%%%%%%%%%%%%%%%%%%%%
%
%
%   split_list(?Element,-Before, -After, +List)
%
% split a list List  at a given Element.
%


split_list(Element, Before, After, List) :-
	append(Before, [Element | After], List).
       
	



%%%%%%%%%%%%%%%%%%%%%%%
%
%
%   compare_subst_lists(+First,
%                       +Second,
%                       +InFirstButNotSecond,
%                       +InSecondButNotFirst
%                      )
%   compare two substitution lists.
%

% special cases
compare_subst_lists(F,S, [],[]) :-
	\+ (F = [_|_]),
        \+ (S = [_|_]), 
        F = S, !.
compare_subst_lists(F,S, F,S) :-
      	\+ (F = [_|_]),
        \+ (S = [_|_]), !.
compare_subst_lists(F,S, FNS, SNF) :-
        \+(F = [_|_]), !,
       del_item(F, S, SNF),
      (member(F,S) -> FNS =[]; FNS = F).
compare_subst_lists(F,S, FNS,SNF) :-
     \+( S = [_|_]), !,
      del_item(S, F, FNS),
      (member(S,F) -> SNF =[]; SNF = S).

compare_subst_lists(F, S, [], []) :-
     F= [F1], S = [S1],
     same_subst(F1, S1), !.
 
compare_subst_lists(F, S, F, S) :-
     length(F,1),
     length(S,1), !. 

compare_subst_lists(F,S, FNS,SNF) :-
     length(F,1),!,
      del_item(F, S, SNF),
      (member(F,S) -> FNS =[]; FNS = F).
compare_subst_lists(F,S, FNS,SNF) :-
     length(S,1),
      del_item(S, F, FNS),
      (member(S,F) -> SNF =[]; SNF = S).


compare_subst_lists(F,S, FNS, SNF) :-
	list_del_item(F,S, SNF),
        list_del_item(S,F, FNS).


%list_del_item(L1, L2, L2LessL1)

list_del_item([], L,L).
list_del_item([It|R], L1, Left) :-
	del_item(It, L1, LInter),
        list_del_item(R, LInter, Left).
           
del_item(_Item, [],[]).
del_item(Item, [It |R], R) :-
      same_subst(Item, It), ! .
    %  del_item(Item, Rest, R).
del_item(Item, [It|Rest], [It |R]) :-
	del_item(Item, Rest, R).

%%
%  same_subst(Sub1, Sub2)
%
%  Sub1 and Sub2 represent the same subst.
%

same_subst([],[]).
same_subst([S1|SRest], Subs) :-
        delmemb(S1, Subs, Subs1),
        same_subst(SRest, Subs1).

%%%%%%%%%%
%
%  delmemb(Item, List, ListMinusItem)
%
% special delete for substitutions.
%

delmemb(_E, [], []).
delmemb(E <-- E1 , [F <-- F1| R], R) :-
         E == F, 
	copy_term(E <-- E1 ,F <-- F1).  % only when LHS's are eq.
delmemb(E, [F|R], [F|R1]) :-
      delmemb(E,R,R1).


%%%%%%%%%%%%%%%%%%%%
%
%    read_test(-Extra,-Missing)
%
%    read a test [G,Expected] from standard in
%    and find the Missing and Extra substitutions.
%
/*
read_test(Extra, Missing) :-
	read(X),
        X = [G, Expected],
        result(G, R),
        compare_subst_lists(R, Expected, Extra, Missing),
        write('Extra Solutions found: '), write(Extra), nl,
        write('Solutions Missing: '), write(Missing).
*/      
        
%%%%%%%%%
%
%   read tests from a file
%


run_tests(File) :-
        asserta(score(File, total(0), wrong(0))),
	open(File, read, S),
        loop_through(File,S),
        close(S).

%%%%%%%%%%%%%%%%%%%%
%
%    loop_through(+File,+Source)
%
%    read a term from the file and test the term
%    the catch is for syntax errors 
%    (which will be errors in the processor).
%
 
/* Change loop_through to use fileContents directly.
loop_through(F, S) :-
	catch(read(S,X), B, X = B),
        (
       X = end_of_file
        -> true
       ;
        reset_flags,
        test(F,X),
        loop_through(F,S)
       ).
 */
loop_through(F, _S) :-
	fileContents(F, X),
        reset_flags,
	% test only has side effects, so call it and fail to get more fileContents.
        test(F,X), fail.
loop_through(_F, _S).

%%%%%%%%%%%%%%%%%%%
%
%   test(+File, +TermRead)
%
%  do the tests. Handles syntax erros in input and end_of_file
%

test(_,end_of_file).
test(F, error(R, _)) :- !,
        write('Error in Input: '), write(R), nl,nl,
        update_score(F, non_null, non_null).

test(F,[G,Expected]) :-	
       result(G,R),
        compare_subst_lists(R, Expected, Extra, Missing),
        write_if_wrong(F, G, Expected, Extra, Missing),
        update_score(F, Missing, Extra).
/* This is only used for testing file I/O.
test(F, [G, ProgFile, Expected]) :-
	[ProgFile],
        result(G,R),
        compare_subst_lists(R, Expected, Extra, Missing),
        write_if_wrong(F, G, Expected, Extra, Missing),
        update_score(F, Missing, Extra). 
*/
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   write_if_wrong(+File, +Goal, +Expected, +Extra, +Missing)
%
% If Either Extra or Missing are non empty write
% an appropriate message.
%
%  A more legant output is possible if the processor supports
%  numbervars/3, insert ther commented out line.
%

write_if_wrong(_,_,_,[],[]):- !.
write_if_wrong(F, G,Expected, Extra, Missing) :-
        fake_numbervars([G,Expected, Missing], 0, _),
        write('In file: '), write(F), nl,
        write('possible error in Goal: '), 
        write(G), nl,
        write('Expected: '), write(Expected), nl,
        write('Extra Solutions found: '), write(Extra), nl,
        write('Solutions Missing: '), write(Missing),nl,nl.
 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  update_score(+File, +Missing, +Extra)
%
%  add 1 to total in all cases.
%  If Missing or Extra are non empty add one to wrong.
%

update_score(F,[],[]) :- !,
	retract(score(F,total(T), wrong(W))),
        T1 is T +1,
        asserta(score(F,total(T1), wrong(W))).
update_score(F,_,_) :-
       retract(score(F,total(T), wrong(W))),
        T1 is T +1, W1 is W + 1,
        asserta(score(F,total(T1), wrong(W1))). 


%%%%%%%%%%%%%
%
%  inerror(?F)
%
%     One of the tests in the file gave an
% unexpected answer.
%



inerror(F) :-
	score(F, total(_X), wrong(Y)),
        Y =\= 0.



%%%%%%%%%
%
%   list all the files 
%   of tests.
%
        	
file(fail).
file(abolish).
file(and).
file(arg).
file(arith_diff).
file(arith_eq).
file(arith_gt).
file('arith_gt=').
file(arith_lt).
file('arith_lt=').
file(asserta).
file(assertz).
file(atom).
file(atom_chars).
file(atom_codes).
file(atom_concat).
file(atom_length).
file(atomic).
file(bagof).
file(call).
file('catch-and-throw').
file(char_code).
file(clause).
file(compound).
file(copy_term).
file(current_input).  % default names of input are imp-def.
file(current_output).
file(current_predicate).
file(current_prolog_flag).
file(cut).
%file(file_manip).  % needs complete rewite.
file(findall).
file(float).
file(functor).
file('if-then').
file('if-then-else').
file(integer).
file(is).
file(nonvar).
file(not_provable).
file(not_unify).
file(number).
file(number_chars).
file(number_codes).
file(once).
file(or).
file(repeat).
file(retract).
file(set_prolog_flag).
file(setof).
file(sub_atom).
file(term_diff).
file(term_eq).
file(term_gt).
file('term_gt=').
file(term_lt).
file('term_lt=').
file(true).
file(unify).


       
%%%%%%%%%%%
%
%   display_list(+List)
%


display_list([]) :- nl.
display_list([H|T]) :-
	write(H), nl,
        display_list(T).


%%%%%%%%%%%%%%%%%
%
%   reset_flags
%
%  some tests reset the prolog flags.
% in order to fix this we restore them to their default values.
% This is why fail is the first test.


reset_flags :-
	set_prolog_flag(unknown, error).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%    tests to see if a given predicate (a bip)
%   exists. Used for current_input and current_output
%   since they don't have default values for the 
%   streams.

exists(P/I) :-
      make_list(I,List),
      G =.. [P|List],
      set_prolog_flag(unknown, fail),
      catch(call(G),_ , true),
      reset_flags,!.
exists(P/I) :-
      write('Predicate: '), write(P/I), write(' not implemented'), nl,
      reset_flags.



%%%%%%%%%%%%%
%
%   make_list(Len, List).
%
make_list(N,L) :-
	N >= 0, 
        make_list1(N,L).
make_list1(0,[]).
make_list1(N, [_|L1]) :-
     N1 is N -1,
     make_list(N1, L1).


%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   fake_numbervars/3
%
%  Like numbervars
%

fake_numbervars(X,N,M) :-
	var(X), !,
        X =.. ['$VAR', N],
        M is N + 1.
fake_numbervars(X, N,N) :-
	atomic(X), !.
fake_numbervars([H|T], N, M) :- !,
	fake_numbervars(H, N, N1),
        fake_numbervars(T, N1, M).
fake_numbervars(T, N, M) :-
        T =.. [_F |Args],
        fake_numbervars(Args, N,M).

member(X,[X|_]).
member(X,[_|Rest]) :- member(X,Rest).

append([],List,List).
append([X|List1],List2,[X|List12]) :- append(List1,List2,List12).

:-if(\+current_predicate(length/2)).
length([], 0) :- !.
length([_|Rest], Length) :- length(Rest, RestLength), Length is RestLength + 1.
:-endif.

% Define file operations locally because they are not supported.
:-if(\+current_predicate(open/3)).
open(_, _, _).
:-endif.
:-if(\+current_predicate(close/1)).
close(_).
:-endif.

% This fails for Javascript since it doesn't distinguish int and float, 
%   so we don't run tests that depend on this.
intAndFloatAreDifferent :- 1.0 \== 1.

% Define the test file contents locally since we aren't testing read_term.
fileContents('abolish', [abolish(abolish/1), permission_error(modify,static_procedure,abolish/1)]).
fileContents('abolish', [abolish(foo/a), type_error(integer,a)]).
fileContents('abolish', [abolish(foo/(-1)), domain_error(not_less_than_zero,-1)]).
fileContents('abolish', [(current_prolog_flag(max_arity,A), X is A + 1, abolish(foo/X)),
                     representation_error(max_arity)]).
fileContents('abolish', [abolish(5/2), type_error(atom,5)]).
fileContents('and', [','(X=1, var(X)), failure]).
fileContents('and', [','(var(X), X=1), [[X <-- 1]]]).
fileContents('and', [','(fail, call(3)), failure]).
fileContents('and', [','(nofoo(X), call(X)), existence_error(procedure, nofoo/1)]).
fileContents('and', [','(X = true, call(X)), [[X <-- true]]]).
fileContents('arg', [arg(1,foo(a,b),a), success]).
fileContents('arg', [arg(1,foo(a,b),X), [[X <-- a]]]).
fileContents('arg', [arg(1,foo(X,b),a), [[X <-- a]]]).
fileContents('arg', [arg(2,foo(a, f(X,b), c), f(a, Y)), [[X <-- a, Y <-- b]]]).
%fileContents('arg', [arg(1,foo(X,b),Y), [[Y <-- X]]]).
% The test suite code doesn't check Y <-- X properly, so use a ground value.
fileContents('arg', [(arg(1,foo(X,b),Y), X=a), [[Y <-- a]]]).
fileContents('arg', [arg(1,foo(a,b),b), failure]).
fileContents('arg', [arg(0,foo(a,b),foo), failure]).
fileContents('arg', [arg(3,foo(3,4),N), failure]).
fileContents('arg', [arg(X,foo(a,b),a), instantiation_error]).
fileContents('arg', [arg(1,X,a), instantiation_error]).
fileContents('arg', [arg(0,atom,A), type_error(compound, atom)]).
fileContents('arg', [arg(0,3,A), type_error(compound, 3)]).
fileContents('arg', [arg(-3,foo(a,b),A), domain_error(not_less_than_zero,-3)]).
fileContents('arg', [arg(a,foo(a,b),X), type_error(integer, a)]).
fileContents('arith_diff', ['=\\='(0,1), success]).
fileContents('arith_diff', ['=\\='(1.0,1), failure]).
fileContents('arith_diff', ['=\\='(3 * 2,7 - 1), failure]).
fileContents('arith_diff', ['=\\='(N,5), instantiation_error]).
fileContents('arith_diff', ['=\\='(floot(1),5), type_error(evaluable, floot/1)]).
fileContents('arith_eq', ['=:='(0,1), failure]).
fileContents('arith_eq', ['=:='(1.0,1), success]).
fileContents('arith_eq', ['=:='(3 * 2,7 - 1), success]).
fileContents('arith_eq', ['=:='(N,5), instantiation_error]).
fileContents('arith_eq', ['=:='(floot(1),5), type_error(evaluable, floot/1)]).
fileContents('arith_eq', [0.333 =:= 1/3, failure]).
fileContents('arith_gt', ['>'(0,1), failure]).
fileContents('arith_gt', ['>'(1.0,1), failure]).
fileContents('arith_gt', ['>'(3*2,7-1), failure]).
fileContents('arith_gt', ['>'(X,5), instantiation_error]).
fileContents('arith_gt', ['>'(2 + floot(1),5), type_error(evaluable, floot/1)]).
fileContents('arith_gt=', ['>='(0,1), failure]).
fileContents('arith_gt=', ['>='(1.0,1), success]).
fileContents('arith_gt=', ['>='(3*2,7-1), success]).
fileContents('arith_gt=', ['>='(X,5), instantiation_error]).
fileContents('arith_gt=', ['>='(2 + floot(1),5), type_error(evaluable, floot/1)]).
fileContents('arith_lt', ['<'(0,1), success]).
fileContents('arith_lt', ['<'(1.0,1), failure]).
fileContents('arith_lt', ['<'(3*2,7-1), failure]).
fileContents('arith_lt', ['<'(X,5), instantiation_error]).
fileContents('arith_lt', ['<'(2 + floot(1),5), type_error(evaluable, floot/1)]).
fileContents('arith_lt=', ['=<'(0,1), success]).
fileContents('arith_lt=', ['=<'(1.0,1), success]).
fileContents('arith_lt=', ['=<'(3*2,7-1), success]).
fileContents('arith_lt=', ['=<'(X,5), instantiation_error]).
fileContents('arith_lt=', ['=<'(2 + floot(1),5), type_error(evaluable, floot/1)]).
fileContents('asserta', [(asserta((bar(X) :- X)), clause(bar(X), B)), [[B <-- call(X)]]]).
fileContents('asserta', [asserta(_), instantiation_error]).
fileContents('asserta', [asserta(4), type_error(callable, 4)]).
fileContents('asserta', [asserta((foo :- 4)), type_error(callable, 4)]).
fileContents('asserta', [asserta((atom(_) :- true)), permission_error(modify,static_procedure,atom/1)]).
fileContents('assertz', [assertz((foo(X) :- X -> call(X))), success]).
fileContents('assertz', [assertz(_), instantiation_error]).
fileContents('assertz', [assertz(4), type_error(callable, 4)]).
fileContents('assertz', [assertz((foo :- 4)), type_error(callable, 4)]).
fileContents('assertz', [assertz((atom(_) :- true)),
                     permission_error(modify,static_procedure,atom/1)]).
fileContents('atom', [atom(atom), success]).
fileContents('atom', [atom('string'), success]).
fileContents('atom', [atom(a(b)), failure]).
fileContents('atom', [atom(Var), failure]).
fileContents('atom', [atom([]), success]).
fileContents('atom', [atom(6), failure]).
fileContents('atom', [atom(3.3), failure]).
fileContents('atom_chars', [atom_chars('',L), [[L <-- []]]]).
fileContents('atom_chars', [atom_chars([],L), [[L <-- ['[',']']]]]).
fileContents('atom_chars', [atom_chars('''',L), [[L <-- ['''']]]]).
fileContents('atom_chars', [atom_chars('iso',L), [[L <-- ['i','s','o']]]]).
fileContents('atom_chars', [atom_chars(A,['p','r','o','l','o','g']), [[A <-- 'prolog']]]).
fileContents('atom_chars', [atom_chars('North',['N'|X]), [[X <-- ['o','r','t','h']]]]).
fileContents('atom_chars', [atom_chars('iso',['i','s']), failure]).
fileContents('atom_chars', [atom_chars(A,L), instantiation_error]).
fileContents('atom_chars', [atom_chars(A,[a,E,c]), instantiation_error]).
fileContents('atom_chars', [atom_chars(A,[a,b|L]), instantiation_error]).
fileContents('atom_chars', [atom_chars(f(a),L), type_error(atom,f(a))]).
fileContents('atom_chars', [atom_chars(A,iso), type_error(list,iso)]).
fileContents('atom_chars', [atom_chars(A,[a,f(b)]), type_error(character,f(b))]).
fileContents('atom_chars', [(atom_chars(X,['1','2']), Y is X + 1), type_error(evaluable, '12'/0)]).
fileContents('atom_codes', [atom_codes('',L), [[L <-- []]]]).
fileContents('atom_codes', [atom_codes([],L), [[L <-- [ 0'[, 0'] ]]]]).
fileContents('atom_codes', [atom_codes('''',L), [[L <-- [ 39 ]]]]).
fileContents('atom_codes', [atom_codes('iso',L), [[L <-- [ 0'i, 0's, 0'o ]]]]).
fileContents('atom_codes', [atom_codes(A,[ 0'p, 0'r, 0'o, 0'l, 0'o, 0'g]), [[A <-- 'prolog']]]).
fileContents('atom_codes', [atom_codes('North',[0'N | L]), [[L <-- [0'o, 0'r, 0't, 0'h]]]]).
fileContents('atom_codes', [atom_codes('iso',[0'i, 0's]), failure]).
fileContents('atom_codes', [atom_codes(A,L), instantiation_error]).
fileContents('atom_codes', [atom_codes(f(a),L), type_error(atom,f(a))]).
fileContents('atom_codes', [atom_codes(A, 0'x), type_error(list,0'x)]).
fileContents('atom_codes', [atom_codes(A,[ 0'i, 0's, o]), representation_error(character_code)]).
fileContents('atom_concat', [atom_concat('hello',' world',A), [[A <-- 'hello world']]]).
fileContents('atom_concat', [atom_concat(T,' world','small world'), [[T <-- 'small']]]).
fileContents('atom_concat', [atom_concat('hello',' world','small world'), failure]).
fileContents('atom_concat', [atom_concat(T1,T2,'hello'),
                     [[T1 <-- '',T2 <-- 'hello'],
                     [T1 <-- 'h',T2 <-- 'ello'],
                     [T1 <-- 'he',T2 <-- 'llo'],
                     [T1 <-- 'hel',T2 <-- 'lo'],
                     [T1 <-- 'hell',T2 <-- 'o'],
                     [T1 <-- 'hello',T2 <-- '']]]).
fileContents('atom_concat', [atom_concat(A1,'iso',A3), instantiation_error]).
fileContents('atom_concat', [atom_concat('iso',A2,A3), instantiation_error]).
fileContents('atom_concat', [atom_concat(f(a),'iso',A3), type_error(atom,f(a))]).
fileContents('atom_concat', [atom_concat('iso',f(a),A3), type_error(atom,f(a))]).
fileContents('atom_concat', [atom_concat(A1,A2,f(a)), type_error(atom,f(a))]).
fileContents('atom_length', [atom_length('enchanted evening', N), [[N <-- 17]]]).
fileContents('atom_length', [atom_length('', N), [[N <-- 0]]]).
fileContents('atom_length', [atom_length('scarlet', 5), failure]).
fileContents('atom_length', [atom_length(Atom, 4), instantiation_error]).
fileContents('atom_length', [atom_length(1.23, 4), type_error(atom, 1.23)]).
fileContents('atom_length', [atom_length(atom, '4'), type_error(integer, '4')]).
fileContents('atomic', [atomic(atom), success]).
fileContents('atomic', [atomic(a(b)), failure]).
fileContents('atomic', [atomic(Var), failure]).
fileContents('atomic', [atomic([]), success]).
fileContents('atomic', [atomic(6), success]).
fileContents('atomic', [atomic(3.3), success]).
fileContents('bagof', [bagof(X,(X=1;X=2),L), [[L <-- [1, 2]]]]).
fileContents('bagof', [bagof(X,(X=1;X=2),X), [[X <-- [1, 2]]]]).
fileContents('bagof', [bagof(X,(X=Y;X=Z),L), [[L <-- [Y, Z]]]]).
fileContents('bagof', [bagof(X,fail,L), failure]).
fileContents('bagof', [bagof(1,(Y=1;Y=2),L), [[L <-- [1], Y <-- 1], [L <-- [1], Y <-- 2]]]).
fileContents('bagof', [bagof(f(X,Y),(X=a;Y=b),L), [[L <-- [f(a, _), f(_, b)]]]]).
fileContents('bagof', [bagof(X,Y^((X=1,Y=1);(X=2,Y=2)),S), [[S <-- [1, 2]]]]).
fileContents('bagof', [bagof(X,Y^((X=1;Y=1);(X=2,Y=2)),S), [[S <-- [1, _, 2]]]]).
fileContents('bagof', [(set_prolog_flag(unknown, warning),
                     bagof(X,(Y^(X=1;Y=1);X=3),S)), [[S <-- [3]]]]).
fileContents('bagof', [bagof(X,(X=Y;X=Z;Y=1),L), [[L <-- [Y, Z]], [L <-- [_], Y <-- 1]]]).
fileContents('bagof', [bagof(X,Y^Z,L), instantiation_error]).
fileContents('bagof', [bagof(X,1,L), type_error(callable, 1)]).
fileContents('bagof', [findall(X,call(4),S),type_error(callable, 4)]).
fileContents('call', [call(!),success]).
fileContents('call', [call(fail), failure]).
fileContents('call', [call((fail, X)), failure]).
fileContents('call', [call((fail, call(1))), failure]).
fileContents('call', [call((write(3), X)), instantiation_error]).
fileContents('call', [call((write(3), call(1))), type_error(callable,1)]).
fileContents('call', [call(X), instantiation_error]).
fileContents('call', [call(1), type_error(callable,1)]).
fileContents('call', [call((fail, 1)), type_error(callable,(fail,1))]).
fileContents('call', [call((write(3), 1)), type_error(callable,(write(3), 1))]).
fileContents('call', [call((1; true)), type_error(callable,(1; true))]).
fileContents('catch-and-throw', [(catch(true, C, write('something')), throw(blabla)), system_error]).
fileContents('catch-and-throw', [catch(number_chars(A,L), error(instantiation_error, _), fail), failure]).
fileContents('char_code', [char_code(a,Code),[[Code <-- 0'a]]]).
fileContents('char_code', [char_code(Char,99),[[Char <-- c]]]).
fileContents('char_code', [char_code(Char,0'c),[[Char <-- c]]]).
% Parser doesn't support hex codes in atom strings.
% fileContents('char_code', [char_code(Char,163),[[Char <-- '\xa3\']]]).
fileContents('char_code', [char_code(b,98),success]).
fileContents('char_code', [char_code(b,4),failure]).
fileContents('char_code', [char_code('ab',Code),type_error(character, 'ab')]).
fileContents('char_code', [char_code(a,x),type_error(integer, x)]).
fileContents('char_code', [char_code(Char,Code),instantiation_error]).
fileContents('char_code', [char_code(Char,-2),representation_error(character_code)]).
fileContents('clause', [clause(x,Body), failure]).
fileContents('clause', [clause(_,B), instantiation_error]).
fileContents('clause', [clause(4,B), type_error(callable,4)]).
fileContents('clause', [clause(f(_),5), type_error(callable,5)]).
fileContents('clause', [clause(atom(_),Body), permission_error(access,private_procedure,atom/1)]).
fileContents('compound', [compound(33.3), failure]).
fileContents('compound', [compound(-33.3), failure]).
fileContents('compound', [compound(-a), success]).
fileContents('compound', [compound(_), failure]).
fileContents('compound', [compound(a), failure]).
fileContents('compound', [compound(a(b)), success]).
fileContents('compound', [compound([a]),success]).
fileContents('copy_term', [copy_term(X,Y), success]).
fileContents('copy_term', [copy_term(X,3), success]).
fileContents('copy_term', [copy_term(_,a), success]).
fileContents('copy_term', [copy_term(a+X,X+b),[[X <-- a]]]).
fileContents('copy_term', [copy_term(_,_), success]).
%fileContents('copy_term', [copy_term(X+X+Y,A+B+B),[[B <-- A]]]).
% The test suite code doesn't check B <-- A properly, so use a ground value.
fileContents('copy_term', [(copy_term(X+X+Y,A+B+B), A=1) ,[[B <-- 1]]]).
fileContents('copy_term', [copy_term(a,a), success]).
fileContents('copy_term', [copy_term(a,b), failure]).
fileContents('copy_term', [copy_term(f(a),f(X)),[[X <-- a]]]).
fileContents('copy_term', [(copy_term(a+X,X+b),copy_term(a+X,X+b)), failure]).
fileContents('current_input', [exists(current_input/1), success]).
fileContents('current_output', [exists(current_output/1), success]).
fileContents('current_predicate', [current_predicate(current_predicate/1), failure]).
% Check for a current dynamic predicate.
fileContents('current_predicate', [current_predicate(score/3), success]).
% Use functor to get the name of a static predicate including the module.
fileContents('current_predicate', [functor(run_tests(1), Name, _),current_predicate(Name/1), success]).
fileContents('current_predicate', [current_predicate(4), type_error(predicate_indicator, 4)]).
fileContents('current_predicate', [current_predicate(dog), type_error(predicate_indicator, dog)]).
fileContents('current_predicate', [current_predicate(0/dog), type_error(predicate_indicator, 0/dog)]).
fileContents('current_prolog_flag', [current_prolog_flag(debug, off), success]).
fileContents('current_prolog_flag', [(set_prolog_flag(unknown, warning),
                     current_prolog_flag(unknown, warning)), success]).
fileContents('current_prolog_flag', [(set_prolog_flag(unknown, warning),
                     current_prolog_flag(unknown, error)), failure]).
fileContents('current_prolog_flag', [current_prolog_flag(debug, V), [[V <-- off]]]).
fileContents('current_prolog_flag', [current_prolog_flag(5, V), type_error(atom,5)]).
fileContents('current_prolog_flag', [current_prolog_flag(warning, V), domain_error(prolog_flag,warning)]).
fileContents('cut', [!, success]).
fileContents('cut', [(!,fail;true), failure]).
fileContents('cut', [(call(!),fail;true), success]).
fileContents('fail', [fail, failure]).
fileContents('fail', [undef_pred, existence_error(procedure, undef_pred/0)]).
fileContents('fail', [(set_prolog_flag(unknown, fail), undef_pred), failure]).
fileContents('fail', [(set_prolog_flag(unknown, warning), undef_pred), failure]).
fileContents('findall', [findall(X,(X=1 ; X=2),S),[[S <-- [1,2]]]]).
fileContents('findall', [findall(X+Y,(X=1),S),[[S <-- [1+_]]]]).
fileContents('findall', [findall(X,fail,L),[[L <-- []]]]).
fileContents('findall', [findall(X,(X=1 ; X=1),S),[[S <-- [1,1]]]]).
fileContents('findall', [findall(X,(X=2 ; X=1),[1,2]), failure]).
fileContents('findall', [findall(X,(X=1 ; X=2),[X,Y]), [[X <-- 1, Y <-- 2]]]).
fileContents('findall', [findall(X,Goal,S),instantiation_error]).
fileContents('findall', [findall(X,4,S),type_error(callable, 4)]).
fileContents('findall', [findall(X,call(1),S),type_error(callable, 1)]).
fileContents('float', [float(3.3), success]).
fileContents('float', [float(-3.3), success]).
fileContents('float', [float(3), failure]) :- intAndFloatAreDifferent.
fileContents('float', [float(atom), failure]).
fileContents('float', [float(X), failure]).
fileContents('functor', [functor(foo(a,b,c),foo,3), success]).
fileContents('functor', [functor(foo(a,b,c),X,Y), [[X <-- foo, Y <-- 3]]]).
fileContents('functor', [functor(X,foo,3), [[X <-- foo(A,B,C)]]]).
fileContents('functor', [functor(X,foo,0), [[X <-- foo]]]).
fileContents('functor', [functor(mats(A,B),A,B), [[A <-- mats,B <-- 2]]]).
fileContents('functor', [functor(foo(a),foo,2), failure]).
fileContents('functor', [functor(foo(a),fo,1), failure]).
fileContents('functor', [functor(1,X,Y), [[X <-- 1,Y <-- 0]]]).
fileContents('functor', [functor(X,1.1,0), [[X <-- 1.1]]]).
fileContents('functor', [functor([_|_],'.',2), success]).
fileContents('functor', [functor([],[],0), success]).
fileContents('functor', [functor(X, Y, 3), instantiation_error]).
fileContents('functor', [functor(X, foo, N), instantiation_error]).
fileContents('functor', [functor(X, foo, a), type_error(integer,a)]).
fileContents('functor', [functor(F, 1.5, 1), type_error(atom,1.5)]).
fileContents('functor', [functor(F,foo(a),1), type_error(atomic,foo(a))]).
fileContents('functor', [(current_prolog_flag(max_arity,A), X is A + 1, functor(T, foo, X)),
                     representation_error(max_arity)]).
fileContents('functor', [functor(T, foo, -1), domain_error(not_less_than_zero,-1)]).
fileContents('if-then', ['->'(true, true), success]).
fileContents('if-then', ['->'(true, fail), failure]).
fileContents('if-then', ['->'(fail, true), failure]).
fileContents('if-then', ['->'(true, X=1), [[X <-- 1]]]).
fileContents('if-then', ['->'(';'(X=1, X=2), true), [[X <-- 1]]]).
fileContents('if-then', ['->'(true, ';'(X=1, X=2)), [[X <-- 1], [X <-- 2]]]).
fileContents('if-then-else', [';'('->'(true, true), fail), success]).
fileContents('if-then-else', [';'('->'(fail, true), true), success]).
fileContents('if-then-else', [';'('->'(true, fail), fail), failure]).
fileContents('if-then-else', [';'('->'(fail, true), fail), failure]).
fileContents('if-then-else', [';'('->'(true, X=1), X=2), [[X <-- 1]]]).
fileContents('if-then-else', [';'('->'(fail, X=1), X=2), [[X <-- 2]]]).
fileContents('if-then-else', [';'('->'(true, ';'(X=1, X=2)), true), [[X <-- 1], [X <-- 2]]]).
fileContents('if-then-else', [';'('->'(';'(X=1, X=2), true), true), [[X <-- 1]]]).
fileContents('integer', [integer(3), success]).
fileContents('integer', [integer(-3), success]).
fileContents('integer', [integer(3.3), failure]).
fileContents('integer', [integer(X), failure]).
fileContents('integer', [integer(atom), failure]).
fileContents('is', ['is'(Result,3 + 11.0),[[Result <-- 14.0]]]).
fileContents('is', [(X = 1 + 2, 'is'(Y, X * 3)),[[X <-- (1 + 2), Y <-- 9]]]).
fileContents('is', ['is'(foo,77), failure]).
fileContents('is', ['is'(77, N), instantiation_error]).
fileContents('is', ['is'(77, foo), type_error(evaluable, foo/0)]).
fileContents('is', ['is'(X,float(3)),[[X <-- 3.0]]]).
fileContents('nonvar', [nonvar(33.3), success]).
fileContents('nonvar', [nonvar(foo), success]).
fileContents('nonvar', [nonvar(Foo), failure]).
fileContents('nonvar', [(foo=Foo,nonvar(Foo)),[[Foo <-- foo]]]).
fileContents('nonvar', [nonvar(_), failure]).
fileContents('nonvar', [nonvar(a(b)), success]).
fileContents('not_provable', [\+(true), failure]).
fileContents('not_provable', [\+(!), failure]).
fileContents('not_provable', [\+((!,fail)), success]).
fileContents('not_provable', [((X=1;X=2), \+((!,fail))), [[X <-- 1],[X <-- 2]]]).
fileContents('not_provable', [\+(4 = 5), success]).
fileContents('not_provable', [\+(3), type_error(callable, 3)]).
fileContents('not_provable', [\+(X), instantiation_error]).
fileContents('not_unify', ['\\='(1,1), failure]).
fileContents('not_unify', ['\\='(X,1), failure]).
fileContents('not_unify', ['\\='(X,Y), failure]).
fileContents('not_unify', [('\\='(X,Y),'\\='(X,abc)), failure]).
fileContents('not_unify', ['\\='(f(X,def),f(def,Y)), failure]).
fileContents('not_unify', ['\\='(1,2), success]).
fileContents('not_unify', ['\\='(1,1.0), success]) :- intAndFloatAreDifferent.
fileContents('not_unify', ['\\='(g(X),f(f(X))), success]).
fileContents('not_unify', ['\\='(f(X,1),f(a(X))), success]).
fileContents('not_unify', ['\\='(f(X,Y,X),f(a(X),a(Y),Y,2)), success]).
fileContents('number', [number(3), success]).
fileContents('number', [number(3.3), success]).
fileContents('number', [number(-3), success]).
fileContents('number', [number(a), failure]).
fileContents('number', [number(X), failure]).
fileContents('number_chars', [number_chars(33,L), [[L <-- ['3','3']]]]).
fileContents('number_chars', [number_chars(33,['3','3']), success]).
fileContents('number_chars', [number_chars(33.0,L), [[L <-- ['3','3','.','0']]]]) :- intAndFloatAreDifferent.
fileContents('number_chars', [number_chars(X,['3','.','3','E','+','0']), [[X <-- 3.3]]]).
fileContents('number_chars', [number_chars(3.3,['3','.','3']), success]).
fileContents('number_chars', [number_chars(A,['-','2','5']), [[A <-- (-25)]]]).
fileContents('number_chars', [number_chars(A,['\n',' ','3']), [[A <-- 3]]]).
fileContents('number_chars', [number_chars(A,['3',x]), syntax_error(_)]).
fileContents('number_chars', [number_chars(A,['0',x,f]), [[A <-- 15]]]).
fileContents('number_chars', [number_chars(A,['0','''','A']), [[A <-- 65]]]).
fileContents('number_chars', [number_chars(A,['4','.','2']), [[A <-- 4.2]]]).
fileContents('number_chars', [number_chars(A,['4','2','.','0','e','-','1']), [[A <-- 4.2]]]).
fileContents('number_chars', [number_chars(A,L), instantiation_error]).
fileContents('number_chars', [number_chars(a,L), type_error(number, a)]).
fileContents('number_chars', [number_chars(A,4), type_error(list, 4)]).
fileContents('number_chars', [number_chars(A,['4',2]), type_error(character, 2)]).
fileContents('number_codes', [number_codes(33,L), [[L <-- [0'3,0'3]]]]).
fileContents('number_codes', [number_codes(33,[0'3,0'3]), success]).
fileContents('number_codes', [number_codes(33.0,L), [[L <-- [51,51,46,48]]]]) :- intAndFloatAreDifferent.
fileContents('number_codes', [number_codes(33.0,[0'3,0'3,0'.,0'0]), success]) :- intAndFloatAreDifferent.
fileContents('number_codes', [number_codes(A,[0'-,0'2,0'5]), [[A <-- (-25)]]]).
fileContents('number_codes', [number_codes(A,[0' ,0'3]), [[A <-- 3]]]).
fileContents('number_codes', [number_codes(A,[0'0,0'x,0'f]), [[A <-- 15]]]).
fileContents('number_codes', [number_codes(A,[0'0,39,0'a]), [[A <-- 97]]]).
fileContents('number_codes', [number_codes(A,[0'4,0'.,0'2]), [[A <-- 4.2]]]).
fileContents('number_codes', [number_codes(A,[0'4,0'2,0'.,0'0,0'e,0'-,0'1]), [[A <-- 4.2]]]).
fileContents('number_codes', [number_codes(A,L), instantiation_error]).
fileContents('number_codes', [number_codes(a,L), type_error(number,a)]).
fileContents('number_codes', [number_codes(A,4), type_error(list,4)]).
fileContents('number_codes', [number_codes(A,[ 0'1, 0'2, '3']), representation_error(character_code)]).
fileContents('once', [once(!), success]).
fileContents('once', [(once(!), (X=1; X=2)), [[X <-- 1],[X <-- 2]]]).
fileContents('once', [once(repeat), success]).
fileContents('once', [once(fail), failure]).
fileContents('once', [once(3), type_error(callable, 3)]).
fileContents('once', [once(X), instantiation_error]).
fileContents('or', [';'(true, fail), success]).
fileContents('or', [';'((!, fail), true), failure]).
fileContents('or', [';'(!, call(3)), success]).
fileContents('or', [';'((X=1, !), X=2), [[X <-- 1]]]).
fileContents('or', [';'(X=1, X=2), [[X <-- 1], [X <-- 2]]]).
fileContents('repeat', [(repeat,!,fail), failure]).
fileContents('retract', [retract((4 :- X)), type_error(callable, 4)]).
fileContents('retract', [retract((atom(_) :- X == '[]')),
                     permission_error(modify,static_procedure,atom/1)]).
fileContents('set_prolog_flag', [(set_prolog_flag(unknown, fail),
                     current_prolog_flag(unknown, V)), [[V <-- fail]]]).
fileContents('set_prolog_flag', [set_prolog_flag(X, warning), instantiation_error]).
fileContents('set_prolog_flag', [set_prolog_flag(5, decimals), type_error(atom,5)]).
fileContents('set_prolog_flag', [set_prolog_flag(date, 'July 1999'), domain_error(prolog_flag,date)]).
fileContents('set_prolog_flag', [set_prolog_flag(debug, no), domain_error(flag_value,debug+no)]).
fileContents('set_prolog_flag', [set_prolog_flag(max_arity, 40), permission_error(modify, flag, max_arity)]).
fileContents('set_prolog_flag', [set_prolog_flag(double_quotes, atom), success]).
%fileContents('set_prolog_flag', [X = "fred", [[X <-- fred]]]).
%Use read/2 because the fileContents predicate is already parsed.
fileContents('set_prolog_flag', [read("\"fred\". ", X), [[X <-- fred]]]).
fileContents('set_prolog_flag', [set_prolog_flag(double_quotes, chars), success]).
%fileContents('set_prolog_flag', [X = "fred", [[X <-- [f,r,e,d]]]]).
fileContents('set_prolog_flag', [read("\"fred\". ", X), [[X <-- [f,r,e,d]]]]).
fileContents('set_prolog_flag', [set_prolog_flag(double_quotes, codes), success]).
%fileContents('set_prolog_flag', [X = "fred", [[X <-- [102,114,101,100]]]]).
fileContents('set_prolog_flag', [read("\"fred\". ", X), [[X <-- [102,114,101,100]]]]).
fileContents('setof', [setof(X,(X=1;X=2),L), [[L <-- [1, 2]]]]).
fileContents('setof', [setof(X,(X=1;X=2),X), [[X <-- [1, 2]]]]).
fileContents('setof', [setof(X,(X=2;X=1),L), [[L <-- [1, 2]]]]).
fileContents('setof', [setof(X,(X=2;X=2),L), [[L <-- [2]]]]).
fileContents('setof', [setof(X,fail,L), failure]).
fileContents('setof', [setof(1,(Y=2;Y=1),L), [[L <-- [1], Y <-- 1], [L <-- [1], Y <-- 2]]]).
fileContents('setof', [setof(f(X,Y),(X=a;Y=b),L), [[L <-- [f(_, b), f(a, _)]]]]).
fileContents('setof', [setof(X,Y^((X=1,Y=1);(X=2,Y=2)),S), [[S <-- [1, 2]]]]).
fileContents('setof', [setof(X,Y^((X=1;Y=1);(X=2,Y=2)),S), [[S <-- [_, 1, 2]]]]).
fileContents('setof', [(set_prolog_flag(unknown, warning),
                     setof(X,(Y^(X=1;Y=1);X=3),S)), [[S <-- [3]]]]).
fileContents('setof', [(set_prolog_flag(unknown, warning),
                     setof(X,Y^(X=1;Y=1;X=3),S)), [[S <-- [_, 1,3]]]]).
fileContents('setof', [setof(X,(X=Y;X=Z;Y=1),L), [[L <-- [Y, Z]], [L <-- [_], Y <-- 1]]]).
fileContents('setof', [setof(X, X^(true; 4),L), type_error(callable,4)]).
fileContents('setof', [setof(X,1,L), type_error(callable,1)]).
fileContents('sub_atom', [sub_atom(abracadabra, 0, 5, _, S2), [[S2 <-- 'abrac']]]).
fileContents('sub_atom', [sub_atom(abracadabra, _, 5, 0, S2), [[S2 <-- 'dabra']]]).
fileContents('sub_atom', [sub_atom(abracadabra, 3, Length, 3, S2), [[Length <-- 5, S2 <-- 'acada']]]).
fileContents('sub_atom', [sub_atom(abracadabra, Before, 2, After, ab),
                     [[Before <-- 0, After <-- 9],
                     [Before <-- 7, After <-- 2]]]).
fileContents('sub_atom', [sub_atom('Banana', 3, 2, _, S2), [[S2 <-- 'an']]]).
fileContents('sub_atom', [sub_atom('charity', Before, 3, After, S2), 
		    [[Before <-- 0, After <-- 4, S2 <-- 'cha'],
                     [Before <-- 1, After <-- 3, S2 <-- 'har'],
                     [Before <-- 2, After <-- 2, S2 <-- 'ari'],
                     [Before <-- 3, After <-- 1, S2 <-- 'rit'],
                     [Before <-- 4, After <-- 0, S2 <-- 'ity']]]).
fileContents('sub_atom', [sub_atom('ab', Before, Length, After, Sub_atom),
                    [[Before <-- 0, Length <-- 0, After <-- 2, Sub_atom <-- ''],
                     [Before <-- 0, Length <-- 1, After <-- 1, Sub_atom <-- 'a'],
                     [Before <-- 0, Length <-- 2, After <-- 0, Sub_atom <-- 'ab'],
                     [Before <-- 1, Length <-- 0, After <-- 1, Sub_atom <-- ''],
                     [Before <-- 1, Length <-- 1, After <-- 0, Sub_atom <-- 'b'],
                     [Before <-- 2, Length <-- 0, After <-- 0, Sub_atom <-- '']]]).
fileContents('sub_atom', [sub_atom(Banana, 3, 2, _, S2), instantiation_error]).
fileContents('sub_atom', [sub_atom(f(a), 2, 2, _, S2), type_error(atom,f(a))]).
fileContents('sub_atom', [sub_atom('Banana', 4, 2, _, 2), type_error(atom,2)]).
fileContents('sub_atom', [sub_atom('Banana', a, 2, _, S2), type_error(integer,a)]).
fileContents('sub_atom', [sub_atom('Banana', 4, n, _, S2), type_error(integer,n)]).
fileContents('sub_atom', [sub_atom('Banana', 4, _, m, S2), type_error(integer,m)]).
fileContents('term_diff', ['\\=='(1,1), failure]).
fileContents('term_diff', ['\\=='(X,X), failure]).
fileContents('term_diff', ['\\=='(1,2), success]).
fileContents('term_diff', ['\\=='(X,1), success]).
fileContents('term_diff', ['\\=='(X,Y), success]).
fileContents('term_diff', ['\\=='(_,_), success]).
fileContents('term_diff', ['\\=='(X,a(X)), success]).
fileContents('term_diff', ['\\=='(f(a),f(a)), failure]).
fileContents('term_eq', ['=='(1,1), success]).
fileContents('term_eq', ['=='(X,X), success]).
fileContents('term_eq', ['=='(1,2), failure]).
fileContents('term_eq', ['=='(X,1), failure]).
fileContents('term_eq', ['=='(X,Y), failure]).
fileContents('term_eq', ['=='(_,_), failure]).
fileContents('term_eq', ['=='(X,a(X)), failure]).
fileContents('term_eq', ['=='(f(a),f(a)), success]).
fileContents('term_gt', ['@>'(1.0,1), failure]).
fileContents('term_gt', ['@>'(aardvark,zebra), failure]).
fileContents('term_gt', ['@>'(short,short), failure]).
fileContents('term_gt', ['@>'(short,shorter), failure]).
fileContents('term_gt', ['@>'(foo(b),foo(a)), success]).
fileContents('term_gt', ['@>'(X,X), failure]).
fileContents('term_gt', ['@>'(foo(a,X),foo(b,Y)), failure]).
fileContents('term_gt=', ['@>='(1.0,1), failure]) :- intAndFloatAreDifferent.
fileContents('term_gt=', ['@>='(aardvark,zebra), failure]).
fileContents('term_gt=', ['@>='(short,short), success]).
fileContents('term_gt=', ['@>='(short,shorter), failure]).
fileContents('term_gt=', ['@>='(foo(b),foo(a)), success]).
fileContents('term_gt=', ['@>='(X,X), success]).
fileContents('term_gt=', ['@>='(foo(a,X),foo(b,Y)), failure]).
fileContents('term_lt', ['@<'(1.0,1), success]) :- intAndFloatAreDifferent.
fileContents('term_lt', ['@<'(aardvark,zebra), success]).
fileContents('term_lt', ['@<'(short,short), failure]).
fileContents('term_lt', ['@<'(short,shorter), success]).
fileContents('term_lt', ['@<'(foo(b),foo(a)), failure]).
fileContents('term_lt', ['@<'(X,X), failure]).
fileContents('term_lt', ['@<'(foo(a,X),foo(b,Y)), success]).
fileContents('term_lt=', ['@=<'(1.0,1), success]).
fileContents('term_lt=', ['@=<'(aardvark,zebra), success]).
fileContents('term_lt=', ['@=<'(short,short), success]).
fileContents('term_lt=', ['@=<'(short,shorter), success]).
fileContents('term_lt=', ['@=<'(foo(b),foo(a)), failure]).
fileContents('term_lt=', ['@=<'(X,X), success]).
fileContents('term_lt=', ['@=<'(foo(a,X),foo(b,Y)), success]).
fileContents('true', [true, success]).
fileContents('unify', ['='(1,1), success]).
fileContents('unify', ['='(X,1),[[X <-- 1]]]).
/* Skip this test since the test suite doesn't check it properly. - jefft0
fileContents('unify', ['='(X,Y),[[Y <-- X]]]).
*/
fileContents('unify', [('='(X,Y),'='(X,abc)),[[X <-- abc, Y <-- abc]]]).
fileContents('unify', ['='(f(X,def),f(def,Y)), [[X <-- def, Y <-- def]]]).
fileContents('unify', ['='(1,2), failure]).
fileContents('unify', ['='(1,1.0), failure]) :- intAndFloatAreDifferent.
fileContents('unify', ['='(g(X),f(f(X))), failure]).
fileContents('unify', ['='(f(X,1),f(a(X))), failure]).
fileContents('unify', ['='(f(X,Y,X),f(a(X),a(Y),Y,2)), failure]).
fileContents('unify', ['='(f(A,B,C),f(g(B,B),g(C,C),g(D,D))),
                     [[A <-- g(g(g(D,D),g(D,D)),g(g(D,D),g(D,D))),
                     B <-- g(g(D,D),g(D,D)),
                     C <-- g(D,D)]]]).
