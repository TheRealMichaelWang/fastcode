# fastcode
A perfect blend of C, Java, and Python tailored for those who desire a simple yet powerful programming language.

Click [here](https://github.com/TheRealMichaelWang/fastcode/raw/main/fastcode.exe) to download.

Here's an example (fizzbuzz).
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
