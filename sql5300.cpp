/**
 * Purpose: client portal for SQL5300 program
 *
 * @file sql5300.cpp
 * @authors Grant Bishop & Jon Kimray (Sprint Verano); 
 *          Alicia Mauldin & Vishakha Bhavsar (Sprint Otono)
 * @see "Seattle University, cpsc4300/5300, Summer 2019"
 */

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <cerrno>
#include "SQLParser.h"
#include "sqlhelper.h"
#include "db_cxx.h"
#include "heap_storage.h"

using namespace std;
using namespace hsql;

/**
 * Allocate and initialize the _DB_ENV global
 */
DbEnv* _DB_ENV;

/**
 * Converts all characters in a given string to lowercase.
 * @param word A string of characters to be converted to lowercase
 * @return string of all lowercase characters
 */
string toLowercase(string word);

/**
 * Helper function to assist in parsing operators to string
 * @param expr SQL expression operator(s) to parse
 * @return a string of the SQL expression operator(s)
 */
string operatorToString(const Expr *expr);

/**
 * Helper function to assist in parsing expressions to string
 * @param expr SQL expression(s) to parse
 * @return a string of the SQL expression(s)
 */
string expressionToString(const Expr *expr);

/**
 * Helper function to assist in parsing AST columns to string
 * @param col AST column to parse
 * @return a string of the SQL table column(s)
 */
string columnToString(const ColumnDefinition *col);

/**
 * Helper function to assist in parsing AST tables to string
 * @param table AST table to parse
 * @return a string of the SQL table
 */
string tableToString(const TableRef *table);

/**
 * Helper function to interpret create SQL command from AST
 * @param stmt CreateStatement AST to be executed
 * @return a string of the create SQL query
 */
string executeCreate(const CreateStatement *stmt);

/**
 * Helper function to interpret select SQL command from AST
 * @param stmt SelectStatement AST to be executed
 * @return a string of the select SQL query
 */
string executeSelect(const SelectStatement *stmt);

/**
 * Interprets create and select SQL statements into string text
 * @param stmt SQLParserResult AST to be executed
 * @return a string of the SQL query
 */
string execute(const SQLStatement *stmt);

/**
 * Main function of CPSC-5300 SQL string parser program
 */
int main(int argc, char* argv[]) {

    bool promptUser = true; // Flags if user wants to quit
    string query;           // Stores user query input


    // Check proper number of command arguments are provided
    if (argc != 2) {
        cerr << "Error: invalid command input.\nFormat is [sql5300: directory]\n";
        return 1;
    }

    // Create and open a database environment
    char *dbenv = argv[1];
    DbEnv myEnv(0U);
    try {
        myEnv.open(dbenv, DB_CREATE | DB_INIT_MPOOL, 0);
    } catch (DbException &e) {
        cerr << "Error: unable to open the " << dbenv << " database environment\n";
        cerr << e.what() << endl;
        return 1;
    } catch (exception &e) {
        cerr << "Error: unable to open the " << dbenv << " database environment\n";
        cerr << e.what() << endl;
        return 1;
    }

    // Print out confirmation of database environment
    cout << "(" << argv[0] << ": running with database environment at "
         << dbenv <<")" << endl;

    _DB_ENV = &myEnv;

    // Prompt user for SQL query input
    do {
        cout << "SQL>";
        getline(cin, query);

        // Continue prompting user if blank command given
        while (query.length() == 0) {
            cout << "SQL>";
            getline(cin, query);
        }

        // Exit if user enters quit command
        if (toLowercase(query) == "quit") {
            promptUser = false;
        } else if (toLowercase(query) == "test") {
            // Run built in tests of heap storage classes
            cout << "test_heap_storage: " << (test_heap_storage() ? "ok" : "failed") << endl;
        } else {
            // Parse the query command
            SQLParserResult *result = SQLParser::parseSQLString(query);

            // Execute the query command if it is valid
            if (result->isValid()) {
                for (unsigned i = 0; i < result->size(); i++) {
                    // Print out the query command in SQL
                    cout << execute(result->getStatement(i)) << endl;
                }
                delete result;
            } else {
                cout << "Error invalid SQL: " << query << endl;
                delete result;
            }
        }

    } while (promptUser);

    // Close the database environment
    try {
        myEnv.close(0);
    } catch(DbException &e) {
        cerr << "Error: unable to close the " << dbenv << " database environment\n";
        cerr << e.what() << endl;
        return 1;
    } catch (exception &e) {
        cerr << "Error: unable to close the " << dbenv << " database environment\n";
        cerr << e.what() << endl;
        return 1;
    }

    return 0;
}

// Converts all characters in a given string to lowercase
string toLowercase(string word) {
    string result;
    for(unsigned i = 0; i < word.length(); i++) {
        result += tolower(word[i]);
    }
    return result;
}

// Helper function to assist in parsing operators to string
string operatorToString(const Expr *expr) {
    // Check not null
    if (expr == NULL) {
        return "null";
    }

    string operatorResult;

    // Check for unary and binary operators
    if (expr->opType == Expr::NOT) {
        operatorResult += "NOT";
    }
    operatorResult += expressionToString(expr->expr) + " ";
    if (expr->opType == Expr::SIMPLE_OP) {
        operatorResult += expr->opChar;
    } else if (expr->opType == Expr::AND) {
        operatorResult += "AND";
    } else if (expr->opType == Expr::OR) {
        operatorResult += "OR";
    }
    if (expr->expr2 != NULL) {
        operatorResult += " " + expressionToString(expr->expr2);
    }
    return operatorResult;
}

// Helper function to assist in parsing expressions to string
string expressionToString(const Expr *expr) {
    string exprResult;
    switch (expr->type) {
        case kExprStar:
            exprResult += "*";
            break;
        case kExprColumnRef:
            if (expr->table != NULL) {
                exprResult += string(expr->table) + ".";
            }
        case kExprLiteralString:
            exprResult += expr->name;
            break;
        case kExprLiteralFloat:
            exprResult += to_string(expr->fval);
            break;
        case kExprLiteralInt:
            exprResult += to_string(expr->ival);
            break;
        case kExprFunctionRef:
            exprResult += string(expr->name) + "?" + expr->expr->name;
            break;
        case kExprOperator:
            exprResult += operatorToString(expr);
            break;
        default:
            exprResult += "unknown expression";
            break;
    }
    if (expr->alias != NULL) {
        exprResult += string(" AS ") + expr->alias;
    }
    return exprResult;
}

// Helper function to assist in parsing AST columns to string
string columnToString(const ColumnDefinition *col) {
    string columnList(col->name);
    if (col->type == ColumnDefinition::DOUBLE) {
        columnList += " DOUBLE";
    } else if (col->type == ColumnDefinition::TEXT) {
        columnList += " TEXT";
    } else if (col->type == ColumnDefinition::INT) {
        columnList += " INT";
    } else {
        columnList += "...";
    }
    return columnList;
}

// Helper function to assist in parsing AST tables to string
string tableToString(const TableRef *table) {
    string tableInfo;
    switch (table->type) {
        case kTableSelect:
            tableInfo += "Command not yet implemented";
            break;

            // Grab table name
        case kTableName:
            tableInfo += table->name;
            if (table->alias != NULL) {
                tableInfo += string(" AS ") + table->alias;
            }
            break;

            // Get join language for SQL join types
        case kTableJoin:
            tableInfo += tableToString(table->join->left);
            switch (table->join->type) {
                case kJoinCross:
                case kJoinInner:
                    tableInfo += " JOIN ";
                    break;
                case kJoinOuter:
                case kJoinLeftOuter:
                case kJoinLeft:
                    tableInfo += " LEFT JOIN ";
                    break;
                case kJoinRightOuter:
                case kJoinRight:
                    tableInfo += " RIGHT JOIN  ";
                    break;
                case kJoinNatural:
                    tableInfo += " NATURAL JOIN ";
                    break;
            }
            tableInfo += tableToString(table->join->right);

            // Bring in any expressions for joins
            if (table->join->condition != NULL) {
                tableInfo += " ON " + expressionToString(table->join->condition);
            }
            break;

            // If product is called on tables
        case kTableCrossProduct:
            bool comma = false;
            for (TableRef* t : *table->list) {
                if (comma) {
                    tableInfo += ", ";
                }
                tableInfo += tableToString(t);
                comma = true;
            }
            break;
    }

    return tableInfo;
}

// Helper function to interpret create SQL command from AST
string executeCreate(const CreateStatement *stmt) {
    string createResult = "CREATE TABLE ";

    // If AST type is not a table, return empty
    if (stmt->type != CreateStatement::kTable) {
        return createResult += "...";
    }

    // Append IF NOT EXISTS flag if present in AST
    if (stmt->ifNotExists) {
        createResult += "IF NOT EXISTS ";
    }

    // Append column(s) to return
    createResult += string(stmt->tableName) + " (";
    bool comma = false;
    for(ColumnDefinition *col : *stmt->columns) {
        if (comma) {
            createResult += ", ";
        }
        createResult += columnToString(col);
        comma = true;
    }
    createResult += ")";
    return createResult;
}

// Helper function to interpret select SQL command from AST
string executeSelect(const SelectStatement *stmt) {
    string selectResult = "SELECT ";

    // Append expression(s) to return
    bool comma = false;
    for(Expr* expr : *stmt->selectList) {
        if (comma) {
            selectResult += ", ";
        }
        selectResult += expressionToString(expr);
        comma = true;
    }
    selectResult += " FROM " + tableToString(stmt->fromTable);
    if (stmt->whereClause != NULL) {
        selectResult += " WHERE " + expressionToString(stmt->whereClause);
    }
    return selectResult;
}

// Interprets create and select SQL statements into string text
string execute(const SQLStatement *stmt) {
    if (stmt->type() == kStmtSelect) {
        return executeSelect((const SelectStatement*) stmt);
    } else if (stmt->type() == kStmtCreate) {
        return executeCreate((const CreateStatement*) stmt);
    } else {
        return "Command not yet implemented";
    }
}
