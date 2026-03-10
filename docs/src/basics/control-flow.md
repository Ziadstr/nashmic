# Control Flow

## Conditionals: `iza` / `wala_iza` / `wala`

`iza` means "if" in Jordanian dialect. `wala` means "or else." `wala_iza` means "or if."

```
iza x > 0 {
    itba3("positive\n")
} wala_iza x == 0 {
    itba3("zero\n")
} wala {
    itba3("negative\n")
}
```

Conditions don't need parentheses. The body must always be wrapped in braces `{}`.

### Comparison operators

```
x == y    // equal
x != y    // not equal
x > y     // greater than
x < y     // less than
x >= y    // greater or equal
x <= y    // less or equal
```

### Logical operators

```
x && y    // logical AND
x || y    // logical OR
!x        // logical NOT
```

## While Loops: `tool_ma`

`tool_ma` means "as long as" -- the loop runs as long as the condition holds.

```
khalli count: adad64 = 0
tool_ma count < 10 {
    itba3("{count}\n")
    count = count + 1
}
```

## For-Each with Ranges: `lakol..fi`

`lakol` means "for each" and `fi` means "in." Together: "for each x in range."

```
lakol i fi 0..10 {
    itba3("{i}\n")
}
```

The range `0..10` iterates from 0 up to (but not including) 10. The loop variable `i` is automatically declared as `adad64`.

Ranges can start at any value:

```
lakol j fi 5..15 {
    itba3("{j}\n")
}
```

## Infinite Loops: `liff`

`liff` means "go around" or "spin" -- it's an infinite loop.

```
liff {
    itba3("forever...\n")
}
```

Use `khalas` to break out.

## Break: `khalas`

`khalas` means "enough!" -- it's THE Jordanian stop word. When someone says "khalas," you stop. The loop obeys the same rule.

```
khalli i: adad64 = 0
liff {
    iza i >= 10 {
        khalas
    }
    itba3("{i}\n")
    i = i + 1
}
```

## Continue: `kammel`

`kammel` means "keep going" -- skip the rest of this iteration and move to the next one.

```
lakol i fi 0..20 {
    iza i % 2 == 0 {
        kammel
    }
    itba3("{i} is odd\n")
}
```

## Return: `rajje3`

`rajje3` means "bring back" -- it returns a value from a function.

```
dalle abs(x: adad64) -> adad64 {
    iza x < 0 {
        rajje3 -x
    }
    rajje3 x
}
```

You can use `rajje3` without a value in void functions to exit early:

```
dalle maybe_print(x: adad64) {
    iza x < 0 {
        rajje3
    }
    itba3("{x}\n")
}
```

## Combining Control Flow

All control flow constructs can be nested freely:

```
dalle fizzbuzz(n: adad64) {
    lakol i fi 1..n {
        iza i % 15 == 0 {
            itba3("FizzBuzz\n")
        } wala_iza i % 3 == 0 {
            itba3("Fizz\n")
        } wala_iza i % 5 == 0 {
            itba3("Buzz\n")
        } wala {
            itba3("{i}\n")
        }
    }
}
```
