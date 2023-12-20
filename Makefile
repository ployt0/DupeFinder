INC=-Iinclude

dupeFinder: main.o dupeFinderP.o sha256.o
	g++ -O3 -static main.o dupeFinderP.o sha256.o -o dupeFinder

main.o: main.cpp
	g++ $(INC) -c main.cpp

sha256.o: src/sha256.cpp
	g++ -c -O3 src/sha256.cpp

dupeFinderP.o: src/dupeFinder.cpp sha256.o
	g++ $(INC) -c -O3 src/dupeFinder.cpp -o dupeFinderP.o

tests: catch_amalgamated.o src/dupeFinder.cpp tests.o sha256.o
	g++ $(INC) -c src/dupeFinder.cpp --coverage
	g++ tests.o dupeFinder.o sha256.o catch_amalgamated.o --coverage -o tests
	./tests

check-coverage: tests
	gcov dupeFinder.cpp -r

tests.o: tests.cpp
	g++ $(INC) -ICatch2/src -c -O3 tests.cpp

catch_amalgamated.o: src/catch_amalgamated.cpp
	g++ $(INC) -c src/catch_amalgamated.cpp

# catch2.o: catch2/catch_test_macros.cpp
# 	g++ -Icatch2 -c catch2/catch_test_macros.cpp

deepclean:
	rm *.o dupeFinder *.gcov

clean:
	find ./ -name "*.o" | grep -v "catch_amalgamated.o" | xargs rm || echo No objects to delete
	rm *.exe
	rm dupeFinder || rm dupeFinder.exe || echo No executable to delete
	rm *.gcov *.gcno *gcda || echo No coverage reports to delete