#ifndef PARSE_NODE
#define PARSE_NODE

#include <string>

struct parseNode{
		std::string value;
		parseNode * left;
		parseNode * right;
};		 

#endif
