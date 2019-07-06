// TODO our header here


#ifndef EXECUTE_H
#define EXECUTE_H

// Include SQL parser that was adapted from Hyrise
#include "SQLParser.h"


class Execute {
public:

	// convert the parse tree from Hyrise back into a string
	static std::string getString(SQLParserResult* parseTree);

private:
};




#endif
