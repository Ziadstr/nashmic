# Functions

## Declaring Functions with `dalle`

`dalle` (دالّة) means "a function" in Arabic. It declares a function.

```
dalle greet(name: nass) {
    itba3("marhaba ya {name}!\n")
}
```

### With a return type

Use `->` to specify what the function returns:

```
dalle add(a: adad64, b: adad64) -> adad64 {
    rajje3 a + b
}
```

`rajje3` (رجّع) means "bring back." It's the return statement.

### Without a return type

Functions that don't return a value omit the `->` annotation:

```
dalle say_hello() {
    itba3("hello!\n")
}
```

## The `yalla()` Entry Point

Every NashmiC program needs a `yalla()` function. It's the entry point, the first thing that runs. `yalla` (يلّا) means "let's go!" - probably the most universally recognized Jordanian/Arabic expression.

```
yalla() {
    itba3("the program starts here\n")
}
```

`yalla()` takes no parameters and has no return type. It's always written exactly as `yalla()`.

## Parameters

Parameters use `name: type` syntax, separated by commas:

```
dalle describe(name: nass, age: adad64, height: fasle64) {
    itba3("{name} is {age} years old\n")
}
```

Parameters are passed by value. Struct parameters receive a copy:

```
dalle print_point(p: Point) {
    itba3("({p.x}, {p.y})\n")
}
```

## Recursion

Recursive functions work as expected:

```
dalle fibonacci(n: adad64) -> adad64 {
    iza n <= 1 {
        rajje3 n
    }
    rajje3 fibonacci(n - 1) + fibonacci(n - 2)
}

yalla() {
    lakol i fi 0..15 {
        itba3("fib({i}) = {fibonacci(i)}\n")
    }
}
```

## Calling Functions

Call a function by name with parentheses:

```
dalle double(x: adad64) -> adad64 {
    rajje3 x * 2
}

yalla() {
    khalli result: adad64 = double(21)
    itba3("result: {result}\n")
}
```

Functions must be declared before they are called. Forward declarations are not supported yet.

## Functions as First-Class Values

Functions are not first-class values in NashmiC yet. You cannot pass functions as arguments or store them in variables. This may change in a future version.
