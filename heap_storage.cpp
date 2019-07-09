// Heap storage engine components
// Inherits and implements
// abstract base classes from storage_engine.h and heap_storage.h

#include "storage_engine.h"
#include "heap_storage.h"

class SlottedPage : public DbBlock
{  
  public:
  //TODO? byte order

  SlottedPage(Dbt &block, BlockID block_id, bool is_new=false) : DbBlock(Dbt &block, BlockID block_id, bool is_new =false)
  {
  }

  RecordID initialize_new()
  {
    // TODO
  }

  RecordID add(const Dbt* data)
  {
    try {
      // TODO
    }
    catch(...){
      std::cout << "Insufficient space in this block.";
    }
  }

  Dbt* get(RecordID record_id)
  {
    //TODO
  }

  void put(RecordID record_id, const Dbt &data)
  {
    try{
      //TODO
    }
    catch(...){
      std::cout << "Insufficient space in this block.";
    }
  }

  void del(RecordID record_id)
  {
    //TODO
  }

  RecordIDs* ids()
  {
    //TODO
  }

  // TODO type?
  Dbt* _get_header(unsigned int id = 0)
  {
    //TODO
  }

  void _put_header(unsigned int id = 0, unsigned int size, unsigned int loc)
  {
    // TODO
  }

  bool _has_room(unsigned int size)
  {
    // TODO
  }

  void _slide(unsigned int start, unsigned int end)
  {
    //TODO
  }

  unsigned int _get_n(unsigned int offset)
  {
    // TODO
  }

  void _put_n(unsigned int offset, unsigned int n)
  {
    //TODO
  }  

};

class TestSlottedPage
{
public:

  void test_basics()
  {
    // TODO
  }
};

class HeapFile() : public DbFile()
{
  // TODO : constructor?

  void _db_open(unsigned int openflags = 0)
  {

  }

  void create()
  {

  }

  void del()
  {

  }

  void open()
  {

  }

  void close()
  {

  }

  SlottedPage get(unsigned int block_id)
  {

  }

  SlottedPage get_new()
  {

  }

  void put(unsigned int block)
  {

  }

  BlockIDs* block_ids()
  {

  }
};

class HeapTable : public DbRelation
{
public:

  void create()
  {

  }

  void open()
  {

  }

  void close()
  {

  }

  void create_if_not_exists()
  {
    try {

    }
    catch(...){

    }
  }

  void drop()
  {

  }

  // type?
  sometype insert(unsigned int row)
  {

  }

  // param type?
  void del(sometype handle)
  {

  }

  // params??
  void select()
  {

  }

  //params??
  sometype project()
  {

  }

  sometype _validate(unsigned int row)
  {

  }

  // TODO _append and on
