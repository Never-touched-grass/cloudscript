# CloudScript
Contents:
1. How to use
2. What it does
3. How it was made
4. Examples
# How to use
Unfortunately the installer for the latest version is not available. It will be available soon.
# What it does
CloudScript does 3 things to run its code:

1. Perl script collects filepath and:

- Throws an error if file is not found (relative or absolute path)

- If the file is found, pipes the content of the file into receiver.exe (see below) -->

2. `receiver.exe` collects the lines that the perl file pipes into it and then interprets it line-by-line.

3. As you can see, there is also a utils.hpp for functions that are not needed to be stored in the main file.

# Examples
the `println` statement can be used like this:
```
// Comments like this are ignored
/*
-----------------
Multiline comment
-----------------
*/

println "Hello, World!"
// Above prints the statement in the quotes and starts a new line.
// Variable declaration:
let myVar = "World!"
println myVar
// Above prints myVar individually
println "Hello, &myVar"
// To interpolate variable into statements, use &varName
// To print without a newline statement, simply use `print` instead of `println`
print "Hello, World!"
// Above prints the quoted string without starting a new line.
println " "
// Start a new line
// Different kinds of things able to use in a let statement:
// Expressions
let x = 1 + 2 * 3
// Parentheses also supported
// Vectors(not inside let statements, needed for later)
vec x 1 2 3 4 5
// I will clean up vector assignment in a later update. The above creates a vector with the items 1, 2, 3, 4, and 5
// Push items
push x 6
// Vector items
let veci = vecitem x 0
// Above accesses item at index 0(vectors are zero-indexed)
// File reading
let content = readfile C:/Users/yourNameHere/myfile.txt
// Above reads a file using absolute path(absolute path is required)
// Other types too
let myInt = 23
let myFloat = 23.22
let myStr = "Hello, World!"
// Reassign
re myInt = 5
// If statements
if 1 + 2 * 3 == 9
println "1 + 2 * 3 is 9"
end

//Logical operators
if 1 + 2 * 3 == 9 && 1 + 2 == 2
  println "Both are true"
end
// Statements with false conditions are skipped entirely
if 1 + 2 * 4 == 9
println "This won't execute anyway."
end
// Obtain user input
let myVar = ""
print "Enter a value: "
read myVar
// For loops
for i in 10
println "Loop iteration &i"
end
// Iterate over a variable
let iter = 10
for j in &iter
println "Loop iteration &j"
end
// Write to a file
// Overwriting
writefile < C:/file.txt "Some content here"
// Appending
writefile > C:/file.txt "added content"
// Call system commands
system echo "Hello, World!"
// Functions
fnc sayHello
println "Hello"
end
// Call it
call sayHello
// Functions with arguments
fnc funcWithArgs
args name age
println "Your name is &name and you are &age years old."
end
// Call with args
call funcWithArgs John 34
// Thats it for the latest version. DO NOT install the current release, as it is not the latest.
```
