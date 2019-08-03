/**
<<<<<<< HEAD
 * Milestone 3 & 4 Implementations
 * @file SQLExec.cpp 
 * @authors Kevin Lundeen, Alicia Mauldin, Vishakha Bhavsar
 * @see "Seattle University, CPSC5300, Summer 2019"
 */
 
=======
 * @file SQLExec.cpp - implementation of SQLExec class 
 * @author Kevin Lundeen
 * @see "Seattle University, CPSC5300, Summer 2018"
 */
#include <algorithm>
>>>>>>> Standardizing files for Milestone 6
#include "SQLExec.h"
using namespace std;
using namespace hsql;

<<<<<<< HEAD
// Define static data
Tables* SQLExec::tables = nullptr;
Indices* SQLExec::indices = nullptr; //for indices implementation


// Make query result be printable
=======
Tables* SQLExec::tables = nullptr;
Indices* SQLExec::indices = nullptr;

>>>>>>> Standardizing files for Milestone 6
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
<<<<<<< HEAD
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
=======
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
>>>>>>> Standardizing files for Milestone 6
                }
                out << " ";
            }
            out << endl;
        }
    }
    out << qres.message;
    return out;
}

<<<<<<< HEAD
// QueryResult dtor clears up non-null class pointers
=======
>>>>>>> Standardizing files for Milestone 6
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

<<<<<<< HEAD
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
=======

QueryResult *SQLExec::execute(const SQLStatement *statement) throw(SQLExecError) {
    // initialize _tables table, if not yet present
    if (SQLExec::tables == nullptr) {
        SQLExec::tables = new Tables();
		SQLExec::indices = new Indices();
	}

    try {
        switch (statement->type()) {
            case kStmtCreate:
                return create((const CreateStatement *) statement);
            case kStmtDrop:
                return drop((const DropStatement *) statement);
            case kStmtShow:
                return show((const ShowStatement *) statement);
            case kStmtInsert:
                return insert((const InsertStatement *) statement);
            case kStmtDelete:
                return del((const DeleteStatement *) statement);
            case kStmtSelect:
                return select((const SelectStatement *) statement);
            default:
                return new QueryResult("not implemented");
>>>>>>> Standardizing files for Milestone 6
        }
    } catch (DbRelationError& e) {
        throw SQLExecError(string("DbRelationError: ") + e.what());
    }
}

<<<<<<< HEAD
// Intakes column info and sets appropriate data type
// Only supports types INT, TEXT, and DOUBLE for now
=======
QueryResult *SQLExec::insert(const InsertStatement *statement) {
    return new QueryResult("INSERT statement not yet implemented");  // FIXME
}

QueryResult *SQLExec::del(const DeleteStatement *statement) {
    return new QueryResult("DELETE statement not yet implemented");  // FIXME
}

QueryResult *SQLExec::select(const SelectStatement *statement) {
    return new QueryResult("SELECT statement not yet implemented");  // FIXME
}

>>>>>>> Standardizing files for Milestone 6
void SQLExec::column_definition(const ColumnDefinition *col, Identifier& column_name,
                                ColumnAttribute& column_attribute) {
    column_name = col->name;
    switch (col->type) {
<<<<<<< HEAD
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
=======
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
 
>>>>>>> Standardizing files for Milestone 6
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
<<<<<<< HEAD
    Handle t_handle = tables->insert(&row);  // Insert into _tables
    try {
        Handles c_handles;
        DbRelation& columns = tables->get_table(Columns::TABLE_NAME);
=======
    Handle t_handle = SQLExec::tables->insert(&row);  // Insert into _tables
    try {
        Handles c_handles;
        DbRelation& columns = SQLExec::tables->get_table(Columns::TABLE_NAME);
>>>>>>> Standardizing files for Milestone 6
        try {
            for (uint i = 0; i < column_names.size(); i++) {
                row["column_name"] = column_names[i];
                row["data_type"] = Value(column_attributes[i].get_data_type() == ColumnAttribute::INT ? "INT" : "TEXT");
                c_handles.push_back(columns.insert(&row));  // Insert into _columns
            }

            // Finally, actually create the relation
<<<<<<< HEAD
            DbRelation& table = tables->get_table(table_name);
=======
            DbRelation& table = SQLExec::tables->get_table(table_name);
>>>>>>> Standardizing files for Milestone 6
            if (statement->ifNotExists)
                table.create_if_not_exists();
            else
                table.create();

<<<<<<< HEAD
        } catch (exception& e) {
=======
        } catch (...) {
>>>>>>> Standardizing files for Milestone 6
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
<<<<<<< HEAD
            tables->del(t_handle);
=======
            SQLExec::tables->del(t_handle);
>>>>>>> Standardizing files for Milestone 6
        } catch (...) {}
        throw;
    }
    return new QueryResult("created " + table_name);
}

<<<<<<< HEAD
// Drop the specified table
QueryResult *SQLExec::drop_table(const DropStatement *statement) {
    Identifier table_name = statement->name;

    if(statement->type != DropStatement::kTable){
        throw SQLExecError("unrecognized DROP statement");
    }

=======
QueryResult *SQLExec::create_index(const CreateStatement *statement) {
    Identifier index_name = statement->indexName;
    Identifier table_name = statement->tableName;

    // get underlying relation
    DbRelation& table = SQLExec::tables->get_table(table_name);

    // check that given columns exist in table
    const ColumnNames& table_columns = table.get_column_names();
    for (auto const& col_name: *statement->indexColumns)
        if (find(table_columns.begin(), table_columns.end(), col_name) == table_columns.end())
            throw SQLExecError(string("Column '") + col_name + "' does not exist in " + table_name);

    // insert a row for every column in index into _indices
    ValueDict row;
    row["table_name"] = Value(table_name);
    row["index_name"] = Value(index_name);
    row["index_type"] = Value(statement->indexType);
    row["is_unique"] = Value(string(statement->indexType) == "BTREE"); // assume HASH is non-unique --
    int seq = 0;
    Handles i_handles;
    try {
        for (auto const &col_name: *statement->indexColumns) {
            row["seq_in_index"] = Value(++seq);
            row["column_name"] = Value(col_name);
            i_handles.push_back(SQLExec::indices->insert(&row));
        }

        DbIndex &index = SQLExec::indices->get_index(table_name, index_name);
        index.create();

    } catch (...) {
        // attempt to remove from _indices
        try {  // if any exception happens in the reversal below, we still want to re-throw the original ex
            for (auto const &handle: i_handles)
                SQLExec::indices->del(handle);
        } catch (...) {}
        throw;  // re-throw the original exception (which should give the client some clue as to why it did
    }
    return new QueryResult("created index " + index_name);
}

// DROP ...
QueryResult *SQLExec::drop(const DropStatement *statement) {
    switch(statement->type) {
        case DropStatement::kTable:
            return drop_table(statement);
        case DropStatement::kIndex:
            return drop_index(statement);
        default:
            return new QueryResult("Only DROP TABLE and CREATE INDEX are implemented");
    }
}
 
QueryResult *SQLExec::drop_table(const DropStatement *statement) {
    Identifier table_name = statement->name;
>>>>>>> Standardizing files for Milestone 6
    if (table_name == Tables::TABLE_NAME || table_name == Columns::TABLE_NAME)
        throw SQLExecError("cannot drop a schema table");

    ValueDict where;
    where["table_name"] = Value(table_name);

    // get the table
<<<<<<< HEAD
    DbRelation& table = tables->get_table(table_name);

    //drop each index before dropping table...
    Handles* i_handles = indices->select(&where);
    for (auto const& handle: *i_handles)
        indices->del(handle);

    //remove from _columns schema
    DbRelation& columns = tables->get_table(Columns::TABLE_NAME);
    Handles* handles = columns.select(&where);     

   
    for(auto const& handle: *handles)
        columns.del(handle);
   
    //delete ptr avoiding memory leak
=======
    DbRelation& table = SQLExec::tables->get_table(table_name);

    // remove any indices
    for (auto const& index_name: SQLExec::indices->get_index_names(table_name)) {
        DbIndex& index = SQLExec::indices->get_index(table_name, index_name);
        index.drop();  // drop the index
    }
    Handles* handles = SQLExec::indices->select(&where);
    for (auto const& handle: *handles)
        SQLExec::indices->del(handle);  // remove all rows from _indices for each index on this table
    delete handles;

    // remove from _columns schema
    DbRelation& columns = SQLExec::tables->get_table(Columns::TABLE_NAME);
    handles = columns.select(&where);
    for (auto const& handle: *handles)
        columns.del(handle);
>>>>>>> Standardizing files for Milestone 6
    delete handles;

    // remove table
    table.drop();

    // finally, remove from _tables schema
<<<<<<< HEAD
    tables->del(*tables->select(&where)->begin());
    
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
    Handles* handles = tables->select();

    // Get size/number of rows (subtract three to account for tables, columns,
    // and indices schemas)
    u_long n = handles->size() - 3;

    ValueDicts* rows = new ValueDicts;
    for (auto const& handle: *handles) {
        ValueDict* row = tables->project(handle, column_names);
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
    DbRelation& columns = tables->get_table(Columns::TABLE_NAME);

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
=======
    SQLExec::tables->del(*SQLExec::tables->select(&where)->begin()); // expect only one row from select

    return new QueryResult(string("dropped ") + table_name);
}

QueryResult *SQLExec::drop_index(const DropStatement *statement) {
    Identifier table_name = statement->name;
    Identifier index_name = statement->indexName;

    // drop index
    DbIndex& index = SQLExec::indices->get_index(table_name, index_name);
    index.drop();

    // remove rows from _indices for this index
    ValueDict where;
    where["table_name"] = Value(table_name);
    where["index_name"] = Value(index_name);
    Handles* handles = SQLExec::indices->select(&where);
    for (auto const& handle: *handles)
        SQLExec::indices->del(handle);
    delete handles;

    return new QueryResult("dropped index " + index_name);
}

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

QueryResult *SQLExec::show_index(const ShowStatement *statement) {
    ColumnNames* column_names = new ColumnNames;
    ColumnAttributes* column_attributes = new ColumnAttributes;
    column_names->push_back("table_name");
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::TEXT));

    column_names->push_back("index_name");
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::TEXT));

    column_names->push_back("column_name");
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::TEXT));

    column_names->push_back("seq_in_index");
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::INT));

    column_names->push_back("index_type");
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::TEXT));

    column_names->push_back("is_unique");
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::BOOLEAN));

    ValueDict where;
    where["table_name"] = Value(string(statement->tableName));
    Handles* handles = SQLExec::indices->select(&where);
    u_long n = handles->size();

    ValueDicts* rows = new ValueDicts;
    for (auto const& handle: *handles) {
        ValueDict* row = SQLExec::indices->project(handle, column_names);
>>>>>>> Standardizing files for Milestone 6
        rows->push_back(row);
    }
    delete handles;
    return new QueryResult(column_names, column_attributes, rows,
                           "successfully returned " + to_string(n) + " rows");
}

<<<<<<< HEAD
// Create an index
QueryResult *SQLExec::create_index(const CreateStatement *statement) {
    indices = new Indices();

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

    DbRelation& table = tables->get_table(table_name);
    
    ColumnNames table_column_names = table.get_column_names();
    ColumnNames stmt_column_names;

    for (Identifier col : *statement->indexColumns) {
        stmt_column_names.push_back(col);
    }
   
    // Check to make sure tables and columns exist; return error if not
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
        indices->insert(&row);
    }
    
    DbIndex& index = indices->get_index(table_name, index_name);
    index.create();
   
    return new QueryResult("created index " + index_name);
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

    // Gather table relation info
    Identifier table_name = statement->tableName;
    ValueDict where;
    where["table_name"] = Value(table_name);

    // Set up view
    Handles* handles = indices->select(&where);
    ValueDicts* entries = new ValueDicts();
    for(auto const& handle: *handles){
        ValueDict* entry = indices->project(handle, column_names);
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

    // Gather and set table and index names
    Identifier table_name = statement->name;
    Identifier index_name = statement->indexName;

    // Drop index relation
    DbIndex& index = indices->get_index(table_name, index_name);
    index.drop();
        
    // Remove references from schema
    ValueDict where;
    where["table_name"] = Value(table_name);
    where["index_name"] = Value(index_name);
    Handles* handles = indices->select(&where);
    for(auto const& handle : *handles) 
        indices->del(handle);

    // Check if index exists on given table
    bool failed = false;
    if(handles->size() == 0)
        failed = true;

    // Clear up memeory
    delete handles;
    
    if(!failed)    
        return new QueryResult("dropped index " + index_name);
    else
        return new QueryResult("no matching index to drop");
}
=======
QueryResult *SQLExec::show_tables() {
    ColumnNames* column_names = new ColumnNames;
    column_names->push_back("table_name");

    ColumnAttributes* column_attributes = new ColumnAttributes;
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::TEXT));

    Handles* handles = SQLExec::tables->select();
    u_long n = handles->size() - 3;

    ValueDicts* rows = new ValueDicts;
    for (auto const& handle: *handles) {
        ValueDict* row = SQLExec::tables->project(handle, column_names);
        Identifier table_name = row->at("table_name").s;
        if (table_name != Tables::TABLE_NAME
            && table_name != Columns::TABLE_NAME
            && table_name != Indices::TABLE_NAME) {

             	rows->push_back(row);
        }
    }
    delete handles;
    return new QueryResult(column_names, column_attributes, rows,
                           "successfully returned " + to_string(n) + " rows");
}

QueryResult *SQLExec::show_columns(const ShowStatement *statement) {
    DbRelation& columns = SQLExec::tables->get_table(Columns::TABLE_NAME);

    ColumnNames* column_names = new ColumnNames;
    column_names->push_back("table_name");
    column_names->push_back("column_name");
    column_names->push_back("data_type");

    ColumnAttributes* column_attributes = new ColumnAttributes;
    column_attributes->push_back(ColumnAttribute(ColumnAttribute::TEXT));

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

>>>>>>> Standardizing files for Milestone 6
