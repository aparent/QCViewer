#lables (optional)
#notes: 1. input labels default to line name if not specified 
#       2. output labels default to input if not specified 
#       3. lines that are constant are labeled with their value if not otherwise specified
#       4. lines which are garbage default to "garbage" as outlabel if not otherwise specified
#lines:   space separted list of line names (if not specified you can use 1,2,3,4,5,6)
#in_lab:  space separted list of mappings from line names to input lables ( ex. 1:first )
#out_lab: space separted list of mappings from line names to output lables ( ex. 2:result )
#constant:space separted list of mappings from line names their input constants ( ex. 2:1 )
#garbage: space separted list of lines which are garbage

circuit a
lines a b c
h a
x a b
x c b

circuit main
h 1
a 3->a 2->c 4->b

y 3
rx[pi/2] 4
