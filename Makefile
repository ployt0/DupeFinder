INC=-Iinclude

dupeFinder: main.o dupeFinderP.o sha256.o
	g++ -O3 -static main.o dupeFinderP.o sha256.o -o dupeFinder

main.o: main.cpp
	g++ $(INC) -c main.cpp

sha256.o: src/sha256.cpp
	g++ -c -O3 src/sha256.cpp

dupeFinderP.o: src/dupeFinder.cpp sha256.o
	g++ $(INC) -c -O3 src/dupeFinder.cpp -o dupeFinderP.o

tests: src/dupeFinder.cpp catch2 tests.o sha256.o
	g++ $(INC) -c src/dupeFinder.cpp --coverage
	g++ tests.o dupeFinder.o sha256.o libCatch2Main.a libCatch2.a --coverage -o tests
	./tests

check-coverage: tests
	gcov dupeFinder.cpp -r

tests.o: tests.cpp
	g++ $(INC) -ICatch2/src -c -O3 tests.cpp


cleancatch:
	rm -fr Catch2

catch2:
	git clone https://github.com/catchorg/Catch2.git
	cd Catch2 && cmake -Bbuild -G"Unix Makefiles" -H. -DBUILD_TESTING=OFF
	cd Catch2/build && make
	cp Catch2/build/src/libCatch2Main.a .
	cp Catch2/build/src/libCatch2.a .
	touch Catch2/src/catch2/catch_user_config.hpp



# catch_amalgamated.o: src/catch_amalgamated.cpp
# 	g++ $(INC) -c src/catch_amalgamated.cpp

# catch2.o: catch2/catch_test_macros.cpp
# 	g++ -Icatch2 -c catch2/catch_test_macros.cpp



deepclean:
	rm *.o dupeFinder *.gcov

clean:
	find ./ -name "*.o" | grep -v "catch_amalgamated.o" | xargs rm || echo No objects to delete
	rm *.exe
	rm dupeFinder || rm dupeFinder.exe || echo No executable to delete
	rm *.gcov *.gcno *gcda || echo No coverage reports to delete