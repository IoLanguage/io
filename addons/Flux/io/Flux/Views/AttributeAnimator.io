
View animator := method(AttributeAnimator setWindow(window) setTarget(self))

AttributeAnimator := Object clone do(
    window ::= nil
    
    target ::= nil
    attributeName ::= "position"
    startValue ::= nil
    value ::= nil
    endValue ::= nil
    
    duration ::= .5
    frameRate ::= 30
    frameNumber := 0
    maxFrame := 0
    
    endAction ::= nil
    
    run := method(
        value = target perform(attributeName)
        startValue = value clone
        maxFrame = duration * frameRate
        frameNumber = 0
        self delay := 1/frameRate
        timer        
    )

    timer := method(
        yield
        //writeln("AttributeAnimator timer")
        v := frameNumber/maxFrame
        nextValue := startValue + ((endValue - startValue) * v)
        value copy(nextValue)
        window postRedisplay
        if(frameNumber != maxFrame, 
            window addTimerTargetWithDelay(self, delay)
        , 
            if(endAction, target perform(endAction))
        )       
        frameNumber = frameNumber + 1
    )
)

