.v a b c d e workspace
.i a b c d e workspace
.o a b c d e
.c
BEGIN H5
H a; H b; H c; H d; H e
END H5

BEGIN GroverIterate (a, b, c, d, e, workspace)
H workspace
T a' b' c d e' workspace
H workspace

H5 a b c d e

X e
Z a' b' c' d' e
X e

H5 a b c d e
END GroverIterate

BEGIN
GroverIterate^4 a b c d e workspace 
END
