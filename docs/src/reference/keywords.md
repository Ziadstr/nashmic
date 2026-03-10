# Keyword Reference

Every NashmiC keyword is a word you'd actually say in Amman. Franco-Arab only -- no Arabic script keywords. The `3` represents ain (ع), `2` represents hamza (ء).

## Control Flow

| Keyword | Pronunciation | Meaning | Equivalent |
|---------|--------------|---------|------------|
| `iza` | ee-za | "if" | `if` |
| `wala` | wa-la | "or else" | `else` |
| `wala_iza` | wa-la ee-za | "or if" | `else if` |
| `tool_ma` | tool ma | "as long as" | `while` |
| `lakol` | la-kol | "for each" | `for` |
| `fi` | fee | "in" / "there is" | `in` / `Some` |
| `liff` | liff | "go around" / "spin" | `loop` |
| `khalas` | kha-las | "enough!" | `break` |
| `kammel` | kam-mel | "keep going" | `continue` |
| `rajje3` | raj-je3 | "bring back" | `return` |
| `hasab` | ha-sab | "depending on" | `match` / `switch` |
| `hale` | ha-le | "case" / "situation" | match arm |
| `3adi` | aa-di | "whatever" / "normal" | `default` / `_` |

## Declarations

| Keyword | Pronunciation | Meaning | Equivalent |
|---------|--------------|---------|------------|
| `khalli` | khal-li | "let there be" | `let` |
| `thabet` | tha-bet | "fixed" / "firm" | `const` |
| `dalle` | dal-le | "a function" | `fn` / `func` |
| `haikal` | hai-kal | "structure" / "skeleton" | `struct` |
| `ta3dad` | ta-adad | "enumeration" | `enum` |
| `tabbe2` | tab-be2 | "apply" / "implement" | `impl` |

## Types

| Keyword | Pronunciation | Meaning | Equivalent |
|---------|--------------|---------|------------|
| `adad` | a-dad | "number" | `int32_t` |
| `adad64` | a-dad 64 | "number (64-bit)" | `int64_t` |
| `fasle` | fas-le | "decimal" | `float` |
| `fasle64` | fas-le 64 | "decimal (64-bit)" | `double` |
| `mante2` | man-te2 | "logical" | `bool` |
| `harf` | harf | "letter" | `char` |
| `nass` | nass | "text" | `string` / `const char*` |
| `fadi` | fa-di | "empty" | `void` |
| `natije` | na-tee-je | "result" / "outcome" | `Result<T>` |
| `yimkin` | yim-kin | "maybe" / "possibly" | `Optional<T>` |

## Values

| Keyword | Pronunciation | Meaning | Equivalent |
|---------|--------------|---------|------------|
| `ah` | ah | "yeah" | `true` |
| `la` | la | "no" | `false` |
| `tamam` | ta-mam | "all good" / "perfect" | `Ok` |
| `ghalat` | gha-lat | "wrong" / "mistake" | `Err` |
| `fi` | fee | "there is" | `Some` |
| `mafi` | ma-fi | "there isn't" | `None` |

## Error Handling

| Keyword | Pronunciation | Meaning | Equivalent |
|---------|--------------|---------|------------|
| `wala?` | wa-la? | "or?" (unwrap or propagate) | `?` operator |
| `ba3dain` | ba-a-dain | "later" | `defer` |
| `faz3a` | faz-a | "panic" / "alarm" | `panic!` |

## Entry Point

| Keyword | Pronunciation | Meaning | Equivalent |
|---------|--------------|---------|------------|
| `yalla` | yal-la | "let's go!" | `main` |

## Cultural Notes

- **`khalas`** is THE Jordanian stop word. Parents say it, teachers say it, everyone says it. When someone says khalas, you stop what you're doing. Perfect for `break`.

- **`yalla`** is universally recognized across the Arab world. It means "come on" / "let's go" -- the perfect way to start a program.

- **`ba3dain`** is the Jordanian art of procrastination. "I'll do it ba3dain" is a national motto. As a language feature, it schedules work for later -- but unlike real life, `ba3dain` in NashmiC actually keeps its promise.

- **`3adi`** is the most Jordanian response possible. "How was the exam?" "3adi." "How's work?" "3adi." It means "normal" / "whatever" / "nothing special" -- the perfect default case.

- **`tamam`/`ghalat`** -- Jordanians evaluate everything this way. "Tamam?" "Tamam." Or "ghalat, fix it."

- **`fi`/`mafi`** -- The universal Jordanian existence check. "Fi chai?" (Is there tea?) "Mafi." (There isn't.) Now it checks for the existence of values instead of beverages.
