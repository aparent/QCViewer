#Use the input state "|0>^5|1>".  After each iteration you should see the state corresponding 
#the toffoli oracle increase in amplitude maximizing at the 4th iteration.

.v a b c d e Workspace
.i a b c d e Workspace
.o a b c d e
.c
BEGIN H5(a b c d e)
H a
H b
H c
H d
H e
END H5

BEGIN GroverIterate (a b c d e Workspace)
H Workspace
T5 a' b' c d e' Workspace
H Workspace

H5 a b c d e

X e
Z a' b' c' d' e
X e

H5 a b c d e
END GroverIterate

BEGIN
H5 a b c d e

GroverIterate^4 a b c d e Workspace 
END
