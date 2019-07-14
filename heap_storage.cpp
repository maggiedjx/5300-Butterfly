// "Project Butterfly" - CPSC 5300 / 4300 Summer 2019                                                  
// See README.md for details
// Authors: Grant Bishop and Jonathan Kimray
// Some functions provided by Kevin Lundeen
// THIS FILE: 

// Heap storage engine components
// Inherits and implements
// abstract base classes from storage_engine.h and heap_storage.h

#include "storage_engine.h"
#include "heap_storage.h"

typedef u_int16_t u16;
const unsigned int BLOCK_SZ = 4096;

// Slotted Page -------------------------------------------------------------

// Constructor
SlottedPage::SlottedPage(Dbt &block,
                         BlockID block_id,
                         bool is_new) : DbBlock(block, block_id, is_new) {
    if (is_new) {
        this->num_records = 0;
        this->end_free = BLOCK_SZ - 1;
        put_header();
    } else {
        get_header(this->num_records, this->end_free);
    }
}

// Add a new record to the block. Return its id.
RecordID SlottedPage::add(const Dbt* data) throw(DbBlockNoRoomError) {
    if (!has_room(data->get_size()))
        throw DbBlockNoRoomError("not enough room for new record");
    u16 id = ++this->num_records;
    u16 size = (u16) data->get_size();
    this->end_free -= size;
    u16 loc = this->end_free + 1;
    put_header();
    put_header(id, size, loc);
    std::memcpy(this->address(loc), data->get_data(), size);
    return id;
}

// Returns a Dbt pointer to a record in the given block, null if
// block has been deleted
Dbt* SlottedPage::get(RecordID record_id){
  u16 size, loc;
  get_header(size, loc, record_id);
  if (loc == 0)
    return NULL; //0 is a tombstone, record was deleted
  return new Dbt(this->address(loc), size);
}

// puts a record in at the given record id
// moves existing records as needed
// Throws DbBlockNoRoomError if insufficient space
void SlottedPage::put(RecordID record_id, const Dbt &data) throw(DbBlockNoRoomError)
{
  u16 size, loc;
  get_header(size, loc, record_id);
  u16 new_size =(u16) data.get_size();
  if (new_size > size){
    u16 extra = new_size - size;
    if (!has_room(extra))
      throw DbBlockNoRoomError("not enough room in block");
    slide(loc + new_size, loc + size);
    memcpy(this->address(loc - extra), data.get_data(), new_size);
  }
  else{
    memcpy(this->address(loc), data.get_data(), new_size);
    slide(loc + new_size, loc + size);
  }
  get_header(size, loc, record_id);
  put_header(record_id, size, loc);
}

// Mark the given id as deleted by changing 
// its size to zero and its location to 0.
// Compact the rest of the data in the block. But keep the record 
// ids the same for everyone.
void SlottedPage::del(RecordID record_id)
{
    u16 size, loc;
    get_header(size,loc,record_id);
    if (loc == 0)
      return;
    put_header(record_id,0,0);
    slide(loc, loc + size);
}

// Note: need for function caller to free memory!
// Returns a pointer to a vector containing all
// record ids in the given SlottedPage
//
// TODO FIXME SORRY
// ERROR: durring the course of running the test_heap_storage function
// this function is called.  The loop would be expcected to run only a few times
// that is not what happends.  instead it gets called thousands of times, untill 
// something (not sure exactly what) overflows or otherwise goes wrong and 
// crashes the program with a segmentation fault
// The reason the loop runs to many times is that num_records is a really large
// number: 52140 to be exact (it does not change run to run)
// This happems when the 'this' slotted page is created in HeapFile::get()
// See comments at that function (search "SORRY") for more info
RecordIDs* SlottedPage::ids()
{
  RecordIDs* recs = new RecordIDs();
  u16 size, loc;
  for(RecordID i = 1; i <= this->num_records; ++i) { // This is the (nearly) unending loop in question
     this->get_header(size,loc,i); // The value of num_records is anomolysly high - 52140, rather than 1 or 2
     if(size != 0 && loc != 0) // Is not deleted
         recs->push_back(i);
  }
  return recs;
}

// Get (via ref arguments) the size and location (offset) for a record id
// returns zero if it is the block header 
void SlottedPage::get_header(u16& size, u16& loc, RecordID id)
{
    size = this->get_n(4*id);
    loc = this->get_n(4*id +2);
}


// Store the size and offset for given id.
// For id of zero, store the block header. 
void SlottedPage::put_header(RecordID id, u16 size, u16 loc) {
    if (id == 0) { 
        size = this->num_records;
        loc = this->end_free;
    }
    put_n(4*id, size);
    put_n(4*id + 2, loc);
}

// Returns true if there is enough room in the SlottedPage to store
// a record of the given size, plus an extra 4 bytes for the header
// if an add is being performed 
bool SlottedPage::has_room(u16 size)
{
  return size <= ((u16)(this->end_free - (this->num_records + 1)*4));
}

// If start < end, remove data from offset start up to, but not including
// offset end by sliding data that is to the left of start to the right.
// If start > end, make room for extra data from end to start by sliding
// data that is to the left of start to the left. Any record headers
// whose data is moved corrected.
// NOTE: Assumes there is enough room if end < start 
void SlottedPage::slide(u16 start, u16 end)
{
  u16 shift = end - start;
  if (shift == 0)
    return;
  u16 size = start - this->end_free + 1;
  char* size_arr =  new char[size];
  void *begin = this->address((u16)(this->end_free + 1));
  void *final = this->address((u16)(this->end_free + 1 + shift));
  std::memcpy(size_arr, begin, size);
  std::memcpy(final, size_arr, size);
  RecordIDs* ids = this->ids();
  for (auto  &id : *ids){
    u16 size, loc;
    this->get_header(size, loc, id);
    if (loc <= start){
      loc += shift;
      this->put_header(id, size, loc);
    }
    delete ids;
    this->end_free += shift;
    this->put_header();
  }
  delete size_arr;
}

// Get 2-byte integer at given offset in block. 
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


// Heapfile ------------------------------------------------------------------

// create the physical file
void HeapFile::create()
{
  this->db_open(DB_CREATE | DB_EXCL);
  SlottedPage* block = this->get_new();
  this->put(block);
  delete block;
}

// Just closes to "drop" the file, has no effect on bool closed
void HeapFile::drop()
{
  this->close();
}

// open the physical file
void HeapFile::open()
{
  this->db_open();
}

// closes the physical file and sets closed to true
void HeapFile::close()
{
  this->db.close(0);
  this->closed = true;
}

// gets a pointer to a slotted page from the DB file
//
// TODO FIXME SORRY
// This slotted page is where the erroneously high number of records 
// (that causes the isssue above - search "SORRY")
// comes from.  Inspecting the page SlottedPage object after creation from the 
// data Dbt object reveals a claimed number of 52140 records
// This is obvious not the case
// Somehow the first header section (first 4 bytes of the slotted page)
// is either not being stored correctly in put or not being
// brought back out correctly in this function.
SlottedPage* HeapFile::get(BlockID block_id)
{
  char block[BLOCK_SZ];
  Dbt data(block, sizeof(block));
  Dbt key(&block_id, sizeof(block_id));
  this->db.get(nullptr, &key, &data, 0);
  SlottedPage* page = new SlottedPage(data, block_id, false);
  return page;
}

// Allocate a new block for the database file.
// Returns the new empty DbBlock that is managing
// the records in this block and its block id.
SlottedPage* HeapFile::get_new(void) {
    char block[BLOCK_SZ];
    std::memset(block, 0, sizeof(block));
    Dbt data(block, sizeof(block));

    int block_id = ++this->last;
    Dbt key(&block_id, sizeof(block_id));

    // write out an empty block and read it back in so
    // Berkeley DB is managing the memory
    SlottedPage* page = new SlottedPage(data, this->last, true);
    this->db.put(nullptr, &key, &data, 0); // write it out with initialization applied
    this->db.get(nullptr, &key, &data, 0);
    return page;
}

// writes a block to the DB file
void HeapFile::put(DbBlock* block)
{
  BlockID id = block->get_block_id();
  void* data = block->get_data();
  Dbt key(&id, sizeof(id));
  Dbt put_data(data, BLOCK_SZ);
  this->db.put(nullptr, &key, &put_data, 0);
}

// returns a ptr to a vector (BlockIDs)  of all block ids (uint16)
// unsure if this is correct TODO 
BlockIDs* HeapFile::block_ids()
{
  BlockIDs* pages = new BlockIDs();
  for(RecordID i = 1; i < this->last; ++i)
    pages->push_back(i);
  return pages;    
}

// wrapper for Berkley DB open
// handles both open and create
void HeapFile::db_open(unsigned int flags)
{
  if(!this->closed)
    return;
  const char* filepath = nullptr;
  _DB_ENV->get_home(&filepath);
  this->dbfilename = filepath + '/' + name + ".db";
  this->db.open(nullptr, dbfilename.c_str(), nullptr, DB_RECNO, flags, 0);
  this->closed = false;
}



// HeapTable ------------------------------------------------------------------


// Destructor 
HeapTable::~HeapTable() {    
    file.close();
}

// Open existing table
// After table is open, can insert, update, delete, select, and project
// NOTE: insert only supports INT and TEXT. Update and Delete are not
// supported at this time.
void HeapTable::open() {
  this->file.open();
}

// Closes the table
// Disables insert, update, delete, select, and project
void HeapTable::close() {
  this->file.close();
}

// Executes CREATE TABLE <table_name> ( <columns>)
// NOTE: Is not responsible for metadata storage or validation 
void HeapTable::create()
{
  this->file.create();
}

// Executes CREATE TABLE IF NOT EXISTS <table_name> ( <columns> )
// NOTE: Is not responsible for metadata storage or validation 
void HeapTable::create_if_not_exists()
{
  try{
    // try to open
    this->open();
  } catch(DbException& e){
    // create if open fails
    this->create();
  }
}

// Executes DROP TABLE <table_name> 
void HeapTable::drop()
{
  this->file.drop();
}

// Executes INSERT INTO <table_name> (<row_keys>) VALUES (<row_values>)
// Returns a Handle of inserted row 
Handle HeapTable::insert(const ValueDict* row)
{
  this->open();
  return this->append(this->validate(row));
}

// Not supported in Milestone 2 
void HeapTable::update(const Handle handle, const ValueDict* new_values)
{

}

// Not supported in Milestone 2 
void HeapTable::del(const Handle handle)
{
  
}

// Returns a pointer to a vector of handles 
Handles* HeapTable::select()
{
  return select(NULL);
}

// Returns a pointer to a vector of handles
// NOTE: WHERE, GROUP BY, and LIMIT
// functionality not supported in Milestone 2 
Handles* HeapTable::select(const ValueDict* where) {
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

// Returns a pointer to a dictionary of values
// given by column_names 
ValueDict* HeapTable::project(Handle handle)
{
  return project(handle, &this->column_names);
}

// Returns a pointer to a dictionary of values
// given by column_names 
ValueDict* HeapTable::project(Handle handle, const ColumnNames* column_names)
{
  BlockID block_id = handle.first;
  RecordID record_id = handle.second;
  SlottedPage* this_block = file.get(block_id);
  Dbt* this_data = this_block->get(record_id);
  ValueDict* this_row = unmarshal(this_data);
  if (column_names->empty())
    return this_row;
  ValueDict* result = new ValueDict();
  for (auto const& col_name: *column_names){
    if (this_row->find(col_name) == this_row->end())
      throw DbRelationError("Column not in table");
    (*result)[col_name] = (*this_row)[col_name];
  }
  return result;
}

// Checks if the given row is acceptable to insert,
// returns dictionary full_row if so.
// Otherwise, throws DBRelationError
ValueDict* HeapTable::validate(const ValueDict* row)
{
  ValueDict* full_row = new ValueDict();
  for (auto& column_name: this->column_names){
    Value value;
    ValueDict::const_iterator column = row->find(column_name);
    if (column == row->end()){
      throw DbRelationError("Error validating");
    }else{
      value = row->at(column_name);
    }
    (*full_row)[column_name] = value;
  }
  return full_row;    
}

// Appends a record to the file, and returns a Handle
// Can make a new block if insufficient space 
Handle HeapTable::append(const ValueDict* row)
{
  Dbt* data = marshal(row);
  SlottedPage* block = this->file.get(this->file.get_last_block_id());
  RecordID record_id;
  try {
    record_id = block->add(data);
  } catch (DbBlockNoRoomError& e){
    block = this->file.get_new();
    record_id = block->add(data);
  }
  this->file.put(block);
  return Handle(this->file.get_last_block_id(), record_id);
}

// return the bits to go into the file
// caller responsible for freeing the returned Dbt
// and its enclosed ret->get_data(). 
Dbt* HeapTable::marshal(const ValueDict* row) {
    char *bytes = new char[DbBlock::BLOCK_SZ]; 
    uint offset = 0;
    uint col_num = 0;
    for (auto const& column_name: this->column_names) {
        ColumnAttribute ca = this->column_attributes[col_num++];
        ValueDict::const_iterator column = row->find(column_name);
        Value value = column->second;
        if (ca.get_data_type() == ColumnAttribute::DataType::INT) {
            *(int32_t*) (bytes + offset) = value.n;
            offset += sizeof(int32_t);
        } else if (ca.get_data_type() == ColumnAttribute::DataType::TEXT) {
            uint size = value.s.length();
            *(u16*) (bytes + offset) = size;
            offset += sizeof(u16);
            std::memcpy(bytes+offset, value.s.c_str(), size); // assume ascii for now
            offset += size;
        } else {
            throw DbRelationError("Only know how to marshal INT and TEXT");
        }
    }
    char *right_size_bytes = new char[offset];
    memcpy(right_size_bytes, bytes, offset);
    delete[] bytes;
    Dbt *data = new Dbt(right_size_bytes, offset);
    return data;
}

// Unmarshals data from the DB
// NOTE: Only INT and TEXT are supported at this time
ValueDict* HeapTable::unmarshal(Dbt* data)
{
  char *bytes = (char*) data->get_data();
  ValueDict *row = new ValueDict();
  uint offset = 0;
  uint col_num = 0;
  Value value;
  for (auto const& column: this->column_names){
    ColumnAttribute ca = this->column_attributes[col_num++];
    value.data_type = ca.get_data_type();
    if (ca.get_data_type() == ColumnAttribute::DataType::INT){
      value.n = *(int32_t*)(bytes + offset);
      offset += sizeof(int32_t);
    } else if (ca.get_data_type() == ColumnAttribute::DataType::TEXT){
      u16 size = *(u16*)(bytes + offset);
      offset += sizeof(u16);
      char buffer[DbBlock::BLOCK_SZ];
      std::memcpy(buffer, bytes + offset, size);
      buffer[size] = '\0';
      value.s = std::string(buffer);
      offset += size;
    } else {
      throw DbRelationError("Only INT and TEXT currently supported");
    }
    (*row)[column] = value;
  }
  return row;    
}

// test function -- returns true if all tests pass
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
    std::cout << "create ok" << std::endl;
    table1.drop();
    std::cout << "drop ok" << std::endl;
    HeapTable table("_test_data_cpp", column_names, column_attributes);
    table.create_if_not_exists();
    std::cout << "create_if_not_exsts ok" << std::endl;
    ValueDict row;
    row["a"] = Value(12);
    row["b"] = Value("Hello!");
    std::cout << "try insert" << std::endl;
    table.insert(&row);
    std::cout << "insert ok" << std::endl;
        
    Handles* handles = table.select();

    std::cout << "select ok " << handles->size() << std::endl;
    ValueDict *result = table.project((*handles)[0]);
    std::cout << "project ok" << std::endl;
    Value value = (*result)["a"];
    if (value.n != 12)
        return false;
    value = (*result)["b"];
    if (value.s != "Hello!")
        return false;
    table.drop();
    return true;
}
