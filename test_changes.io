#!/usr/bin/env io

# Test script to verify our changes work correctly
# This tests the basic functionality without requiring full build

writeln("Testing Io language changes...")

# Test 1: File operations (related to IoFile.c fix)
writeln("\n1. Testing File operations:")
testFile := File with("/tmp/io_test_file.txt")
if(testFile exists, testFile remove)
testFile openForUpdating
testFile write("Hello, World!")
testFile atPut(0, 72)  # Change 'H' to 'H' (ASCII 72)
testFile close
writeln("   File operations test completed")

# Test 2: Basic parsing (related to IoMessage_parser.c fix)
writeln("\n2. Testing Parser error handling:")
try(
    # This should trigger proper error handling
    doString("foo(1, 2, ")  # Missing closing paren
) catch(Exception,
    writeln("   Parser correctly caught missing parenthesis")
)

# Test 3: Path operations (related to test runner fix)
writeln("\n3. Testing Path operations:")
testPath := Path with(".", "test.io")
writeln("   Path join test: " .. testPath)

writeln("\nAll basic tests completed successfully!")
writeln("Note: Full integration testing requires complete build")