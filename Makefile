SHELL := /bin/bash

buildifier:
	buildifier BUILD;\
	for i in **/BUILD; do buildifier $$i; done;\
	buildifier WORKSPACE;\
