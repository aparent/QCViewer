#ifndef QC_PARSER_UTILS
#define QC_PARSER_UTILS
#include<iostream>
#include<string>
class name_node{
	public:
		name_node(std::string n_name, name_node *n_next){
			//std::cout << n_name << std::endl;
			name = n_name;
			next = n_next;
			neg = false;
		}
		name_node(std::string n_name, name_node *n_next, bool n_neg){
			//std::cout << "NEG:"  << n_name << std::endl;
			name = n_name;
			next = n_next;
			neg = n_neg;
		}
		std::string name;
		bool neg;
		name_node *next;
};
#endif
