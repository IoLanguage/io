Selectable := Object clone do(
	isSelected ::= false

	unselect := method(setIsSelected(false))
	select   := method(setIsSelected(true); setMoveOffset; self)
	toggleSelection := method(if(isSelected, unselect, select))
)
