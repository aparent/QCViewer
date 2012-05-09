.v  f11 gr11 f10 gr10 f9 gr9 f8 gr8 f7 gr7 f6 gr6 f5 gr5 f4 gr4 f3 gr3 f2 gr2 f1 gr1 r f gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
.i  0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 
.o  f11 gr11 f10 gr10 f9 gr9 f8 gr8 f7 gr7 f6 gr6 f5 gr5 f4 gr4 f3 gr3 f2 gr2 f1 gr1 r f gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
.ol  f11 gr11 f10 gr10 f9 gr9 f8 gr8 f7 gr7 f6 gr6 f5 gr5 f4 gr4 f3 gr3 f2 gr2 f1 gr1 r f gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z

BEGIN MAJ(1 2 3)
tof 3 2
tof 3 1
tof 2 1 3
END MAJ

BEGIN IMAJ(1 2 3)
tof 2 1 3
tof 3 1
tof 3 2
END MAJ

BEGIN UMA (1 2 3)
tof 2 1 3
tof 3 1
tof 1 2
END UMA 

BEGIN ADDER(0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z)
MAJ 0 b0 a0
MAJ a0 b1 a1
MAJ a1 b2 a2
MAJ a2 b3 a3
MAJ a3 b4 a4
MAJ a4 b5 a5
tof a5 z
UMA a4 b5 a5
UMA a3 b4 a4
UMA a2 b3 a3
UMA a1 b2 a2
UMA a0 b1 a1
UMA 0 b0 a0
END ADDER

BEGIN SUBTRACTOR(0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z)
tof a0; tof a1; tof a2; tof a3; tof a4; tof a5
ADDER 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
tof b0; tof a0
tof b1; tof a1
tof b2; tof a2
tof b3; tof a3
tof b4; tof a4
tof b5; tof a5
END SUBTRACTOR

BEGIN COMPARATOR(0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z)
tof a0; tof a1; tof a2; tof a3; tof a4; tof a5

MAJ 0 b0 a0
MAJ a0 b1 a1
MAJ a1 b2 a2
MAJ a2 b3 a3
MAJ a3 b4 a4
MAJ a4 b5 a5
tof a5 z
IMAJ a4 b5 a5
IMAJ a3 b4 a4
IMAJ a2 b3 a3
IMAJ a1 b2 a2
IMAJ a0 b1 a1
IMAJ 0 b0 a0

tof a0; tof a1; tof a2; tof a3; tof a4; tof a5
END COMPARATOR

BEGIN ITER(f1 r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z)
COMPARATOR 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred z gr
fred gr b0 a0
fred gr b1 a1
fred gr b2 a2
fred gr b3 a3
fred gr b4 a4
fred gr b5 a5
SUBTRACTOR r : 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
tof r b0' b1' b2' b3' b4' b5' f1
fred gr b5 a5
fred gr b4 a4
fred gr b3 a3
fred gr b2 a2
fred gr b1 a1
fred gr b0 a0
END ITER

BEGIN 
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f1; fred gr gr1; tof f1 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f2; fred gr gr2; tof f2 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f3; fred gr gr3; tof f3 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f4; fred gr gr4; tof f4 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f5; fred gr gr5; tof f5 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f6; fred gr gr6; tof f6 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f7; fred gr gr7; tof f7 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f8; fred gr gr8; tof f8 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f9; fred gr gr9; tof f9 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f10; fred gr gr10; tof f10 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
fred f f11; fred gr gr11; tof f11 r;
ITER f r gr 0 b0 a0 b1 a1 b2 a2 b3 a3 b4 a4 b5 a5 z
END
