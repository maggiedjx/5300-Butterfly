// Main for ButterflyShell
// (early incomplete version)

#include <iostream>
#include <string.h> // TODO .h ?
#include <exception>

#include <ctime> // Also, shouldn't mix ctime and chrono like this .... ?

#include "db_cxx.h" // Build env set up needed to find this ... TODO Makefile to find this ...

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


	// TODO check ahead of time if entered dir is valid?  Easay way without c++17 filesystem .... ?
	// Otherwise just assume it will work then catch exceptions when it dosn't

	// Open the enviroment and the (single, for now) database
	// For opening environemnt and db use the DB_CREATE flag to create if no previous one exists
	// https://docs.oracle.com/cd/E17076_05/html/gsg/CXX/databases.html#DBOpen
	
	// Open the database environment
	// https://docs.oracle.com/cd/E17076_05/html/gsg/CXX/CoreEnvUsage.html
	// DB_INIT_MPOOL is to "initialize the in-memory cache"
	DbEnv dbEnvironment(0U); // Needs 0 unsigned paramater - why *see doc above*
	try {
		dbEnvironment.open(filepath.c_str(), DB_CREATE | DB_INIT_MPOOL, 0);
	}
	// May throw DbException errors:
	// https://docs.oracle.com/cd/E17076_05/html/gsg/CXX/coreExceptions.html
	catch(DbException &excep) {
		std::cout << "Failure opening database environment at " << filepath << std::endl;
		std::cout << "BerkelyDB exception: " << excep.get_errno() << ": " << excep.what() << std::endl;
		return -1;
	}
	// note that may return std::exception dervied error as well as DbException errors
	catch(std::exception &excep) {
		std::cout << "Failure opening database environment at " << filepath << std::endl;
		std::cout << "Other error: " << excep.what() << std::endl;
		return -1;
	};

	// Open the database
	Db database(&dbEnvironment,0);
	database.set_re_len(DB_BLOCK_SIZE); // Record length as per Kevin's example
	try {
		database.open(NULL,DB_FILENAME,NULL, DB_RECNO, DB_CREATE, 0644); // Just using the octal 644 flag for user permisions from Kevin's example - what is the reasoning behing this - why is the default 0 not good enough?
	}
	catch(DbException &excep) {
		std::cout << "Failure opening database file at " << filepath << "/" << DB_FILENAME << std::endl;
		std::cout << "BerkelyDB exception: " << excep.get_errno() << ": " << excep.what() << std::endl;
		return -1;
	}
	catch(std::exception &excep) {
		std::cout << "Failure opening database file at " << filepath << "/" << DB_FILENAME << std::endl;
		std::cout << "Other error: " << excep.what() << std::endl;
		return -1;
	}
	// TODO: some violation of the DRY principle here - any easy way to have both environment and 
	// database open inside same try/catch, without placing db on the heap?		
	

	
	// TODO	
	
	// Main loop: get user input, process querry, ask for next input
	// do { }while(...)

	// Calls to shell.cpp ... ?



	// TODO any heap to clean up ...?

	return 0;

}
