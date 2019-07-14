// "Project Butterfly" - CPSC 5300 / 4300 Summer 2019                                                  
// See README.md for details 
// THIS FILE: Program entry point: set up DB environemnt then go to SQL prompt

#include <iostream>
#include <string>

#include "db_cxx.h" // Berkeley
#include "SQLParser.h" // Hyrise
#include "SQLParserResult.h"

#include "Execute.h" // "fake" query execution: functions for unparsing queries

#include "heap_storage.h"

// Global setup parameters
const unsigned int DB_BLOCK_SZ = 4096; // Don't change name!
const char* DB_FILENAME = "Butterfly_1.db"; // Hardcoded filename!

// Berkeley DB enviroment global pointer
// Need for test_heap_storage to get environment
DbEnv* _DB_ENV;

int main(int argc, char* argv[]) {

	// Get filepath for db file from command line argument
    std::string filepath = "";
	switch(argc) {
	case 1:
		std::cout << "Error: please specifiy database file path" << std::endl;
		return -1;
	case 2:
		filepath = argv[1];
		break;
	case 3:
		std::cout << "Error: too many arguments" << std::endl;
		return -1;
	}

    // Set up Berkeley DB

    // DB environment
    DbEnv dbEnvironment(0U);
    try { 
        dbEnvironment.open(filepath.c_str(), DB_CREATE | DB_INIT_MPOOL, 0);
    }
    catch(DbException& e) {
        std::cerr << "Failure to open DB environment: " << e.what() << std::endl;
        return -1;
    }
    catch(std::exception& e) { // BerkeleyDB can also throw std::exception 
        std::cerr << "std::exception failure: " << e.what() << std::endl;
        return -1;
    }
    _DB_ENV = &dbEnvironment;


    // Main SQL prompt loop
    std::string sql;
    do{ 
      sql = "";
      std::cout << "SQL> ";
      std::getline(std::cin,sql); // not cin >> because need to get including spaces!
      if(sql == "test") {
        std::cout << "test with test_heap_storage(): " << std::endl;
        bool pass = test_heap_storage();
        std::cout << std::endl;
        std::cout << "Test result: ";
        if(pass)
            std::cout << "pass";
        else
            std::cout << "fail";
        std::cout << std::endl;
      }
      hsql::SQLParserResult* result;
      result = hsql::SQLParser::parseSQLString(sql);
      if(result->isValid())
        std::cout << Execute::getString(result) << std::endl;
      else
        std::cout << "Failed to unparse" << std::endl;
      delete result;

    } while(sql != "quit");
	return 0;
}
