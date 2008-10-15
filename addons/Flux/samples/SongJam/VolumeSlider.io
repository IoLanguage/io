VolumeSlider := HSlider clone do (

  init := method (
    resend
   ImageManager addPath( Path with(System launchPath,  "Images" ) )
    //self background := Image clone open( Path with(System launchPath,  "Images/VolumeBack.png" ) )
    self background := ImageManager itemOrNil( "VolumeBack" )
  )

  draw := method (
    //textures draw( width, height )
    glTranslated( 0, 4, 0 )
    background bindTexture
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE )
    glColor3d( 1, 0, 1 )
    background drawTexture( background width, background height )
    knob setWidth( 15 )
  )

)

