#include "Berkeley.h"

const std::string Berkeley::DB_FILENAME = "ButterflyDB.db";
const int Berkeley::DB_BLOCK_SIZE = 4096;



// TODO: suppress error (cerr (?) output - possible / desired?)
// Note use of member intializer list as DbEnv and Db can't be assigned with =
Berkeley::Berkeley(std::string filepath) : 
	dbEnvironment(0U), database(&dbEnvironment,0) {

	// Open the enviroment and the (single, for now) database
	// For opening environemnt and db use the DB_CREATE flag to create if no previous one exists
	// https://docs.oracle.com/cd/E17076_05/html/gsg/CXX/databases.html#DBOpen
	
	// Open the database environment
	// https://docs.oracle.com/cd/E17076_05/html/gsg/CXX/CoreEnvUsage.html
	// DB_INIT_MPOOL is to "initialize the in-memory cache"
	// dbEnvironment = DbEnv(0U); // Needs 0 unsigned paramater - why *see doc above* TODO
	try {
		dbEnvironment.open(filepath.c_str(), DB_CREATE | DB_INIT_MPOOL, 0);
	}
	// May throw DbException errors:
	// https://docs.oracle.com/cd/E17076_05/html/gsg/CXX/coreExceptions.html
	catch(DbException &excep) {
		dbErrors =  "Failure opening database environment at " + filepath + "\n" + 
		"BerkelyDB exception: " + std::to_string(excep.get_errno()) +  ": " + excep.what() + "\n";
		dbIsOk = false;
		return;
	}
	// note that may return std::exception dervied error as well as DbException errors
	catch(std::exception &excep) {
		dbErrors =  "Failure opening database environment at " + filepath + "\n" + 
		"Other exception: " + excep.what() + "\n";
		dbIsOk = false;
		return;
	}

	// Open the database
	// Db database(&dbEnvironment,0); // TODO
	database.set_re_len(DB_BLOCK_SIZE); // Record length as per Kevin's example
	try {
		database.open(NULL,DB_FILENAME.c_str(),NULL, DB_RECNO, DB_CREATE, 0644); // Just using the octal 644 flag for user permisions from Kevin's example - what is the reasoning behing this - why is the default 0 not good enough?
	}
	catch(DbException &excep) {
		dbErrors =  "Failure opening database file at " + filepath + "/" + DB_FILENAME + "\n" + 
		"BerkelyDB exception: " + std::to_string(excep.get_errno()) +  ": " + excep.what() + "\n";
		dbIsOk = false;
		return;

	}
	catch(std::exception &excep) {
		dbErrors =  "Failure opening database file at " + filepath + "/" + DB_FILENAME + "\n" + 
		"Other exception: " + excep.what() + "\n";
		dbIsOk = false;
		return;
	}


}
