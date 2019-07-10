# 5300-Butterfly
##"Project Butterfly" - CPSC 5300 / 4300 Summer 2019
####	'relation manager' project

Sprint Verano:
		- Jonathan Kimray
		- Grant Bishop
Sprint Otono:
		- Your Name
		- Your Name
Sprint Invierno:
		- Your Name
		- Your Name

---

## Sprint Verano

### Milestone 1: Skeleton:
	
	- Berkeley DB environment creation: handled by the 'Berkeley' class (Berkeley.h, .cpp)
		- Set up environment using constructor, i.e. Berkeley myDB("~/path/to/db/dir/")
		- Closes with destructor
		- Write and Read functions exist in header only 
		- Change for later milestones to integrate with storage engine!
	- Hyrise SQL query result unparsing: handled by the 'Execute' static class (Execute.h, .cpp)
		- Only one public function: Execute::getString(SQLParserResult*)
			- Doesn't support all Hyrise features, just a basic subset
		- Calls several private functions to unparse different elements of the parse tree
		- Test unparsing with TEST_Execute.cpp
			- Build with queryTest target, i.e. make queryTest
			- Tests all queries shown on Milestone 1 canvas page, e.g. CREATE TABLE foo (a TEXT, b INT...
			- Some additional tests - not all are supposed to pass
	- sql5300 program: in main.cpp, default build target in Makefile
			- Running: provide directory for BerkeleyDB environment as command line argument
			- Sets up environment, then goes to SQL unparsing prompt using Execute

### Milestone 2: Rudimentary Storage Engine

## Sprint Otono

To be continued ...
