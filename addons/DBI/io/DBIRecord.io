//metadoc DBIRecord copyrigth Jeremy Cowgar, 2006
//metadoc DBIRecord license BSD revised
//metadoc DBIRecord category Databases
/*metadoc DBIRecord description
A DBI Record. When utilizing `foreach' or `populate' methods
of a DBIResult object, you can pass an optional Object cloned from DBIRecord.
This object will be populated with the row contents making it possible to
write objects that represent your SQL results. A simple example would be:

<pre>
Person := DBIRecord clone do (fullName := method(firstName.." "..lastName))
q := conn query("SELECT id, firstName, lastName FROM people")
q foreach(Person, p, writeln("Name = ", p fullName))
</pre>

As you can see, fullName was not in the SQL query, however, a dynamic method
in your Person class.

DBIRecord in and of itself provides no real functionality. It simply acts
as an Object and stores the values from the SQL query into a Map. You can
access the field information:

<pre>
o := r populate(Person)
o firstName            // would retrieve the firstName value of the SQL query
o setFirstName("John") // would update the object's firstName value to be John
</pre>

Do not confuse the above example as updating the actual database. The call
to setFirstName <i>only</i> updates the objects representation of firstName.
*/

DBIRecord := Object clone do(
	init := method(
		resend
		self _map := Map clone
	)

	//doc DBIRecord with(aMap) Returns a new DBIRecord instance for the given value map.
	with := method(aMap,
		obj := self clone
		obj _map := aMap
		obj
	)

	//doc DBIRecord forward Private method that implements getting and setting values.
	forward := method(
		m := call message

		if(m argCount == 1 and m name beginsWithSeq("set"),
			k := m name afterSeq("set") asMutable makeFirstCharacterLowercase \
						asSymbol

			_map atPut(k, call evalArgAt(0))
			? _changed append(k)

			return self
		)

		_map at(call message name)
	)
)
