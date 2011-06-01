# (c) Copyright 2004-2009 Dmitri Maslov and Igor Markov
# Distributing this file without an express written permission or erasing this note is prohibited.
.v x1,x2,x3,x4,x5,s1,s2,s3
.i x1,x2,x3,x4,x5
.o x1,x2,x3,x4,x5
.c 0,02,03
BEGIN
# Cylcle3_1 application to x1 and s_i
# t4 x1,s1,s2,s3
# t3 x1,s1,s2
t2 x1,s1
# Cylcle3_1 application to x2 and s_i
# t4 x2,s1,s2,s3
t3 x2,s1,s2
t2 x2,s1
# Cylcle3_1 application to x3 and s_i
# t4 x3,s1,s2,s3
t3 x3,s1,s2
t2 x3,s1
# Cylcle3_1 application to x4 and s_i
t4 x4,s1,s2,s3
t3 x4,s1,s2
t2 x4,s1
# Cylcle3_1 application to x5 and s_i
t4 x5,s1,s2,s3
t3 x5,s1,s2
t2 x5,s1
# Fredkins with control=s1 and swapping distance 1
f3 s1,x1,x2
f3 s1,x3,x4
f3 s1,x5,x1
f3 s1,x3,x5
# Fredkins with control=s2 and swapping distance 2
f3 s2,x1,x3
f3 s2,x2,x4
f3 s2,x5,x2
f3 s2,x1,x5
# Fredkins with control=s3 and swapping distance 4
f3 s3,x1,x5
f3 s3,x2,x1
f3 s3,x3,x2
f3 s3,x4,x3
END
