SGML

/*
Sequence diff := method(s,
	self foreach(i, c, 
		if(c != s at(i),
			write(c asCharacter)
		)
	)
	writeln("")
)
*/

SGMLTest := UnitTest clone do(
	testFull := method(
		s := """<html>
				<p attribute="o'brian">
				<body bgcolor="black''''">
						<!-- This is a test comment -->
						<!--This is a test comment2-->
						<!--This is a test comment3 -->
						<form name="bobby" method="post">
								<input type='hidden' name='action' value='go'>
								<input type="testing" name="startat" value="">
								<input type='hidden' name='action' value='go'>
								<p>
										What's up
								</p>
								<input type="submit" value="submit" name="submit"/>
						</form>
				</body>
		</html>"""

				
		s1 := s asHTML asString 
		s2 := s1 asHTML asString 
		//s1 := s2 asHTML asString 
		
		assertEquals(s1, s2)
		//assertEquals(s1 asHTML subitems first subitems at(1) name, "p")
		//assertEquals(s1 size, 467)
	)
)