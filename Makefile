
DB_INCLUDES = /usr/local/db6/include
DB_LIBRARIES = /usr/local/db6/lib

COMPILED_OB = main.o Berkeley.o Execute.o

%.o: %.cpp
	g++ -I$(DB_INCLUDES) -c -O -std=c++11 -Wall -pedantic -DHAVE_CXX_STDHEADERS -D_GNU_SOURCE -D_REENTRANT -o "$@" "$<"

sql5300: $(COMPILED_OB)
	g++ -L$(DB_LIBRARIES) -ldb_cxx -lsqlparser -o $@ $<

clean:
	rm -f *.o
