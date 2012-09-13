VERSION "1.0" 

IMPORT carrySelect FROM "adder.qc"

BEGIN test (a b c) 
  BEGIN HEADER
    Author = "blah"
    Description = "Blah"
  END HEADER
  BEGIN BITINFO
    INLABELS a : ain , b : stuff
    OUTLABELS a : aout , c : garbage
    INIT a : |0>+|1> , c b : |00> + |11>
  END BITINFO
  BEGIN GATES
    H a
    toff b c 
    :NEWBIT n
    toff b n
    toff b n
    :FREE n
  END GATES
END test
