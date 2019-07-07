
DB_INCLUDES = /usr/local/db6/include
DB_LIBRARIES = /usr/local/db6/lib
OWN_DIR = ./

COMPILED_OB = FAKEmain.o Berkeley.o Execute.o

#TODO don't actually nead every library / header and -l options (db_cxx, sqlparser) for every file,
# e.g. main has no db_cxx dependency

%.o: %.cpp
	g++ -I$(DB_INCLUDES) -c -O -std=c++11 -Wall -pedantic -DHAVE_CXX_STDHEADERS -D_GNU_SOURCE -D_REENTRANT -o "$@" "$<"

sql5300: $(COMPILED_OB)
	g++ -L$(DB_LIBRARIES) -L$(OWN_DIR) -ldb_cxx -lsqlparser -o $@ $^

clean:
	rm -f *.o
