//
// ProgressPanel.io
//

ProgressPanel := Panel clone do (

  progressBar := nil
  progress    := 0
  bar         := Point clone
  barColor    := Color clone set( 0, 0, 1 )
  
  draw := method (
    resend
    
    //barColor glColor
    bar setX( width * progress )
    bar setY( height )
    bar drawQuad
  )

)

