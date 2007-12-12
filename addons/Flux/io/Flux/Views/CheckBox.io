
CheckBox := Button clone do(	
	position setX(100) setY(100)
	isSelected := 0
	image := ImageManager item("CheckBox")
	altImage := ImageManager item("CheckBoxChecked")
	size setWidth(26) setHeight(26)
	title := "CheckBox"
	motionAction := nil
	
	setImage := method(v, self image := v)
	setAltImage := method(v, self altImage := v)
	sizeToImage := method(
	  if (image, self size setWidth(image width) setHeight(image height))
	)
	
	sizeToImage
	
	draw := method(
		if (self image, 
			if (isSelected == 0, image draw, if (altImage, altImage draw, image draw))
			return
		)
	)
	
	drawBackground := method(self)
	
	motion := method(
		"CheckBox motion\n" print
		isSelected  := if (self screenHit, 1, 0)
		write("isSelected := ", isSelected, "\n")
		//if(isSelected == 1, 
		if (self target and self motionAction, self target perform(motionAction, self)) 
		//)
		glutPostRedisplay
	)
)
