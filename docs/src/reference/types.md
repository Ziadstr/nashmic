# Type Reference

## Primitive Types

| NashmiC | C Equivalent | Size | Description |
|---------|-------------|------|-------------|
| `adad` | `int32_t` | 4 bytes | 32-bit signed integer |
| `adad64` | `int64_t` | 8 bytes | 64-bit signed integer |
| `fasle` | `float` | 4 bytes | 32-bit IEEE 754 floating point |
| `fasle64` | `double` | 8 bytes | 64-bit IEEE 754 floating point |
| `mante2` | `bool` | 1 byte | Boolean (`ah` or `la`) |
| `harf` | `char` | 1 byte | Single ASCII character |
| `nass` | `const char*` | pointer | UTF-8 string |
| `fadi` | `void` | 0 | No value (return type only) |

## Composite Types

| NashmiC | Category | Description |
|---------|----------|-------------|
| `haikal` | Struct | Named product type with typed fields |
| `ta3dad` | Tagged Union | Sum type with variants that can carry data |

## Generic Types

| NashmiC | Equivalent | Description |
|---------|-----------|-------------|
| `natije<T>` | `Result<T, String>` | Result type -- `tamam(T)` or `ghalat(nass)` |
| `yimkin<T>` | `Option<T>` | Optional type -- `fi(T)` or `mafi` |

## Boolean Values

| NashmiC | Meaning | C Value |
|---------|---------|---------|
| `ah` | "yeah" / true | `1` |
| `la` | "no" / false | `0` |

## Result Values

| NashmiC | Meaning | When to use |
|---------|---------|-------------|
| `tamam(value)` | "all good" -- success | Function succeeded, wraps the return value |
| `ghalat(message)` | "wrong" -- error | Function failed, wraps an error message |

## Optional Values

| NashmiC | Meaning | When to use |
|---------|---------|-------------|
| `fi(value)` | "there is" -- present | Value exists, wraps it |
| `mafi` | "there isn't" -- absent | No value |

## Numeric Ranges

| Type | Min | Max |
|------|-----|-----|
| `adad` | -2,147,483,648 | 2,147,483,647 |
| `adad64` | -9,223,372,036,854,775,808 | 9,223,372,036,854,775,807 |
| `fasle` | ~1.2e-38 | ~3.4e+38 |
| `fasle64` | ~2.2e-308 | ~1.8e+308 |

## Type Naming Convention

NashmiC type names come from Arabic/Jordanian words:

- **`adad`** (عدد) = number
- **`fasle`** (فاصلة) = decimal point / comma
- **`mante2`** (منطقي) = logical
- **`harf`** (حرف) = letter / character
- **`nass`** (نص) = text
- **`fadi`** (فاضي) = empty / free
- **`natije`** (نتيجة) = result / outcome
- **`yimkin`** (يمكن) = maybe / possibly
