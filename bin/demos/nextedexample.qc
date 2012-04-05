.v 1 2
.i 1 2 
.o 1 2 
.ol 1 2
.c

BEGIN sub1(1 2)
tof 1 2
END sub1

BEGIN sub2(1 2)
sub1 1 2
sub1 1 2
sub1 1 2
END sub2

BEGIN sub3(1 2)
sub2 1 2
sub2 1 2
sub2 1 2
END sub3

BEGIN sub4(1 2)
sub3 1 2
sub3 1 2
sub3 1 2
END sub4

BEGIN sub5(1 2)
sub4 1 2
sub4 1 2
sub4 1 2
END sub5

BEGIN sub6(1 2)
sub5 1 2
sub5 1 2
sub5 1 2
END sub6

BEGIN sub7(1 2)
sub6 1 2
sub6 1 2
sub6 1 2
END sub7

BEGIN sub8(1 2)
sub7 1 2
sub7 1 2
sub7 1 2
END sub8


BEGIN
sub8 1 2
END
