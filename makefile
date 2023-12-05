build:
	cd ./src &&\
	make &&\
	cp libso_loader.so ./../test
run_test: build
	cd ./src;\
	make -f Makefile.example;\
	LD_LIBRARY_PATH=. ./so_exec so_test_prog > ../result.out
run: build
	cd ./test &&\
	make -f Makefile.checker > ../results.out
run_test_no:
	cd ./test &&\
	_test/run_test.sh init &&\
	_test/run_test.sh 5 &&\
	_test/run_test.sh clean
save: clean
	git add -A	&&\
	echo Enter commit message: &&\
	read line &&\
	git commit -m "$$line" &&\
	git push
clean: clean_src clean_test

clean_test:
	cd ./test;\
	rm -f libso_loader.so;\
	make -f Makefile.checker clean

clean_src:
	cd ./src;\
	rm -f *.o so_exec so_test_prog libso_loader.so ../results.out ../results.out