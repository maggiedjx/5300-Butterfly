/**
 * Milestone 3 & 4 Implementations
 * @file SQLExec.cpp 
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
    
    // initialize _indices if not yet present
    if (SQLExec::indices == nullptr)
        SQLExec::indices = new Indices();

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
    case ShowStatement::kTables:
        return show_tables();
    case ShowStatement::kColumns:
        return show_columns(statement);
    case ShowStatement::kIndex:
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

    //drop each index before dropping table...
    Handles* i_handles = indices->select(&where);
    for (auto const& handle: *i_handles)
        indices->del(handle);

    //remove from _columns schema
    DbRelation& columns = SQLExec::tables->get_table(Columns::TABLE_NAME);
    Handles* handles = columns.select(&where);     

   
    for(auto const& handle: *handles)
        columns.del(handle);
   
    //delete ptr avoiding memory leak
    delete handles;

    // remove table
    table.drop();

    // finally, remove from _tables schema
    SQLExec::tables->del(*SQLExec::tables->select(&where)->begin());
    
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

    // Get size/number of rows (subtract three to account for tables, columns,
    // and indices schemas)
    u_long n = handles->size() - 3;

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
    SQLExec::indices = new Indices();

    // Declare Identifier
    Identifier table_name = statement->tableName;
    Identifier index_name = statement->indexName;
    Identifier index_type = statement->indexType;
    
    bool is_unique = true;
    
    if(index_type == "HASH")
        is_unique = false;
    
    // Add to schema
    Handles cHandles;
    ValueDict row;

    DbRelation& table = SQLExec::tables->get_table(table_name);
    
    ColumnNames table_column_names = table.get_column_names();
    ColumnNames stmt_column_names;

    for (Identifier col : *statement->indexColumns) {
        stmt_column_names.push_back(col);
    }
   
    //check to make sure tables and columns exist; return error if not
    for (u_int i = 0; i < stmt_column_names.size(); i++) {
        bool exists = false;
        for (u_int p = 0; p < table_column_names.size(); p++) {
            if(stmt_column_names.at(i) == table_column_names.at(p))
                exists = true;
        }
        if(exists == false)
            return new QueryResult("unable to create index; some table or column does not exist");
    }

    row["table_name"] = table_name;
    row["index_name"] = index_name;
    row["seq_in_index"] = 0;
    row["index_type"] = index_type;
    row["is_unique"] = is_unique;   
    
    for(u_int i = 0; i < stmt_column_names.size(); i++) {
        row["seq_in_index"] = row["seq_in_index"].n + 1;
        row["column_name"] = stmt_column_names.at(i);
        indices->insert(&row); //Handle i_handle = SQLExec::indices
    }
    
    DbIndex& index = indices->get_index(table_name, index_name);
    index.create();
   
    return new QueryResult("create index " + index_name);
}

// Displays index info
QueryResult *SQLExec::show_index(const ShowStatement *statement) {  
    // Create index view columns
    ColumnNames* column_names = new ColumnNames;
    column_names->push_back("table_name");
    column_names->push_back("index_name");
    column_names->push_back("column_name");
    column_names->push_back("seq_in_index");	
    column_names->push_back("index_type");
    column_names->push_back("is_unique");
    
    Identifier table_name = statement->tableName;
    ValueDict where;
    where["table_name"] = Value(table_name);

    Handles* handles = SQLExec::indices->select(&where);
    ValueDicts* entries = new ValueDicts();

    for(auto const& handle: *handles){
        ValueDict* entry = SQLExec::indices->project(handle, column_names);
        entries->push_back(entry);
    }

    ColumnAttributes* column_attributes = new ColumnAttributes;
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::TEXT));

    int size = handles->size();

    // Clean up memory
    delete handles;

    return new QueryResult(column_names, column_attributes, entries,
                           "successfully returned " + to_string(size)
                           + " rows");
}

// Drop the specified index 
QueryResult *SQLExec::drop_index(const DropStatement *statement) { 
    // Check if statement is vaild DROP statement
    if(statement->type != DropStatement::kIndex)
        throw SQLExecError("Drop type is unrecognized");

    Identifier table_name = statement->name;
    Identifier index_name = statement->indexName;

    ValueDict where;
    where["table_name"] = Value(table_name);
    where["index_name"] = Value(index_name);

    DbRelation& t_index = SQLExec::tables->get_table(Indices::TABLE_NAME);
    Handles* handles = t_index.select(&where);

    DbIndex& index = SQLExec::indices->get_index(table_name,index_name);
    
    for(auto const& handle : *handles) 
        t_index.del(handle);

    bool failed = false;
    if(handles->size() == 0)
        failed = true;

    // Clear up memeory
    index.drop();
    delete handles;
    
    if(!failed)    
        return new QueryResult("dropped index " + index_name + " from "
                               + table_name);
    else
        return new QueryResult("no matching index to drop");
}

