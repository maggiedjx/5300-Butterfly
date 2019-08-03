/**
<<<<<<< HEAD
 * Milestone 3 & 4
=======
>>>>>>> Standardizing files for Milestone 6
 * @file sql5300.cpp - main entry for the relation manaager's SQL shell
 * @author Kevin Lundeen
 * @see "Seattle University, cpsc4300/5300, summer 2018"
 */
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <string>
#include <cassert>
#include "db_cxx.h"
#include "SQLParser.h"
#include "ParseTreeToString.h"
#include "SQLExec.h"
<<<<<<< HEAD

=======
>>>>>>> Standardizing files for Milestone 6
using namespace std;
using namespace hsql;

/*
 * we allocate and initialize the _DB_ENV global
 */
void initialize_environment(char *envHome);

<<<<<<< HEAD
/*
 * allows testing of pre-canned queries
 */
void run_test(string test[], int size);


// List of test queries for passing Milestone 3 requirements
string test_queries_ms3[] = {
    "show tables",
    "show columns from _tables",
    "show columns from _columns",
    "create table foo (id int, data text, x integer, y integer, z integer)",
    "create table foo (goober int)", // should throw error
    "create table goo (x int, x text)", // should throw error
    "show tables",
    "show columns from foo",
    "drop table foo",
    "show tables",
    "show columns from foo"
};

// List of test queries for passing Milestone 4 requirements
string test_queries_ms4[] = {
    "create table goober (x int, y int, z int)",
    "show tables",
    "show columns from goober",
    "create index fx on goober (x,y)",
    "show index from goober",
    "drop index fx from goober",
    "show index from goober",
    "create index fx on goober (x)",
    "show index from goober",
    "create index fx on goober (y,z)", // should throw error
    "show index from goober",
    "create index fyz on goober (y,z)",
    "show index from goober",
    "drop index fx from goober",
    "show index from goober",
    "drop index fyz from goober",
    "show index from goober",
    "drop table goober"
};
=======
>>>>>>> Standardizing files for Milestone 6

/**
 * Main entry point of the sql5300 program
 * @args dbenvpath  the path to the BerkeleyDB database environment
 */
int main(int argc, char *argv[]) {

<<<<<<< HEAD
    // Open/create the db environment
    if (argc != 2) {
        cerr << "Usage: cpsc5300: dbenvpath" << endl;
        return 1;
    }
    initialize_environment(argv[1]);

    // Enter the SQL shell loop
    while (true) {
        cout << "SQL> ";
        string query;
        getline(cin, query);
        if (query.length() == 0)
            continue;  // blank line -- just skip
        if (query == "quit")
            break;  // only way to get out
        if (query == "test") {
            cout << "test_heap_storage: " << (test_heap_storage() ? "ok" : "failed") << endl;
            continue;
        }
        if (query == "test ms3") {
            int size_ms3 = sizeof(test_queries_ms3)/sizeof(test_queries_ms3[0]);
            run_test(test_queries_ms3, size_ms3);
            continue;
        }
        if (query == "test ms4") {
            int size_ms4 = sizeof(test_queries_ms4)/sizeof(test_queries_ms4[0]);
            run_test(test_queries_ms4, size_ms4);
            continue;
        }

        // parse and execute
        SQLParserResult* parse = SQLParser::parseSQLString(query);
        if (!parse->isValid()) {
            cout << "invalid SQL: " << query << endl;
            cout << parse->errorMsg() << endl;
        } else {
            for (uint i = 0; i < parse->size(); ++i) {
                const SQLStatement *statement = parse->getStatement(i);
                try {
                    cout << ParseTreeToString::statement(statement) << endl;
                    QueryResult *result = SQLExec::execute(statement);
                    cout << *result << endl;
                    delete result;
                } catch (SQLExecError& e) {
                    cout << "Error: " << e.what() << endl;
                }
            }
        }
        delete parse;
    }
    return EXIT_SUCCESS;
=======
	// Open/create the db enviroment
	if (argc != 2) {
		cerr << "Usage: cpsc5300: dbenvpath" << endl;
		return 1;
	}
	initialize_environment(argv[1]);

	// Enter the SQL shell loop
	while (true) {
		cout << "SQL> ";
		string query;
		getline(cin, query);
		if (query.length() == 0)
			continue;  // blank line -- just skip
		if (query == "quit")
			break;  // only way to get out
		if (query == "test") {
			cout << "test_heap_storage: " << (test_heap_storage() ? "ok" : "failed") << endl;
			continue;
		}

		// parse and execute
		SQLParserResult* parse = SQLParser::parseSQLString(query);
		if (!parse->isValid()) {
			cout << "invalid SQL: " << query << endl;
			cout << parse->errorMsg() << endl;
		} else {
			for (uint i = 0; i < parse->size(); ++i) {
				const SQLStatement *statement = parse->getStatement(i);
				try {
					cout << ParseTreeToString::statement(statement) << endl;
					QueryResult *result = SQLExec::execute(statement);
					cout << *result << endl;
					delete result;
				} catch (SQLExecError& e) {
					cout << "Error: " << e.what() << endl;
				}
			}
		}
		delete parse;
	}
	return EXIT_SUCCESS;
>>>>>>> Standardizing files for Milestone 6
}

DbEnv *_DB_ENV;
void initialize_environment(char *envHome) {
<<<<<<< HEAD
    cout << "(sql5300: running with database environment at " << envHome
         << ")" << endl;

    DbEnv *env = new DbEnv(0U);
    env->set_message_stream(&cout);
    env->set_error_stream(&cerr);
    try {
        env->open(envHome, DB_CREATE | DB_INIT_MPOOL, 0);
    } catch (DbException &exc) {
        cerr << "(sql5300: " << exc.what() << ")" << endl;
        exit(1);
    }
    _DB_ENV = env;
    initialize_schema_tables();
}

void run_test(string test[], int size) {
    for (int i = 0; i < size; i++) {
        cout << endl;
        // Print test query for user to see
        cout << "Query " << i + 1 << ": " << test[i] << endl;
        SQLParserResult* q_result = SQLParser::parseSQLString(test[i]);

        // Ensure validity and print result
        if (!q_result->isValid()) {
            cout << "invalid SQL: " << test[i] << endl;
            cout << q_result->errorMsg() << endl;
        } else {
            for (uint j = 0; j < q_result->size(); j++) {
                const SQLStatement* q_statement = q_result->getStatement(j);
                try {
                    cout << ParseTreeToString::statement(q_statement) << endl;
                    QueryResult* test_result = SQLExec::execute(q_statement);
                    cout << *test_result << endl;
                    delete test_result;
                } catch (SQLExecError& e) {
                    cout << "Error: " << e.what() << endl;
                }
            }
        }
        delete q_result;
    }
    cout << endl;
=======
	cout << "(sql5300: running with database environment at " << envHome
		 << ")" << endl;

	DbEnv *env = new DbEnv(0U);
	env->set_message_stream(&cout);
	env->set_error_stream(&cerr);
	try {
		env->open(envHome, DB_CREATE | DB_INIT_MPOOL, 0);
	} catch (DbException &exc) {
		cerr << "(sql5300: " << exc.what() << ")" << endl;
		exit(1);
	}
	_DB_ENV = env;
	initialize_schema_tables();
>>>>>>> Standardizing files for Milestone 6
}
