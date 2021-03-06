# fastcode
A perfect blend of C, Java, and Python tailored for those who desire a simple yet powerful programming language. It's also fast, versatile, and powerful - while it's syntax is simple and easy to understand.

Here's an example (fizzbuzz).
```
proc range(n) {
  range = array(n)
  i = 0
  while i < n {
    range[i] = i
    i++
  }
  return range
}

rem fizzbuzz from 0 to 99
for i in range(100) {
  if i % 15 == 0 {
    print("fizzbuzz")
  }
  elif i % 5 == 0 {
    print("fizz")
  }
  elif i % 3 == 0 {
    print("buzz")
  }
  printl(i)
}
```
