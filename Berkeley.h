// TODO project header here


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
	Berkeley(sts::string path);


	// Write data (as a string) to the given block in the DB file
	// block: block number to write too
	// data: data to write to block TODO: will want to switch to "bit strings" (char* ?) instead of std::string ?
	// returns: void TODO return false if fail ? What is maximum data length -> associated with block size, 1:1 ?
	void Write(int block, std::string data);

	// Read data (a string) from the given block in the DB file
	// block: block number to read from
	// returns: string of data from that block
	std::string Read(int block);

private:

	// fixed parameters
	static const int DB_BLOCK_SIZE = 4096;

};
