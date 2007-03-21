$oneMillion = 1000000

class Tester
  attr_accessor :t1, :x

  def foo
    1
  end

  def beginTimer
    @t1 = Time.now.to_f
  end

  def endTimer(s)
    mps = 1.00 / (Time.now.to_f - @t1)
    printf "Ruby %s := %0.2f\n", s, mps
  end

  def testSlot
    beginTimer()
    @x = 1
    ($oneMillion/8).times {
      x; x; x; x;
      x; x; x; x;
    }
    endTimer("slotAccesses       ")
  end

  def testSetSlot
    beginTimer
    @x = 1
    ($oneMillion/8).times {
      @x = 1; @x = 1; @x = 1; @x = 1;
      @x = 1; @x = 1; @x = 1; @x = 1;
    }
    endTimer("slotSets           ")
  end

  def testBlock
    beginTimer
    ($oneMillion/8).times {
      foo; foo; foo; foo;
      foo; foo; foo; foo;
    }
    endTimer("blockActivations   ")
  end

  def testInstantiations
    beginTimer
    ($oneMillion/8).times {
      Tester.new; Tester.new; Tester.new; Tester.new;
      Tester.new; Tester.new; Tester.new; Tester.new;
    }
    endTimer("instantiations     ")
  end

  def testLocals
    beginTimer
    v = 1
    ($oneMillion/8).times {
      v; v; v; v;
      v; v; v; v;
    }
    endTimer("localAccesses      ")
  end

  def testSetLocals
    beginTimer
    v = 1
    ($oneMillion/8).times {
      v = 1; v = 2; v= 3; v= 4;
      v = 1; v = 2; v= 3; v= 4;
    }
    endTimer("localSets          ")
  end

  def test
    puts ""
    testLocals
    testSetLocals
    puts ""
    testSlot
    testSetSlot
    puts ""
    testBlock
    testInstantiations
    printf "Ruby version := \"%s\"", RUBY_VERSION
    puts ""
    puts "// values in millions per second"
    puts ""
  end
end

begin
  Tester.new.test
end