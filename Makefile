dupeFinder: main.o dupeFinderP.o sha256.o
	g++ -O3 -static main.o dupeFinderP.o sha256.o -o dupeFinder

main.o: main.cpp
	g++ -c main.cpp

sha256.o: sha256.cpp
	g++ -c -O3 sha256.cpp

dupeFinderP.o: dupeFinder.cpp sha256.o
	g++ -c -O3 dupeFinder.cpp -o dupeFinderP.o

tests: catch_amalgamated.o dupeFinder.cpp tests.o sha256.o
	g++ -c dupeFinder.cpp --coverage
	g++ tests.o dupeFinder.o sha256.o catch_amalgamated.o --coverage -o tests
	./tests

check-coverage: tests
	gcov dupeFinder.cpp -r

tests.o: tests.cpp
	g++ -c -O3 tests.cpp

catch_amalgamated.o: catch_amalgamated.cpp
	g++ -c catch_amalgamated.cpp

deepclean:
	rm *.o dupeFinder *.gcov

clean:
	find ./ -name "*.o" | grep -v "catch_amalgamated.o" | xargs rm || echo No objects to delete
	rm *.exe
	rm dupeFinder || rm dupeFinder.exe || echo No executable to delete
	rm *.gcov *.gcno *gcda || echo No coverage reports to delete