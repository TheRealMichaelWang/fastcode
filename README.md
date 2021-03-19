# Fastcode
A perfect blend of C, Java, and Python tailored for those who desire a simple yet powerful programming language. FastCode is a procedural/structural, dynamically typed, and iterpreted programming language that doesn't excessivley consume memory or CPU time. It's perfectly suited for being used as an extension language as well as a general purpose language. 

Click [here](https://github.com/TheRealMichaelWang/fastcode/raw/main/fastcode.exe) to download.

# Rationale
FastCode grabs the best features of Python, Java, and C and leaves the worst out.

# examples
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
