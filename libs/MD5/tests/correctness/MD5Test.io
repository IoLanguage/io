
MD5Test := UnitTest clone do(

	testBasic := method(
		correct := "12f05c669a0c27f07ed68b8af739a558"
		
		data := """Working week's come to its endParty time is here againEveryone can come if they want toIf you want to be with meIf you want to be with meYou can come with me if you want toExercise your basic rightWe could build a building siteFrom the bricks of shame is built the homeIf you want to be with meIf you want to be with meEven though you may still not want toLet tomorrow and todayBring a life of ecstasyWipe away your tears of confusionIf you want to be with meIf you want to be with meYou can come with me if you want toEven though you may still not want to"""
		
		digest := MD5 clone
		digest appendSeq(data)
		v := digest md5String

		assertEquals(v, correct)
	)
)

