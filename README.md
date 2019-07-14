# 5300-Butterfly
## "Project Butterfly" - CPSC 5300 / 4300 Summer 2019
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
			- Build with queryTest target, i.e. make queryTest (additional buld target in makefile)
			- Tests all queries shown on Milestone 1 canvas page, e.g. CREATE TABLE foo (a TEXT, b INT...
			- Some additional tests - not all are supposed to pass
	- sql5300 program: in main.cpp, default build target in Makefile
			- Running: provide directory for BerkeleyDB environment as command line argument
			- Sets up environment, then goes to SQL unparsing prompt using Execute

### Milestone 2: Rudimentary Storage Engine

#### To start: we are sorry
 Our time managment was ... less than stellar.  The issue:
    The program does compile, but it does not run.  the test_heap_storage function does not complete.  A segfault crashes the program.  We spent about 12 hours on Sunday (yeah poor time managment!) trying to figure out why.  We have an idea of where the probelm occurs - search for "SORRY" in heap_file.cpp to find comments associated with two functions explaining the issue so far.  Maybe fresh eyes will find the issue quickly?  We are going to office hours on Monday to get something worked out so that you, the next Team Butterfly won't be completely up a creek without a paddle.
    Since we did not finish testing, there are LIKLEY ADDITIONAL BUGS.  

    - With appologies, Grant and Jon

#### Overview of code:

    There isn't much to say here (since I have 15 min before midnight!): are code essentilly follows the provided heap_storage.h and python examples without deviation (except for wherever our bug(s) are).

    - storage engine.h: this is just the proivided file
    - heap storage.h: this is almost just the provided file
    - heap storage.cpp: our implemention.  Functions provided on canvas where used exactly as is
        - implemention of other function follows very close to the python.
    - main.cpp: nothing special here, quite similar to the provided sql5300.cpp

    - Makefile: still has the additional queryTest target for milestone 1.
    - MakefileDebug: this makefile has some additional options for debugging.  use with 'make -f MakefileDebug [buildTarget]'.  Additional arguments are passed to g++ to allow the use of the gdb debugger.  This is what got us as far as we did in trying to find our problem.  In addition to the sql5300 build target there is a TEST_SP target.  This was used with TEST_SP.cpp when we where in the early stages of trying to locate the cause of our segfault.  



## Sprint Otono

To be continued ...
