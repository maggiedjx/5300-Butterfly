/**
**@file btree.cpp - implementation for B+Tree
**@author Kevin Lundeen, Nina Nguyen, Maggie Dong
**@See "Seattle University, CPSC5300, Summer 2019"
**/

#include "btree.h"
#include <iostream>
using namespace std;

BTreeIndex::BTreeIndex(DbRelation& relation, Identifier name, ColumnNames key_columns, bool unique)
        : DbIndex(relation, name, key_columns, unique),
          closed(true),
          stat(nullptr),
          root(nullptr),
          file(relation.get_table_name() + "-" + name),
          key_profile() {
    if (!unique)
        throw DbRelationError("BTree index must have unique key");
	// FIXME - what else?! NINA
	build_key_profile();
}

//M6 - NINA
//Figure out the data types of each key component
void BTreeIndex::build_key_profile(){
	for (ColumnAttribute col: *relation.get_column_attributes(this->key_columns)){
		key_profile.push_back(col.get_data_type());
	}
}

BTreeIndex::~BTreeIndex() {
	// FIXME - free up stuff NINA
	delete this->stat;
	delete this->root;
	this->stat = nullptr;
	this->root = nullptr;
}

// Create the index.
void BTreeIndex::create() {
	// FIXME NINA
	this->file.create();
	this->stat = new BTreeStat(file, STAT, STAT + 1, key_profile);
	this->root = new BTreeLeaf(file, stat->get_root_id(), key_profile, true);
	this->closed = false;

	//now build the index! -- add every row from relation into index
	Handles* handles = relation.select();
	Handles* n_handles = new Handles();

	for (auto const handle : *handles){
		insert(handle);
		n_handles->push_back(handle);
	}

	delete handles;
	delete n_handles;
}

// Drop the index.
void BTreeIndex::drop() {
	// FIXME NINA
	file.drop();
}

// Open existing index. Enables: lookup, range, insert, delete, update.
void BTreeIndex::open() {
	// FIXME NINA
	if(this->closed){
		file.open();
		this->stat = new BTreeStat(file, STAT, key_profile);
		if (this->stat->get_height() == 1){
			this->root = new BTreeLeaf(file, stat->get_root_id(), key_profile, false);
		} else {
			this->root = new BTreeInterior(file, stat->get_root_id(), key_profile, false);
		}
		this->closed = false;
	}
}

// Closes the index. Disables: lookup, range, insert, delete, update.
void BTreeIndex::close() {
	// FIXME MAGGIE
}

// Find all the rows whose columns are equal to key. Assumes key is a dictionary whose keys are the column
// names in the index. Returns a list of row handles.
Handles* BTreeIndex::lookup(ValueDict* key_dict) const {
	// FIXME MAGGIE
	return nullptr;
}

Handles* BTreeIndex::range(ValueDict* min_key, ValueDict* max_key) const {
    throw DbRelationError("Don't know how to do a range query on Btree index yet");
    // FIXME: Not in scope for M6
}

// Insert a row with the given handle. Row must exist in relation already.
void BTreeIndex::insert(Handle handle) {
	// FIXME MAGGIE
}

void BTreeIndex::del(Handle handle) {
    throw DbRelationError("Don't know how to delete from a BTree index yet");
	// FIXME: Not in scope for M6
}

KeyValue *BTreeIndex::tkey(const ValueDict *key) const {
	return nullptr;
	// FIXME MAGGIE
}


bool test_btree(){
	//TODO
	return false;
}
