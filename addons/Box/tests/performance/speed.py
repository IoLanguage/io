from numarray import *
import sys
import time
import math

m = 30000
max = 3000
flops = 1000000000.0

a = arrayrange(m)
b = ones(m)

print "// vector size = %i" % a.size()
print "// iterations = %i" % max
print "// values are in GFLOPS"
print "Python := Object clone do("


t1 = time.time()
for j in xrange(m):
    a[j] = sin(a[j])
t = time.time() - t1
print "  sin               := %0.7f" % ((5*m)/(t*flops))

t1 = time.time()
for i in xrange(max):
    a.mean()
t = time.time() - t1
print "  mean              := %0.2f" % ((max*m)/(t*flops))

t1 = time.time()
for i in xrange(max):
    b *= b
    math.sqrt(b.mean())
t = time.time() - t1
print "  rms               := %0.2f" % ((max*m)/(t*flops))

print ""




t1 = time.time()
for i in xrange(max):
    a *= b
t = time.time() - t1
print "  timesEquals       := %0.2f" % ((max*m)/(t*flops))

t1 = time.time()
for i in xrange(max):
    a += b
t = time.time() - t1
print "  plusEquals        := %0.2f" % ((max*m)/(t*flops))

t1 = time.time()
for i in xrange(max):
    a -= b
t = time.time() - t1
print "  minusEquals       := %0.2f" % ((max*m)/(t*flops))

print ""



t1 = time.time()
for i in xrange(max):
    a *= 1
t = time.time() - t1
print "  timesEqualsScalar := %0.2f" % ((max*m)/(t*flops))

t1 = time.time()
for i in xrange(max):
    a += 1
t = time.time() - t1
print "  plusEqualsScalar  := %0.2f" % ((max*m)/(t*flops))

t1 = time.time()
for i in xrange(max):
    a -= 1
t = time.time() - t1
print "  minusEqualsScalar := %0.2f" % ((max*m)/(t*flops))

print ""



t1 = time.time()
for i in xrange(max):
    a *= a
t = time.time() - t1
print "  square            := %0.2f" % ((max*m)/(t*flops))

t1 = time.time()
for i in xrange(max):
    dot(a, b)
t = time.time() - t1
print "  dot               := %0.2f" % ((max*m)/(t*flops))



print ""
print "  version := \"%i.%i.%i %s %i\"" % sys.version_info
print ")"
print ""



