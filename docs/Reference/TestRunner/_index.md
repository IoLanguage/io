# TestRunner

Core testing object responsible for running tests, collected by UnitTests and TestSuites.

- **category**: Testing

## name

Returns the name of the TestRunner.

## run(testMap)

Runs all tests from a given Map object, where keys are names of the UnitTests
to run and values - lists of test slots theese UnitTests provide.

Returns a `list` which is empty if the tests passed or contains exceptions on
failures.

## testCount

Returns the number of tests to be ran.

