lookup(Key, [(Key, Value) | _Dict], Value).
lookup(Key, [(Key1, _Val1) | Dictionary], Value) :-
	Key \= Key1, lookup(Key, Dictionary, Value).
