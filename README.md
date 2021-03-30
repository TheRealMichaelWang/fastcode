# Fastcode
A perfect blend of C, Java, and Python tailored for those who desire a simple yet powerful programming language. FastCode is a procedural/structural, dynamically typed, and iterpreted programming language. 

Click [here](https://github.com/TheRealMichaelWang/fastcode/raw/main/fastcode.exe) to download.
Click [here](https://github.com/TheRealMichaelWang/fastcode/wiki) for documentation.


# The Philosophy and Rationale
FastCode is meant to be a simple-to-use, portable, and fast programming language that only uses a minimal amount of computing resources. It's best suited for usage as an extension language as well as a general purpose language. FastCode grabs the best features of Python, Java, and C and leaves the worst out while adhering to to it's philosophy.
### Simplified External Dependencies
With any moderatly or large program in C or C++, one has wrestle with complex header-body declerations and all the other messy items (such as headerguards, pragma onces, etc...) that it often entails. These niche dependency systems are hard to learn, bog down the development process, and that often aren't very useful beyond the realm of c and c++.  FastCode automatically handles all aspects of including external dependencies so you can focus purley on writing your code. 
### Variable Access Protection
Often times (in C and C++), varaibles that aren't used globally are allowed to be accessed as globally (all throught the program), which often cluters the identifier namespace and provides a myriad of naming, accessibilty issues. In FastCode, each varaible (aside from global variables) cannot be accessed outside of the call frame it was declared in, and will be deleted once that call frame is finished. 
### Minimal Verbosity
For small programs or snippets, extreme verbosity is cumbersome and often self defeating since small snippets of code doesn't need to document it's functionality. And frankly, if you don't have an IDE that does extreme auto-suggest/complete you're going to waste more time typing. This is why FastCode is a superb extension language, especially if you only need to do some minor scripting within your application.
### Small Project Sizes
With C# or Java your soloution would have a dozen different, often reduntant and unnecessary configuration files. With FastCode, you do not need to load a million different configuration files with your project. This is a waste of resources especially when it comes to small programs/snippets. FastCode on the other hand doesn't compell you to use unnecessary configuration files.
### Streamlined Syntax
Python's syntax, albeit simple, relies heavily on indentation and is inconsistent (see the great schism of python 2.7). People who learn python often have lot's of trouble acclimating to other programming languages because of python's unique yet niche syntax. FastCode is not only a good first step for beginners, programmers expeiened in other languages can easily read and understand it. 
### Read-Execute-Loop
Since FastCode is based off of the read-execute-print-loop model, one can dynamically execute/debug, unlike C++, C, or Java. 
### Interopability and Portability
Since FastCode is written in C++, it can be easily be used in your C/C++ project and it can easily be ported to any machine, so long as you have the correct toolchain. 

# Some Examples
FizzBuzz
```
proc range(n) {
  range = array(n)
  while n-- {
    range[n] = n
  }
  return range
}

rem fizzbuzz from 0 to 99
for i in range(100) {
  print(i)
  if i % 5 == 0 {
    print("fizz")
  }
  if i % 3 == 0 {
    print("buzz")
  }
}
```
Fibonacci(Recursive Brute Force)
```
proc fibonacci(n) {
  if n == 0 or n == 1 {
    return n
  }
  return fibonacci(n - 1) + fibonacci(n - 2)
}
print(fibonacci(15)) //prints 15th number in a fibonacci sequence.
```
Factorial(Recursive Brute Force)
```
proc fact(n) {
  if n == 0 {
    return 1
  }
  return n * fact(n - 1)
}
print(fact(10)) rem Prints 10!
```
