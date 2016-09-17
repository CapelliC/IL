%-----------------------------------------------
% play: program 18.8
%	two players game playing framework
%
% from Sterling and Shapiro - The art of Prolog
%	page 297
%

play(Game) :-
	initialize(Game, Position, Player),
	display_game(Position, Player),
	play(Position, Player,_Result).

play(Position, Player, Result) :-
	game_over(Position, Player, Result), !, announce(Result).
play(Position, Player, Result) :-
	choose_move(Position, Player, Move),
	move(Move, Position, Position1),
	display_game(Position1, Player),
	next_player(Player, Player1),
	!, play(Position1, Player1, Result).
