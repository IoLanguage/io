
SplitViewBar := View clone do(
    setBackgroundColor(Color clone set(1,1,1,.1))
    setWidth(4)
    leftMouseDown := method(makeFirstResponder)
    leftMouseMotion := method(superview barLeftMouseMotion)
    leftMouseUp := method(releaseFirstResponder)
    draw := method(drawBackground)
)

SplitView := View clone do(
    setResizeWidth(101)
    setResizeHeight(101)
    
    init := method(
        self sideOne := View clone
        self bar     := SplitViewBar clone setResizeWidth(010) setResizeHeight(101)
        self sideTwo := View clone
        self ratio := .5
        self orientation := "horizontal"
        addSubview(sideOne)
        addSubview(bar)
        addSubview(sideTwo)
        sideOne resizeWithSuperview
        sideTwo resizeWithSuperview
        bar setResizeHeight(101) setResizeWidth(110)
    )
    
    updateRatio := method(r,
        ratio = r
        arrange
    )
    
    arrange := method(
        if(orientation == "horizontal", arrangeHorizontal, arrangeVertical)
    )
    
    arrangeHorizontal := method(
        sideOne resizeTo(width*ratio, height)
        bar resizeTo(bar width, height)
        bar placeRightOf(sideOne)
        sideTwo resizeTo(width*(1-ratio) - bar width, height)
        sideTwo placeRightOf(bar)
        glutPostRedisplay
    )
    
    didChangeSize := method(arrange)
    
    barLeftMouseMotion := method(
        updateRatio(viewMousePoint x/superview width)
    )
    
)