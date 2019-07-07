// TODO project header here

#ifndef BERKELEY_H
#define BERKELEY_H

#include <string>

// Berkely DB - linking set up in makefile
#include "db_cxx.h"

// Class to provide accsess to storage with the BerkeleyDB library
// Wraps BerkeleyDB in a more OO interface
class Berkeley {
public:

	// Constructor opens a BerkeleyDB database environment (directory) and file
	// Creates new one if not found
	// path: directory for database environment
	Berkeley(std::string path);

	// Destructor closes DB
	~Berkeley();


	// Write data (as a string) to the given block in the DB file
	// block: block number to write too
	// data: data to write to block TODO: will want to switch to "bit strings" (char* ?) instead of std::string ?
	// returns: void TODO return false if fail ? What is maximum data length -> associated with block size, 1:1 ?
	void Write(int block, std::string data);

	// Read data (a string) from the given block in the DB file
	// block: block number to read from
	// returns: string of data from that block
	std::string Read(int block);


	// db set up error flag and string  TODO better way to do this
	bool dbIsOk;
	std::string dbErrors;


private:

	// BerkeleyDB objects
	DbEnv dbEnvironment;
	Db database;

	// fixed parameters
	static const int DB_BLOCK_SIZE;
	static const std::string DB_FILENAME;
};

#endif
