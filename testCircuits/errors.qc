# This is meant to include all errors that we should accept without crashing for testing purposes
.v first,second,third,fourth,fifth
.i first,second,third,fourth,fifth
.o first,second,third,fourth,fifth
BEGIN
t0 first third test #unknown wire
t1 first,first #double wire ref
t3 first,second,third,fourth
f first,second,fourth
t1 first,fourth
H second
END
