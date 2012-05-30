.v a b c d e Workspace test
.i a b c d e Workspace test
.o a b c d e 
.c

BEGIN S1 (1 2 3 4)
Y 2
S2 3 4
RX(0.5) 1
RY(0.5) 1
RZ(0.5) 1
H 2
S2^4 1 3
H 3
S2 1 2
X 4
END S1

BEGIN S2 (1 2)
X 1
Y 2
T 1 2
END S2

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
tof a b c d
not b c d e
S1 a b d c 
GroverIterate test : a b c d e Workspace 
tof a b c d
S1 b c d e 
TOF a b c d
NOT b c d e
END
