Person := DBIRecord clone do (
	fullName := method(first .. " " .. last)

	age := method(
		dur := Duration fromNumber(dob secondsSinceNow)
		return dur years)
)