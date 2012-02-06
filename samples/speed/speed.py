import sys
import time

oneMillion = 1000000

if sys.version_info >= (3,):
  def xrange(v):
    return range(int(v))

class Tester:
  def foo(self):
    return 1
    
  def beginTimer(self):
    self.t1 = time.time()
    
  def endTimer(self, s):
    mps = 1/(time.time() - self.t1)
    print("Python %s := %0.2f" % (s, mps))
   
  def testSlot(self):
    self.beginTimer()
    self.x = 1
    for i in xrange(oneMillion/8):
      self.x; self.x; self.x; self.x; 
      self.x; self.x; self.x; self.x;
    self.endTimer("slotAccesses       ")

  def testSetSlot(self):
    self.beginTimer()
    self.x = 1
    for i in xrange(oneMillion/8):
      self.x = 1; self.x = 2; self.x = 3; self.x = 4; 
      self.x = 1; self.x = 2; self.x = 3; self.x = 4; 
    self.endTimer("slotSets           ")

  def testBlock(self):
    self.beginTimer()
    for i in xrange(oneMillion/8):
      self.foo(); self.foo(); self.foo(); self.foo(); 
      self.foo(); self.foo(); self.foo(); self.foo()
    self.endTimer("blockActivations   ")

  def testInstantiations(self):
    self.beginTimer()
    for i in xrange(oneMillion/8):
      Tester(); Tester(); Tester(); Tester();
      Tester(); Tester(); Tester(); Tester();
    self.endTimer("instantiations     ")

  def testLocals(self):
    self.beginTimer()
    v = 1
    for i in xrange(oneMillion/8):
      v; v;  v; v;  
      v; v;  v; v;  
    self.endTimer("localAccesses      ")

  def testSetLocals(self):
    self.beginTimer()
    v = 1
    for i in xrange(oneMillion/8):
      v = 1; v = 2; v = 3; v = 4;  
      v = 1; v = 2; v = 3; v = 4;  
    self.endTimer("localSets          ")
    
  def test(self):
    print("")
    self.testLocals()
    self.testSetLocals()
    print("")
    self.testSlot()
    self.testSetSlot()
    print("")
    self.testBlock()
    self.testInstantiations()

    import sys
    print("Python version := \"%i.%i.%i %s %i\"" % tuple(sys.version_info))
    print("")
    print("// values in millions per second")
    print("")


Tester().test()

