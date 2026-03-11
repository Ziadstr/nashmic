# Arrays

> **Status:** Working. See `examples/arrays.nsh`.

## `saff<T>` - The Array Type

`saff` (صف) means "row" or "line" in Arabic — like a row of students standing in line. `saff<T>` is a dynamic, heap-allocated array of elements of type `T`. It grows automatically when you push elements.

```
khalli a3dad: saff<adad64> = [10, 20, 30, 40, 50]
```

## Array Literals

Create arrays with square bracket syntax:

```
khalli numbers: saff<adad64> = [10, 20, 30, 40, 50]
khalli grades: saff<fasle64> = [95.5, 87.3, 92.1]
khalli empty: saff<adad64> = []
```

## Indexing

Access elements by index with `arr[i]`:

```
khalli a3dad: saff<adad64> = [10, 20, 30, 40, 50]
itba3("a3dad[0] = %lld\n", a3dad[0])
itba3("a3dad[2] = %lld\n", a3dad[2])
```

## Iteration with `lakol`

Iterate over array elements with `lakol x fi arr`:

```
khalli a3dad: saff<adad64> = [10, 20, 30, 40, 50]
lakol n fi a3dad {
    itba3("%lld ", n)
}
itba3("\n")
```

This works exactly like range iteration but over array elements instead of a numeric range.

## `.zeed()` - Push Element

`zeed` (زيد) means "add more." It appends an element to the end of the array:

```
khalli a3dad: saff<adad64> = [10, 20, 30]
a3dad.zeed(40)
a3dad.zeed(50)
// a3dad is now [10, 20, 30, 40, 50]
```

## `.toul()` - Array Length

`toul` (طول) means "length." It returns the number of elements in the array:

```
khalli a3dad: saff<adad64> = [10, 20, 30, 40, 50]
itba3("toul: %lld\n", a3dad.toul())
// Output: toul: 5
```

## Empty Arrays

You can create an empty array and populate it with `.zeed()`:

```
khalli farigh: saff<adad64> = []
farigh.zeed(1)
farigh.zeed(2)
farigh.zeed(3)
// farigh is now [1, 2, 3]
```

## Arrays as Function Parameters

Pass arrays to functions:

```
dalle print_array(nums: saff<adad64>) {
    itba3("[ ")
    lakol n fi nums {
        itba3("%lld ", n)
    }
    itba3("]\n")
}

yalla() {
    khalli a3dad: saff<adad64> = [10, 20, 30]
    print_array(a3dad)
}
```

## Under the Hood

In the generated C, `saff<adad64>` becomes a struct with a pointer, length, and capacity:

```c
struct nsh_array_int64_t {
    int64_t *data;    // heap-allocated buffer
    int64_t len;      // current element count
    int64_t cap;      // allocated capacity
};
```

Array literals allocate on the heap with `malloc`. `.zeed()` uses `nsh_array_push()` from the runtime, which doubles capacity when full (amortized O(1) push). Iteration uses a simple index loop over `.data[]`.

Arrays are currently passed by value (the struct is copied, but the data pointer is shared). This means modifying an array inside a function affects the caller's array — similar to Go slices.

## Full Working Example

From `examples/arrays.nsh`:

```
dalle print_array(nums: saff<adad64>) {
    itba3("[ ")
    lakol n fi nums {
        itba3("%lld ", n)
    }
    itba3("]\n")
}

yalla() {
    // Array literal
    khalli a3dad: saff<adad64> = [10, 20, 30, 40, 50]

    // Length
    itba3("toul: %lld\n", a3dad.toul())

    // Index access
    itba3("a3dad[0] = %lld\n", a3dad[0])
    itba3("a3dad[2] = %lld\n", a3dad[2])

    // Iteration with lakol
    itba3("kol al-a3dad: ")
    print_array(a3dad)

    // Push element
    a3dad.zeed(60)
    a3dad.zeed(70)
    itba3("ba3d al-ziyade: ")
    print_array(a3dad)
    itba3("toul jadeed: %lld\n", a3dad.toul())

    // Float array
    khalli drajat: saff<fasle64> = [95.5, 87.3, 92.1]
    itba3("\ndrajat: [ ")
    lakol d fi drajat {
        itba3("%g ", d)
    }
    itba3("]\n")

    // Empty array with push
    khalli farigh: saff<adad64> = []
    farigh.zeed(1)
    farigh.zeed(2)
    farigh.zeed(3)
    itba3("\nfarigh ba3d al-ziyade: ")
    print_array(farigh)
}
```
