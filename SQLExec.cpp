/**
 * Milestone 3
 * @file SQLExec.cpp - Milestone 3: Implementation of SQLExec class 
 * @authors Kevin Lundeen, Alicia Mauldin, Vishakha Bhavsar
 * @see "Seattle University, CPSC5300, Summer 2019"
 */
 
#include "SQLExec.h"
using namespace std;
using namespace hsql;

// Define static data
Tables* SQLExec::tables = nullptr;
Indices* SQLExec::indices = nullptr; //for indices implementation


// Make query result be printable
ostream &operator<<(ostream &out, const QueryResult &qres) {
    if (qres.column_names != nullptr) {
        for (auto const &column_name: *qres.column_names)
            out << column_name << " ";
        out << endl << "+";
        for (unsigned int i = 0; i < qres.column_names->size(); i++)
            out << "----------+";
        out << endl;
        for (auto const &row: *qres.rows) {
            for (auto const &column_name: *qres.column_names) {
                Value value = row->at(column_name);
                switch (value.data_type) {
                    case ColumnAttribute::INT:
                        out << value.n;
                        break;
                    case ColumnAttribute::TEXT:
                        out << "\"" << value.s << "\"";
                        break;
		    case ColumnAttribute::BOOLEAN:
                        out << (value.n == 0 ? "false" : "true");
                        break;
                    default:
                        out << "???";
                }
                out << " ";
            }
            out << endl;
        }
    }
    out << qres.message;
    return out;
}

// QueryResult dtor clears up non-null class pointers
QueryResult::~QueryResult() {
    if (column_names != nullptr)
        delete column_names;
    if (column_attributes != nullptr)
        delete column_attributes;
    if (rows != nullptr) {
        for (auto row: *rows)
            delete row;
        delete rows;
    }
}

// Intakes a SQL statement and sets the initial table if not already present
QueryResult *SQLExec::execute(const SQLStatement *statement) throw(SQLExecError) {
    // initialize _tables table, if not yet present
    if (SQLExec::tables == nullptr)
        SQLExec::tables = new Tables();

    try {
        switch (statement->type()) {
            case kStmtCreate:
                return create((const CreateStatement *) statement);
            case kStmtDrop:
                return drop((const DropStatement *) statement);
            case kStmtShow:
                return show((const ShowStatement *) statement);
            default:
                return new QueryResult("not implemented");
        }
    } catch (DbRelationError& e) {
        throw SQLExecError(string("DbRelationError: ") + e.what());
    }
}

// Intakes column info and sets appropriate data type
// Only supports types INT, TEXT, and DOUBLE for now
void SQLExec::column_definition(const ColumnDefinition *col, Identifier& column_name,
                                ColumnAttribute& column_attribute) {
    column_name = col->name;
    switch (col->type) {
        case ColumnDefinition::INT:
            column_attribute.set_data_type(ColumnAttribute::INT);
            break;
        case ColumnDefinition::TEXT:
            column_attribute.set_data_type(ColumnAttribute::TEXT);
            break;
        case ColumnDefinition::DOUBLE:
        default:
            throw SQLExecError("unrecognized data type");
    }
}

// Parses which create function to call; only supports CREATE TABLE and CREATE INDEX
QueryResult *SQLExec::create(const CreateStatement *statement) {
    switch(statement->type) {
        case CreateStatement::kTable:
            return create_table(statement);
        case CreateStatement::kIndex:
            return create_index(statement);
        default:
            return new QueryResult("Only CREATE TABLE and CREATE INDEX are implemented");
    }
}

// Parses which drop function to call; only supports DROP TABLE and DROP INDEX
QueryResult *SQLExec::drop(const DropStatement *statement) {
    switch(statement->type) {
        case DropStatement::kTable:
            return drop_table(statement);
        case DropStatement::kIndex:
            return drop_index(statement);
        default:
            return new QueryResult("Only DROP TABLE and DROP INDEX are implemented");
    }
}

// Displays list of tables, table columns, or indices
QueryResult *SQLExec::show(const ShowStatement *statement) {
    switch (statement->type) {
        case ShowStatement::EntityType::kTables:
            return show_tables();
        case ShowStatement::EntityType::kColumns:
            return show_columns(statement);
        case ShowStatement::EntityType::kIndex:
            return show_index(statement);
        default:
            throw SQLExecError("unrecognized SHOW type");
    }
}

// Create table based on SQL query info
QueryResult *SQLExec::create_table(const CreateStatement *statement) {
    Identifier table_name = statement->tableName;
    ColumnNames column_names;
    ColumnAttributes column_attributes;
    Identifier column_name;
    ColumnAttribute column_attribute;
    for (ColumnDefinition *col : *statement->columns) {
        column_definition(col, column_name, column_attribute);
        column_names.push_back(column_name);
        column_attributes.push_back(column_attribute);
    }

    // Add to schema: _tables and _columns
    ValueDict row;
    row["table_name"] = table_name;
    Handle t_handle = SQLExec::tables->insert(&row);  // Insert into _tables
    try {
        Handles c_handles;
        DbRelation& columns = SQLExec::tables->get_table(Columns::TABLE_NAME);
        try {
            for (uint i = 0; i < column_names.size(); i++) {
                row["column_name"] = column_names[i];
                row["data_type"] = Value(column_attributes[i].get_data_type() == ColumnAttribute::INT ? "INT" : "TEXT");
                c_handles.push_back(columns.insert(&row));  // Insert into _columns
            }

            // Finally, actually create the relation
            DbRelation& table = SQLExec::tables->get_table(table_name);
            if (statement->ifNotExists)
                table.create_if_not_exists();
            else
                table.create();

        } catch (exception& e) {
            // attempt to remove from _columns
            try {
                for (auto const &handle: c_handles)
                    columns.del(handle);
            } catch (...) {}
            throw;
        }

    } catch (exception& e) {
        try {
            // attempt to remove from _tables
            SQLExec::tables->del(t_handle);
        } catch (...) {}
        throw;
    }
    return new QueryResult("created " + table_name);
}

// Drop the specified table
QueryResult *SQLExec::drop_table(const DropStatement *statement) {
    Identifier table_name = statement->name;

    if(statement->type != DropStatement::kTable){
        throw SQLExecError("unrecognized DROP statement");
    }

    if (table_name == Tables::TABLE_NAME || table_name == Columns::TABLE_NAME)
        throw SQLExecError("cannot drop a schema table");

    ValueDict where;
    where["table_name"] = Value(table_name);

    // get the table
    DbRelation& table = SQLExec::tables->get_table(table_name);

    //get indices table and handle
    DbRelation& indicesTable = SQLExec::tables->get_table(Indices::TABLE_NAME);
    Handles* h_indices = indicesTable.select(&where);  
    //get name of indices
    IndexNames indexID = SQLExec::indices->get_index_names(table_name);    

    //Deleting indices 
    for(auto const& handle: *h_indices)
	indicesTable.del(handle);

    // getting index name and dropping 
    for(auto const& index_id: indexID){
	DbIndex& index = SQLExec::indices->get_index(table_name,index_id);
	index.drop();
    }

    // remove from _columns schema
    DbRelation& columns = SQLExec::tables->get_table(Columns::TABLE_NAME);
    Handles* handles = columns.select(&where);
    for (auto const& handle: *handles)
        columns.del(handle);
    delete handles;
    delete h_indices;

    // remove table
    table.drop();

    // finally, remove from _tables schema
    SQLExec::tables->del(*SQLExec::tables->select(&where)->begin()); // expect only one row from select

    return new QueryResult(string("dropped ") + table_name);
}

// Displays table info
QueryResult *SQLExec::show_tables() {
    // Set up labels/header
    ColumnNames* column_names = new ColumnNames;
    column_names->push_back("table_name");

    ColumnAttributes* column_attributes = new ColumnAttributes;
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::TEXT));

    // Retrieve handles
    Handles* handles = SQLExec::tables->select();
    u_long n = handles->size() - 3; // Get size/number of rows (subtract three to account for tables, columns, and indices schemas)

    ValueDicts* rows = new ValueDicts;
    for (auto const& handle: *handles) {
        ValueDict* row = SQLExec::tables->project(handle, column_names);
        Identifier table_name = row->at("table_name").s;
        if (table_name != Tables::TABLE_NAME && table_name != Columns::TABLE_NAME && table_name != Indices::TABLE_NAME)
            rows->push_back(row);
    }
    delete handles;
    return new QueryResult(column_names, column_attributes, rows,
                           "successfully returned " + to_string(n) + " rows");
}

// Displays column info
QueryResult *SQLExec::show_columns(const ShowStatement *statement) {
    DbRelation& columns = SQLExec::tables->get_table(Columns::TABLE_NAME);

    // Set up lables/header
    ColumnNames* column_names = new ColumnNames;
    column_names->push_back("table_name");
    column_names->push_back("column_name");
    column_names->push_back("data_type");

    ColumnAttributes* column_attributes = new ColumnAttributes;
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::TEXT));

    // Retrieve handles for this table from columns schema
    ValueDict where;
    where["table_name"] = Value(statement->tableName);
    Handles* handles = columns.select(&where);
    u_long n = handles->size();

    ValueDicts* rows = new ValueDicts;	
    for (auto const& handle: *handles) {
        ValueDict* row = columns.project(handle, column_names);
        rows->push_back(row);
    }
    delete handles;
    return new QueryResult(column_names, column_attributes, rows,
                           "successfully returned " + to_string(n) + " rows");
}

// Create an index
QueryResult *SQLExec::create_index(const CreateStatement *statement) {
    // Declare Identifier
    Identifier table_name = statement->tableName;
    Identifier index_name = statement->indexName;
    Identifier index_type;
    
    bool is_unique;
    
    // Add to schema
    Handles cHandles;
    ValueDict row;
    row["table_name"]= table_name;

    try{
	index_type = statement->indexType;
    } catch(exception& e){
	index_type="BTREE";
    }
    if(index_type == "BTREE")
	is_unique = true;
    else
	is_unique = false;

    row["table_name"] = table_name;
    row["index_name"] = index_name;
    row["seq_in_index"] = 0;
    row["index_type"] = index_type;
    row["is_unique"] = is_unique;	

    try{
	for(auto const& col: *statement->indexColumns){
	    row["seq_in_index"].n += 1;
	    row["column_name"] = string(col);
	    cHandles.push_back(SQLExec::indices->insert(&row));
	}	
   
    // Create index
    DbIndex& index = SQLExec::indices->get_index(table_name, index_name);
    index.create();
    } catch(exception& e){ // Delete handles if error occurs
	for(auto const &handle: cHandles){
	    SQLExec::indices->del(handle);
	}
    throw;
    }
   
    return new QueryResult("Create Index "+ index_name);
}

// TODO Displays index info
QueryResult *SQLExec::show_index(const ShowStatement *statement) {
    return new QueryResult("show index not implemented");  // FIXME
}

// Drop the specified index
QueryResult *SQLExec::drop_index(const DropStatement *statement) {
    // Check if statement is vaild DROP statement
    if(statement->type != DropStatement::kIndex)
	return new QueryResult("Unrecognized Drop Statement");
    
    Identifier table_name = statement->name;
    Identifier index_name = statement->indexName;
    DbIndex& index = SQLExec::indices->get_index(table_name, index_name);
    ValueDict where;
    where["table_name"] = table_name;
    where["index_name"] = index_name;

    Handles* index_handles = SQLExec::indices->select(&where);
    index.drop();

    for(unsigned int i = 0; i<index_handles->size(); i++)
	SQLExec::indices->del(index_handles->at(i));

    // Clear up memeory
    delete index_handles;
    return new QueryResult("Dropped Index " + index_name);
}

