
CFLAGS=-Wall -Werror -Wextra -g -std=c11
GCOVFLAGS = --coverage
LIBS=-lcheck
#TEST_LIBS = -lcheck -lm -lpthread
OS = $(shell uname)
#TEST_FILE_NAME = test.c

ifeq ($(OS), Darwin)
	TEST_LIBS = -lcheck -lm -lpthread
else
	TEST_LIBS=-lcheck -lsubunit -pthread -lrt -lm
endif
TEST_FUNC = c/parse_obj.c c/transformations.c
all: clean 3dViewer.a #gcov_report

3dViewer.a:

	$(CC) $(CFLAGS) -c $(TEST_FUNC)
	ar rc 3dViewer.a *.o
	ranlib 3dViewer.a

test: 3dViewer.a
	$(CC) $(CFLAGS) -c -g c/test.c -o test_calc.o 
	$(CC) test_calc.o 3dViewer.a $(TEST_LIBS) -o 3dViewer
	./3dViewer

gcov_report: test
	$(CC) $(CFLAGS) $(GCOVFLAGS) c/parse_obj.c c/transformations.c c/test.c 3dViewer.a $(TEST_LIBS) -o gcov_test
	rm -rf u*.gcno
	rm -rf u*.gcda
	rm -rf u*.gcov
	chmod +x *
	./gcov_test
	gcov s21_*.gcda
	rm -rf u*.gcda
	gcovr -b
	gcovr
	gcovr --html-details -o report.html
	lcov -t "test"  -o report.info -c -d .
	genhtml -o report report.info
	open ./report/index.html

clean:
	rm -rf 3dViewer
	rm -rf *.o
	rm -rf *.a
	rm -rf *.gcno
	rm -rf *.gcda
	rm -rf *.gcov
	rm -rf *.info
	rm -rf *.html
	rm -rf *.css
	rm -rf test
	rm -rf gcov_test
	rm -rf *.dSYM
	rm -rf RESULT_VALGRIND.txt
	rm -rf report
style:
	clang-format -style=google -n c/*.c
	clang-format -style=google -n c/*.h
	clang-format -style=google -n cpp/*.h
	clang-format -style=google -n cpp/*.cpp
style_i:
	clang-format -style=google -i c/*.c
	clang-format -style=google -i c/*.h
	clang-format -style=google -i cpp/*.h
	clang-format -style=google -i cpp/*.cpp
valgrind: clean
		$(CC) -c $(TEST_FUNC)
	ar rc 3dViewer.a *.o
	ranlib 3dViewer.a
	$(CC) -c -g c/test.c -o test_calc.o
	$(CC) test_calc.o 3dViewer.a $(TEST_LIBS) -o 3dViewer
	./3dViewer
	CK=FORK=no valgrind --trace-children=yes --track-origins=yes --show-leak-kinds=all --verbose --log-file=RESULT_VALGRIND.txt ./3dViewer
lik: 3dViewer.a
	CK_FORK=no leaks -atExit -- ./3dViewer

install:
	mkdir build
	cd build
	cp -rf 3dViewer_v1-0.pro build
	cp -rf c build
	cp -rf cpp build
	cp -rf qtGifImage build
	cp -rf ui build
	# cp -rf resource_files build
	cd build && qmake 3dViewer_v1-0.pro
	make -C build
	cd build && make clean

uninstall:
	rm -rf build
	make clean
open:
	cd build && open 3dViewer_v1-0.app
dist:
	rm -rf Archive_SmartCalc_v1.0/
	mkdir Archive_SmartCalc_v1.0/
	mkdir Archive_SmartCalc_v1.0/src
	mv ./build/SmartCalc.app Archive_SmartCalc_v1.0/src/
	tar cvzf Archive_SmartCalc_v1.0.tgz Archive_SmartCalc_v1.0/
	rm -rf Archive_SmartCalc_v1.0/
rebuild: clean all

dvi: description.md
ifeq ($(OS), Darwin)
	open description.md
else
	cat description.md
endif

