// Main for ButterflyShell
// (early incomplete version)

#include <iostream>
#include <string.h> // TODO .h ?
#include <exception>

#include <ctime> // Also, shouldn't mix ctime and chrono like this .... ?

#include "db_cxx.h" // Build env set up needed to find this ... TODO Makefile to find this ...
#include "SQLParser.h" // TODO Makefile
#include "SQLParserResult.h" // TODO same as above
// Global setup parameters
const unsigned int DB_BLOCK_SIZE = 4096;
const char* DB_FILENAME = "Butterfly_1.db";


int main(int argc, char* argv[]) {

	// Get filepath for db file from command line argument
	std::string filepath;
	switch(argc) {
	case 1:
		std::cout << "Error: please specifiy database file path" << std::endl;
		return -1;
	case 2:
		filepath = argv[1];
		break;
	case 3:
		std::cout << "Error: unxecpected arguments" << std::endl;
		return -1;
	}


    // TODO Create a Berkeley DB environment in that directory using DbEnv::open with the DB_CREATE flag

    // TODO Have a user-input (do-while) loop prompting the user with "SQL> "
    std::string sql;
    do{
      std::cout << "SQL> ";
      std::cin >> sql;
      SQLParser::SQLParserResult* result;
      result = SQLParser::parseSQLString(sql);
      result->isValid();
      // TODO pass to "execute" and print resulting string to console
    } while(sql != "quit")
	return 0;

}
