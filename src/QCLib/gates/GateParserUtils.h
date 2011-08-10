
struct row_terms{
			row_terms(row_terms *n_next){next = n_next; val = NULL;}	
			~row_terms(){
				if (next!= NULL) delete next;
				if (val != NULL) delete val;
			}	
			complex<float> *val;
			rowTerm *next
}

struct matrix_row{
		matrix_row(MatrixRow *n_next){next = n_next; terms = NULL;}	
		~matrix_row(){
			if (next!= NULL) delete next;
			if (terms != NULL) delete terms;
		}	
		row_terms *terms;
		matrix_row *next;
}	

struct gate_node{
	gate_node(){next = NULL; row = NULL;}
	~gate_node(){
		if (next!= NULL) delete next;
		if (row != NULL) delete row;
	}
	gate_node* next;
	string name;
	matrix_row *row;
}	
