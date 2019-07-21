/**
 * Milestone 3 - Heap Storage Engine
 * @file heap_storage.cpp
 * @authors Grant Bishop & Jon Kimray (Sprint Verano), 
 *          Alicia Mauldin & Vishakha Bhavsar (Sprint Otono)
 * @see 'Seattle University, CPSC-5300, Summer 2019'
 *
 * Heap storage engine components:
 * Inherits abstract base classes from storage_engine.h 
 * and implements concrete classes from heap_storage.h
 */

#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include "heap_storage.h"

using namespace std;
typedef uint16_t u16;

//******** SLOTTED PAGE IMPLEMENTATION ********//

// Ctor for SlottedPage class
SlottedPage::SlottedPage(Dbt &block, BlockID block_id, bool is_new) : DbBlock(block, block_id, is_new) {
    if (is_new) {
        // Sets initial block header values if new block
        this->num_records = 0;
        this->end_free = DbBlock::BLOCK_SZ - 1;
        this->put_header();
    } else {
        this->get_header(this->num_records, this->end_free);
    }
}

// Add a new record to the SlottedPage block and return its id
RecordID SlottedPage::add(const Dbt* data) throw(DbBlockNoRoomError) {
    // Throw error if not enough space
    if (!has_room(data->get_size() + 4))
        throw DbBlockNoRoomError("Error: not enough room for a new record");

    // Increment record count and decrease end_free point
    u16 id = ++this->num_records;
    u16 size = (u16) data->get_size();
    this->end_free -= size;
    u16 loc = this->end_free + 1;

    // Update block header info
    this->put_header();

    // Add header for new record entry and return id
    this->put_header(id, size, loc);
    memcpy(this->address(loc), data->get_data(), size);
    return id;
}

// Get record from block using its id
Dbt* SlottedPage::get(RecordID record_id) {
    u16 size;
    u16 loc;
    this->get_header(size, loc, record_id);

    // If record was deleted, return null pointer
    if (loc == 0) {
        return nullptr;
    }
    char* buffer = new char[size];
    memcpy(buffer, this->address(loc), size);
    Dbt* recordFromBlock = new Dbt(buffer, size);
    return recordFromBlock;
}

// Update a record with new data
void SlottedPage::put(RecordID record_id, const Dbt &data) throw(DbBlockNoRoomError) {
    u16 size;
    u16 loc;
    this->get_header(size, loc, record_id);
    u16 newSize = data.get_size();

    // If record size has increased
    if (newSize > size) {
        u16 extra = newSize - size;

        // Throw error if not enough space
        if (!has_room(extra))
            throw DbBlockNoRoomError("Error: not enough room for a new record");

        // Shift record slot space
        this->slide(loc + newSize, loc + size);
        memcpy(this->address(loc - extra), data.get_data(), newSize);
    } else {
        memcpy(this->address(loc), data.get_data(), newSize);
        this->slide(loc + newSize, loc + size);
    }

    // Update header
    this->get_header(size, loc, record_id);
    this->put_header(record_id, newSize, loc);
}

// Delete a record by reducing size to 0 and location to 0
void SlottedPage::del(RecordID record_id) {
    u16 size;
    u16 loc;
    this->get_header(size, loc, record_id);
    this->put_header(record_id, 0, 0);
    this->slide(loc, loc + size);
}

// Returns all record ids in SlottedPage block
RecordIDs* SlottedPage::ids(void) {
    u16 recordCount = this->num_records;
    RecordIDs* idRecordList = new RecordIDs();
    for (int i = 1; i <= recordCount; i++) {
        u16 size;
        u16 loc;
        this->get_header(size, loc, i);
        if (loc != 0) {
            idRecordList->push_back(i);
        }
    }
    return idRecordList;
}

// Get the size and offset location for given id. For id of zero,
// get the block header.
void SlottedPage::get_header(u16 &size, u16 &loc, RecordID id) {
    size = get_n(4 * id);
    loc = get_n(4 * id + 2);
}

// Store the size and offset for given id. For id of zero,
// store the block header.
void SlottedPage::put_header(RecordID id, u16 size, u16 loc) {
    if (id == 0) { // Update the block header info
        size = this->num_records;
        loc = this->end_free;
    }
    put_n(4 * id, size);
    put_n(4 * id + 2, loc);
}

// Returns if space is available for a record with a given size,
// inclusive of 4 byte header
bool SlottedPage::has_room(u16 size) {
    u16 spaceLeft = this->end_free - (this->num_records + 1) * 4;
    return (size <= spaceLeft);
}

// If start < end, then remove data from offset start up to but not including offset end by sliding data
// that is to the left of start to the right. If start > end, then make room for extra data from end to start
// by sliding data that is to the left of start to the left.
// Also fix up any record headers whose data has slid. Assumes there is enough room if it is a left
// shift (end < start).
void SlottedPage::slide(u16 start, u16 end) {
    u16 shift = end - start;
    if (shift == 0) {
        return;
    }

    // Slide data according to the amount of shift
    memcpy(this->address(this->end_free + 1), this->address(this->end_free + 1 + shift), abs(shift));

    // Update record headers
    RecordIDs* recordList = this->ids();
    for (u16 id : *recordList) {
        u16 size;
        u16 loc;
        this->get_header(size, loc, id);
        if (loc <= start) {
            loc += shift;
            this->put_header(id, size, loc);
        }

        // Update block header info
        this->end_free += shift;
        this->put_header();
    }

    delete recordList;
}

// Get 2-byte integer at given offset in block
u16 SlottedPage::get_n(u16 offset) {
    return *(u16*)this->address(offset);
}

// Put a 2-byte integer at given offset in block.
void SlottedPage::put_n(u16 offset, u16 n) {
    *(u16*)this->address(offset) = n;
}

// Make a void* pointer for a given offset into the data block.
void* SlottedPage::address(u16 offset) {
    return (void*)((char*)this->block.get_data() + offset);
}

//******** HEAP FILE IMPLEMENTATION ********//

// Create file
void HeapFile::create(void) {
    this->db_open(DB_CREATE | DB_EXCL);
    DbBlock* block = this->get_new();
    this->put(block);
    delete block;
}

// Delete file
void HeapFile::drop(void) {
    this->close();
    remove(this->dbfilename.c_str());
}

// Open file
void HeapFile::open(void) {
    this->db_open();
}

// Close file
void HeapFile::close(void) {
    this->db.close(0);
    this->closed = true;
}

// Allocate a new block for the database file.
// Returns the new empty DbBlock that is managing the records in this block and its block id.
SlottedPage* HeapFile::get_new(void) {
    char block[DbBlock::BLOCK_SZ];
    memset(block, 0, sizeof(block));
    Dbt data(block, sizeof(block));

    int block_id = ++this->last;
    Dbt key(&block_id, sizeof(block_id));

    // write out an empty block and read it back in so Berkeley DB is managing the memory
    SlottedPage* page = new SlottedPage(data, this->last, true);
    this->db.put(nullptr, &key, &data, 0); // write it out with initialization applied
    this->db.get(nullptr, &key, &data, 0);
    return page;
}

// Get a block from the database file
SlottedPage* HeapFile::get(BlockID block_id) {
    char block[DbBlock::BLOCK_SZ];
    Dbt data(block, sizeof(block));
    Dbt key(&block_id, sizeof(block_id));
    this->db.get(nullptr, &key, &data, 0);
    SlottedPage* page = new SlottedPage(data, block_id, false);
    return page;
}

// Write a block back to the database file
void HeapFile::put(DbBlock* block) {
    void* blockData = block->get_data();
    Dbt data(blockData, DbBlock::BLOCK_SZ);
    int block_id = block->get_block_id();
    Dbt key(&block_id, sizeof(block_id));
    this->db.put(nullptr, &key, &data, 0);
}

// Return all block ids
BlockIDs* HeapFile::block_ids() {
    BlockIDs* blockList = new BlockIDs();
    for (BlockID i = 1; i <= this->last; i++) {
        blockList->push_back(i);
    }
    return blockList;
}

// Wrapper for Berkeley DB open, which does both open and creation.
void HeapFile::db_open(unsigned int flags) {
    // If file is not already open
    if (!this->closed) {
        return;
    }

    const char* envHome = nullptr;
    _DB_ENV->get_home(&envHome);
    string path = envHome;
    this->dbfilename = path + "/" + this->name + ".db";
    this->db.open(nullptr, this->dbfilename.c_str(), nullptr, DB_RECNO, flags, 0);
    DB_BTREE_STAT *stat;
    this->db.stat(nullptr, &stat, DB_FAST_STAT);
    this->last = stat->bt_ndata;
    this->closed = false;
}

//******** HEAP TABLE IMPLEMENTATION ********//

// Ctor for HeapTable class
HeapTable::HeapTable(Identifier table_name, ColumnNames column_names, ColumnAttributes column_attributes) : DbRelation(table_name, column_names, column_attributes), file(table_name) {}

// Create table
void HeapTable::create() {
    this->file.create();
}

// Open an existing table if present
void HeapTable::create_if_not_exists() {
    try {
        this->file.open();
    } catch (DbException &e) {
        this->file.create();
    }
}

// Delete table
void HeapTable::drop() {
    this->file.drop();
}

// Open table
void HeapTable::open() {
    this->file.open();
}

// Close table
void HeapTable::close() {
    this->file.close();
}

// Insert a row into the table
// Only supports INT or TEXT type for now
Handle HeapTable::insert(const ValueDict* row) {
    this->open();
    return this->append(this->validate(row));
}

// Update row values in the table
void HeapTable::update(const Handle handle, const ValueDict* new_values) {
    // TODO implement in later milestone
}

void HeapTable::del(const Handle handle) {
    // TODO implement in later milestone
}

// Default select row values
Handles* HeapTable::select() {
    Handles* handles = new Handles();
    BlockIDs* block_ids = this->file.block_ids();
    for (auto const& block_id: *block_ids) {
        SlottedPage* block = this->file.get(block_id);
        RecordIDs* record_ids = block->ids();
        for (auto const& record_id: *record_ids)
            handles->push_back(Handle(block_id, record_id));
        delete record_ids;
        delete block;
    }
    delete block_ids;
    return handles;
}

// Overloaded select row values based on where condition
// Conceptually, execute: SELECT <handle> FROM <table_name> WHERE <where>
Handles* HeapTable::select(const ValueDict* where) {
    // TODO implement where functionality in later milestone
    cout << "HeapTable::select overload should not be called\n";
    Handles* handles = new Handles();
    BlockIDs* block_ids = file.block_ids();
    for (auto const& block_id: *block_ids) {
        SlottedPage* block = file.get(block_id);
        RecordIDs* record_ids = block->ids();
        for (auto const& record_id: *record_ids)
            handles->push_back(Handle(block_id, record_id));
        delete record_ids;
        delete block;
    }
    delete block_ids;
    return handles;
}

// Return a sequence of values for handle
ValueDict* HeapTable::project(Handle handle) {
    BlockID block_id = handle.first;
    RecordID record_id = handle.second;
    SlottedPage* block = this->file.get(block_id);
    Dbt* data = block->get(record_id);
    ValueDict* row = unmarshal(data);
    delete data;
    delete block;
    return row;
}

// Overloaded project row values for handle given by column_names
ValueDict* HeapTable::project(Handle handle, const ColumnNames* column_names) {
    // TODO implement in later milestone
    ValueDict* row = new ValueDict();
    return row;
}

// Check if the given row is acceptable to insert. Raise an error if not.
// Otherwise return the full row dictionary.
ValueDict* HeapTable::validate(const ValueDict* row) {
    ValueDict* fullRow = new ValueDict();
    for (auto const& column_name: this->column_names) {
        ValueDict::const_iterator column = row->find(column_name);
        Value val;
        if (column == row->end()) {
            throw DbRelationError("Error: NULL and defaults not yet implemented");
        } else {
            val = column->second;
        }
        (*fullRow)[column_name] = val;
    }
    return fullRow;
}

// Assumes row is fully fleshed-out. Appends a record to the file.
Handle HeapTable::append(const ValueDict* row) {
    u16 record_id;
    Dbt* data = marshal(row);

    // Get id of last block in the file and then get the block itself
    SlottedPage* block = this->file.get(this->file.get_last_block_id());

    // Try adding row data to the block
    try {
        record_id = block->add(data);
    } catch (DbBlockNoRoomError const &e) {
        // Need a new block
        block = this->file.get_new();
        record_id = block->add(data);
    }

    // Add updated block to file and create handle tuple reference
    this->file.put(block);
    delete block;
    delete data;
    return Handle(this->file.get_last_block_id(), record_id);
}

// Return the bits to go into the file.
// Caller responsible for freeing the returned Dbt and its enclosed ret->get_data().
Dbt* HeapTable::marshal(const ValueDict* row) {
    char *bytes = new char[DbBlock::BLOCK_SZ]; // more than we need (we insist that one row fits into DbBlock::BLOCK_SZ)
    unsigned int offset = 0;
    unsigned int col_num = 0;
    for (auto const& column_name: this->column_names) {
        ColumnAttribute ca = this->column_attributes[col_num++];
        ValueDict::const_iterator column = row->find(column_name);
        Value value = column->second;
        if (ca.get_data_type() == ColumnAttribute::DataType::INT) {
            *(int32_t*) (bytes + offset) = value.n;
            offset += sizeof(int32_t);
        } else if (ca.get_data_type() == ColumnAttribute::DataType::TEXT) {
            unsigned int size = value.s.length();
            *(u16*) (bytes + offset) = size;
            offset += sizeof(u16);
            memcpy(bytes+offset, value.s.c_str(), size); // assume ascii for now
            offset += size;
        } else {
            throw DbRelationError("Error: only know how to marshal INT and TEXT");
        }
    }
    char *right_size_bytes = new char[offset];
    memcpy(right_size_bytes, bytes, offset);
    delete[] bytes;
    Dbt *data = new Dbt(right_size_bytes, offset);
    return data;
}

// Convert the bits into values from the file.
ValueDict* HeapTable::unmarshal(Dbt* data) {
    ValueDict* row = new ValueDict();
    char *bytes = (char*) data->get_data();
    unsigned int offset = 0;
    unsigned int col_num = 0;
    Value val;
    for (auto const& column_name: this->column_names) {
        ColumnAttribute ca = this->column_attributes[col_num++];
        val.data_type = ca.get_data_type();
        if (ca.get_data_type() == ColumnAttribute::DataType::INT) {
            val.n = *(int32_t*)(bytes + offset);
            offset += sizeof(int32_t);
        } else if (ca.get_data_type() == ColumnAttribute::DataType::TEXT) {
            unsigned int size = *(u16*)(bytes + offset);
            offset += sizeof(u16);
            char dataFromBytes[DbBlock::BLOCK_SZ];
            memcpy(dataFromBytes, bytes + offset, size);
            dataFromBytes[size] = '\0';
            val.s = string(dataFromBytes);
            offset += size;
        } else {
            throw DbRelationError("Error: only know how to unmarshal INT and TEXT");
        }
        (*row)[column_name] = val;
    }
    return row;
}

// Tests Milestone 2 aspects of the heap_storage.cpp program.
// Returns true if all tests pass.
bool test_heap_storage() {
    ColumnNames column_names;
    column_names.push_back("a");
    column_names.push_back("b");
    ColumnAttributes column_attributes;
    ColumnAttribute ca(ColumnAttribute::INT);
    column_attributes.push_back(ca);
    ca.set_data_type(ColumnAttribute::TEXT);
    column_attributes.push_back(ca);
    HeapTable table1("_test_create_drop_cpp", column_names, column_attributes);
    table1.create();
    cout << "create ok" << endl;
    table1.drop();
    cout << "drop ok" << endl;
    HeapTable table2("_test_data_cpp", column_names, column_attributes);
    table2.create_if_not_exists();
    cout << "create_if_not_exists ok" << endl;
    ValueDict row;
    row["a"] = Value(12);
    row["b"] = Value("Hello!");
    cout << "try insert" << endl;
    table2.insert(&row);
    cout << "insert ok" << endl;
    Handles* handles = table2.select();
    cout << "select ok " << handles->size() << endl;
    ValueDict* result = table2.project((*handles)[0]);
    cout << "project ok" << endl;
    Value value = (*result)["a"];
    if (value.n != 12) {
        cout << "value.n failed" << endl;
        cout << value.n << endl;
        return false;
    }
    value = (*result)["b"];
    if (value.s != "Hello!") {
        cout << "value.s failed" << endl;
        cout << value.s << endl;
        return false;
    }
    table2.drop();
    delete handles;
    delete result;
    return true;
}
