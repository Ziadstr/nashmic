# NashmiC — Implementation Plan

> **نشمي** (Nashmi) = brave, noble, valiant — the ultimate Jordanian word.
>
> A C-level systems programming language with **Jordanian dialect** keywords,
> dual-mode syntax (Arabic script + Franco-Arab Latin), and 🇯🇴 identity in every keyword.
>
> Not a toy. A real, Linux-native, properly specified language.

---

## Table of Contents

1. [Project Identity](#1-project-identity)
2. [Language Design](#2-language-design)
3. [Complete Keyword Table](#3-complete-keyword-table)
4. [Compiler Architecture](#4-compiler-architecture)
5. [Runtime System](#5-runtime-system)
6. [Garbage Collector](#6-garbage-collector)
7. [Standard Library (maktabe)](#7-standard-library-maktabe)
8. [Toolchain](#8-toolchain)
9. [Example Programs](#9-example-programs)
10. [Repository Structure](#10-repository-structure)
11. [Phased Roadmap](#11-phased-roadmap)
12. [Compiler Output Style](#12-compiler-output-style)

---

## 1. Project Identity

### Name: NashmiC

- **نشمي** = Jordanian for "brave, noble, valiant"
- The word Jordanians use to describe someone truly respectable
- Carries national identity — this is Jordan's language

### Philosophy

- **Jordanian dialect**, not MSA textbook Arabic
- Sounds like a Jordanian dev talking to another dev
- Every keyword has cultural weight and daily-life usage
- **Dual-mode**: Arabic script for cultural statement, Franco for practical typing

### File Extension

- `.nsh` — NashmiC source
- `.nshh` — NashmiC header/declarations

### Key Design Decision: Dual-Mode Keywords

Every keyword works in both **Arabic script** and **Franco-Arab Latin** transliteration:
- `اذا` and `iza` are identical tokens
- Arabic script = cultural statement, visual identity
- Franco = practical typing, LTR, standard editors

---

## 2. Language Design

### Design Principles

1. **C-level power** — manual memory possible, no hidden costs
2. **Safe by default** — GC manages memory unless you opt out
3. **Jordanian first** — keywords are Jordanian dialect, not formal Arabic
4. **Dual-mode** — every keyword works in Arabic script AND Franco transliteration
5. **Linux-native** — ELF binaries, System V ABI, DWARF debug info
6. **Tooling-ready** — formatter, test runner, package manager from day one

### Memory Model

- **Default**: Garbage collected (Boehm GC → custom mark-sweep → generational)
- **Opt-in manual**: `3al_3amyani` (unsafe) blocks for manual memory management
- **Guard blocks**: `hares` for pointer safety checks

### Concurrency Model

- Phase 1: pthreads
- Phase 3: cooperative green threads
- Channels (`qanah`) for communication between threads

### Error Handling

- `natije<T>` (Result type) for recoverable errors: `tamam(v)` / `ghalat(e)`
- `jarreb`/`imsek`/`irmi` (try/catch/throw) for exceptional errors
- `faz3a` (panic) for unrecoverable errors — Bedouin emergency rally cry
- `ba3dain` (defer) — procrastination as a feature

---

## 3. Complete Keyword Table — Pure Jordanian Dialect

### Control Flow

| Concept | Arabic | Franco | Meaning | Why It's Jordanian |
|---------|--------|--------|---------|-------------------|
| `if` | `اذا` | `iza` | "if" | "اذا بدك" — every Jordanian's "if" |
| `else` | `والا` | `wala` | "or else" | "والا شو بدك؟" |
| `else if` | `والا_اذا` | `wala_iza` | "or if" | Natural compound |
| `while` | `طول_ما` | `tool_ma` | "as long as" | "طول ما انت هون" |
| `for each` | `لكل` | `lakol` | "for each" | "لكل واحد فيهم" |
| `loop` | `لف` | `liff` | "go around" | "لف لف" = going in circles |
| `break` | `خلص` | `khalas` | "enough / done" | THE Jordanian "stop" |
| `continue` | `كمل` | `kammel` | "keep going" | "كمّل شغلك" |
| `return` | `رجع` | `rajje3` | "bring back" | "رجّعلي الباقي" |
| `switch` | `حسب` | `hasab` | "depending on" | "حسب الحالة" |
| `case` | `حالة` | `hale` | "case/situation" | "شو الحالة؟" |
| `default` | `عادي` | `3adi` | "normal/whatever" | THE Jordanian "default" |

### Types

| Concept | Arabic | Franco | Size | Why |
|---------|--------|--------|------|-----|
| `int32` | `عدد` | `adad` | 32-bit | "number" |
| `int64` | `عدد٦٤` | `adad64` | 64-bit | number + size |
| `float` | `فاصلة` | `fasle` | 32-bit | "decimal point" |
| `double` | `فاصلة٦٤` | `fasle64` | 64-bit | double decimal |
| `bool` | `منطق` | `mante2` | 8-bit | "logic" — "هاد مش منطق!" |
| `char` | `حرف` | `harf` | 8-bit | "letter" |
| `byte` | `بايت` | `bait` | 8-bit | Borrowed English — Jordanians say "byte" |
| `string` | `نص` | `nass` | ptr+len | "text" |
| `void` | `فاضي` | `fadi` | 0 | "empty/free" — "انا فاضي" |
| `pointer` | `مؤشر` | `mu2shr` | 64-bit | "pointer/indicator" |
| `array` | `صف` | `saff` | fixed | "row/line" — things lined up |
| `struct` | `هيكل` | `haikal` | record | "skeleton/frame" |
| `enum` | `تعداد` | `ta3dad` | tagged union | "enumeration" |

### Values

| Concept | Arabic | Franco | Why |
|---------|--------|--------|-----|
| `true` | `اه` | `ah` | THE Jordanian "yes" — not formal نعم |
| `false` | `لا` | `la` | "no" — universal |
| `null` | `ولا_اشي` | `wala_ishi` | "nothing at all" — "شو عندك؟" "ولا اشي" |

### Declarations

| Concept | Arabic | Franco | Why |
|---------|--------|--------|-----|
| `let` / `var` | `خلي` | `khalli` | "خلي x يساوي 5" — THE Jordanian "let" |
| `const` | `ثابت` | `thabet` | "fixed / won't change on you" |
| `function` | `دالة` | `dalle` | "function" — learned in school |
| `public` | `عام` | `aam` | "public" |
| `private` | `خاص` | `khas` | "private — هاد اشي خاص" |
| `export` | `صدر` | `sadder` | "export" — trade term |
| `import` | `جيب` | `jeeb` | "bring!" — "جيبلي اياه" |
| `module` | `قسم` | `qesm` | "section/department" |
| `extern` | `خارجي` | `kharji` | "external/foreign" |
| `new` | `جديد` | `jdeed` | "new — اشي جديد" |
| `free` | `فك` | `fukk` | "release/untie" |

### Memory

| Concept | Arabic | Franco | Why |
|---------|--------|--------|-----|
| `alloc` | `احجز` | `ihjez` | "reserve a spot" — hotel booking = memory reservation |
| `free` | `فك` | `fukk` | "release/untie" |
| `guard` | `حارس` | `hares` | "guard" — pointer safety block |
| `unsafe` | `عالعمياني` | `3al_3amyani` | "going in blind" — doing something risky |

### Error Handling

| Concept | Arabic | Franco | Why |
|---------|--------|--------|-----|
| `try` | `جرب` | `jarreb` | "try this — جرّب هاد" |
| `catch` | `امسك` | `imsek` | "catch/grab — امسك الغلط" |
| `throw` | `ارمي` | `irmi` | "throw" |
| `panic` | `فزعة` | `faz3a` | Bedouin emergency tribal rally cry |
| `Result<T>` | `نتيجة` | `natije` | "result/outcome" |
| `Ok(v)` | `تمام` | `tamam` | "good/OK — تمام التمام" |
| `Err(e)` | `غلط` | `ghalat` | "error/mistake" |
| `defer` | `بعدين` | `ba3dain` | "later" — procrastination as a feature |

### Logical Operators

| Concept | Arabic | Franco |
|---------|--------|--------|
| `and` | `و` | `w` |
| `or` | `او` | `aw` |
| `not` | `مش` | `mish` |
| `sizeof` | `حجم` | `hajm` |
| `typeof` | `نوع` | `no3` |

### Concurrency

| Concept | Arabic | Franco | Why |
|---------|--------|--------|-----|
| `spawn` | `شغل` | `shaghghel` | "start the engine — شغّل المكينة" |
| `wait/join` | `استنى` | `istanna` | "wait — استنّى شوي" said 500x/day |
| `channel` | `قناة` | `qanah` | "channel" |
| `send` | `ابعث` | `ib3ath` | "send — ابعثلي رسالة" |
| `receive` | `استلم` | `istalam` | "receive — استلمت الطرد" |
| `lock` | `اقفل` | `i2fel` | "lock the door" |
| `unlock` | `افتح` | `iftah` | "open the door" |
| `async` | `عالطاير` | `3al_tayer` | "on the fly" |
| `thread` | `خيط` | `khait` | "thread" (same metaphor as English) |

### I/O & Common Ops

| Concept | Arabic | Franco | Why |
|---------|--------|--------|-----|
| `print` | `اطبع` | `itba3` | "print" — imperative |
| `read/input` | `اقرأ` | `i2ra` | "read!" — first word of the Quran |
| `exit` | `اطلع` | `itla3` | "get out" — blunt, Jordanian |
| `assert` | `اكد` | `akked` | "confirm" |
| `test` | `فحص` | `fahas` | "examine" — checking health |

### Collections (stdlib)

| Concept | Arabic | Franco |
|---------|--------|--------|
| `List/Vec` | `قائمة` | `Qa2ime` |
| `Map/Dict` | `قاموس` | `Qamoos` |
| `Set` | `مجموعة` | `Majmoo3a` |

### Extra Jordanian Flavor Keywords

| Concept | Arabic | Franco | Why |
|---------|--------|--------|-----|
| `main()` | `يلا` | `yalla` | "LET'S GO!" — every program starts with يلا |
| `this/self` | `هاد` | `had` | "this thing" |
| `match` | `طابق` | `tabe2` | "match/correspond" |
| `type alias` | `سمي` | `sammi` | "name it" |
| `impl` | `طبق` | `tabbe2` | "implement/apply" |
| `trait` | `صفة` | `sife` | "trait/quality" |
| `set/assign` | `حط` | `7utt` | "put — حط الاشي هون" |
| `get/fetch` | `جيب` | `jeeb` | "bring — جيبلي اياه" |
| `remove` | `شيل` | `sheel` | "remove/take away" |
| `add/push` | `زيد` | `zeed` | "add more" (also Ziad's name lol) |
| `finally` | `بنهايته` | `binhayto` | "at the end of it" |
| deprecated | `زنخ` | `zanakh` | "stale/outdated" — smells bad |
| overengineered | `بلجيكي` | `baljiki` | "Belgian" = unnecessarily complex |

---

## 4. Compiler Architecture

### Overview

Hand-written recursive descent parser + Pratt precedence climbing, written in C.

### Pipeline

```
.nsh source → Lexer → Tokens → Parser → AST → Sema → Typed AST → Codegen → Output
```

### Backends (Phased)

| Phase | Backend | Output | When |
|-------|---------|--------|------|
| 1 | C transpiler | `.nsh` → `.c` → binary via host `cc` | Now (Phase 0) |
| 2 | QBE | `.nsh` → QBE IR → native binary | Month 9-11 |
| 3 | LLVM | `.nsh` → LLVM IR → optimized native binary | Month 14-18 |

### Lexer

- UTF-8 aware tokenizer
- Dual-mode keyword recognition via FNV-1a hash table
- Arabic Unicode ranges: 0x0600-0x06FF, 0xFE70-0xFEFF
- Arabic-Indic digit support (٠-٩)
- Single-line (`//`) and nested multi-line (`/* */`) comments

### Parser

- Recursive descent for statements and declarations
- Pratt precedence climbing for expressions
- Operator precedence: assignment < or < and < equality < comparison < addition < multiplication < unary < call

### Semantic Analysis (Phase 1)

- Type checking and inference
- Symbol table with lexical scoping
- Function signature validation
- Type coercion rules

---

## 5. Runtime System

### Components

| File | Purpose |
|------|---------|
| `nsh_runtime.c/h` | Init/shutdown, itba3 (print), i2ra (read), itla3 (exit) |
| `nsh_gc.c/h` | Garbage collector (Boehm → custom mark-sweep) |
| `nsh_string.c/h` | UTF-8 string type (length-prefixed) |
| `nsh_io.c/h` | File I/O, Reader/Writer |
| `nsh_error.c/h` | Errors, panic, stack traces |
| `nsh_channel.c/h` | Channel implementation |
| `nsh_thread.c/h` | Thread spawning |

### Strings

- Length-prefixed UTF-8
- Rune iteration for Arabic text
- Immutable by default, mutable builder for construction

### Linux Integration

- ELF binaries, System V AMD64 ABI
- DWARF debug info (Phase 2+)
- C FFI compatible
- GDB/perf/valgrind/strace compatible

---

## 6. Garbage Collector

### Phased Implementation

| Phase | Strategy | Details |
|-------|----------|---------|
| 1 | Boehm GC | Conservative, drop-in, works immediately |
| 2 | Custom mark-sweep | Precise, type-aware, better performance |
| 3 | Generational | Young/old generations, concurrent marking |

### Manual Memory

- `ihjez` (alloc) + `fukk` (free) for manual allocation
- `3al_3amyani` blocks: YOU manage memory, compiler warns if you forget `fukk()`
- `hares` (guard) blocks: pointer safety checks

---

## 7. Standard Library — `مكتبة` / `maktabe`

| Module | Franco | What |
|--------|--------|------|
| `اساس` | `asas` | Foundation — builtins, type info |
| `ذاكرة` | `dhakire` | Memory — allocators, GC hooks |
| `مدخلات` | `madkhalat` | I/O — stdin/stdout, Reader/Writer |
| `تنسيق` | `tansee2` | Formatting — string format |
| `نصوص` | `nusoos` | Text — string ops, UTF-8 |
| `نظام` | `nizam` | System — exit, args, env |
| `اخطاء` | `akhta2` | Errors — error types, result |
| `مجموعات` | `majmoo3at` | Collections — list, map, set |
| `رياضيات` | `riyadiyat` | Math |
| `ملفات` | `malafat` | File I/O |
| `ترتيب` | `tarteeb` | Sorting |
| `وقت` | `wa2t` | Time |
| `شبكة` | `shabake` | Networking — TCP, UDP |
| `خيوط` | `khyoot` | Threading |
| `تزامن` | `tazamun` | Sync — channels, atomics, mutex |
| `تشفير` | `tashfeer` | Crypto — SHA-256, hashing |
| `ترميز` | `tarmeez` | Encoding — JSON, base64 |

---

## 8. Toolchain

### mansaf CLI (المنسف — the compiler)

```
mansaf build main.nsh          # Cook code into binary
mansaf run main.nsh            # Cook + eat (build + run)
mansaf fahas                   # Run tests (فحص)
mansaf fmt                     # Format code
mansaf doc                     # Generate docs
mansaf diwan                   # Open REPL (ديوان)
mansaf init my-project         # Scaffold new project
mansaf clean                   # Remove build artifacts
mansaf lex main.nsh            # Debug: dump tokens
```

### souq CLI (السوق — package manager)

```
souq install github.com/nashmic/json@v1.0
souq search http
souq update
souq publish
souq list
```

### Package Manifest — `mansaf.toml`

```toml
[mashroo3]
ism = "my-app"
nuskhe = "1.0.0"
saheb = "Ziad Al-Sharif"
tarkhees = "MIT"
wasf = "A web server in NashmiC"

[bina]
hadaf = "binary"
madkhal = "src/main.nsh"

[taba3iyat]
json = { git = "github.com/nashmic/json", tag = "v1.0.0" }
http = { git = "github.com/nashmic/http", tag = "v0.5.0" }

[taba3iyat_tatweer]
mock = { git = "github.com/nashmic/mock", tag = "v0.1.0" }
```

### Editor Support

| Tool | Name |
|------|------|
| Tree-sitter | `tree-sitter-nashmic` |
| VS Code | `vscode-nashmic` |
| Vim | `nashmic.vim` |
| LSP | `mansaf-lsp` |

---

## 9. Example Programs

### Hello World

```
// main.nsh — مرحبا يا عالم

yalla() {
    itba3("مرحبا يا عالم! 🇯🇴\n")
    itba3("Welcome to NashmiC — ya nashmi!\n")
}
```

### Fibonacci

```
dalle fibonacci(n: adad64) -> adad64 {
    iza n <= 1 { rajje3 n }
    rajje3 fibonacci(n - 1) + fibonacci(n - 2)
}

yalla() {
    lakol i fi 0..20 {
        itba3("fib({}) = {}\n", i, fibonacci(i))
    }
}
```

### Structs + Error Handling

```
haikal Talib {              // Student (طالب)
    ism: nass,              // name
    mu3addal: fasle64,      // GPA
    saneh: adad,            // year
}

dalle najah(t: Talib) -> natije<nass> {
    iza t.mu3addal < 2.0 {
        rajje3 ghalat("يا زلمة المعدل واطي — GPA too low")
    }
    rajje3 tamam("مبروك يا {} — Passed!".tansee2(t.ism))
}

yalla() {
    khalli ziad = Talib{
        ism: "Ziad",
        mu3addal: 3.2,
        saneh: 6,           // 6 سنين يا ريت تخلص
    }

    hasab najah(ziad) {
        tamam(msg) => itba3("{}\n", msg),
        ghalat(e)  => itba3("فشل: {}\n", e),
    }
}
```

### Concurrency — Producer/Consumer

```
jeeb maktabe.tazamun

yalla() {
    khalli ch: qanah<adad> = jdeed qanah(10)

    shaghghel {
        lakol i fi 0..10 {
            ib3ath(ch, i * i)
        }
        ch.sakker()
    }

    lakol val fi ch {
        itba3("استلمت: {}\n", val)
    }

    itba3("خلصنا — تمام التمام\n")
}
```

### Manual Memory (unsafe)

```
yalla() {
    // Default: GC manages everything
    khalli p = ihjez(Talib{ism: "Ahmad", mu3addal: 3.5, saneh: 4})

    // 3al_3amyani block: YOU manage memory
    3al_3amyani {
        khalli raw = ihjez(Talib{ism: "Laith", mu3addal: 2.8, saneh: 5})
        itba3("Name: {}\n", raw.ism)
        fukk(raw)
    }
}
```

---

## 10. Repository Structure

```
nashmic/
├── compiler/src/           # mansaf compiler (written in C)
│   ├── main.c              # CLI (mansaf build/run/fmt/fahas/etc.)
│   ├── lexer.c/h           # UTF-8 tokenizer, dual-mode keywords
│   ├── parser.c/h          # Recursive descent + Pratt parsing
│   ├── ast.c/h             # AST node types
│   ├── sema.c/h            # Semantic analysis, type checking
│   ├── codegen_c.c/h       # Phase 1: C transpiler
│   ├── codegen_qbe.c/h     # Phase 2: QBE backend
│   ├── codegen_llvm.c/h    # Phase 3: LLVM backend
│   ├── diagnostics.c/h     # Errors + proverbs + colors
│   ├── keywords.c/h        # Arabic + Franco keyword tables
│   ├── utf8.c/h            # UTF-8 handling
│   └── span.h              # Source locations
├── runtime/                # Runtime library (C)
│   ├── nsh_runtime.c/h     # Init/shutdown
│   ├── nsh_gc.c/h          # GC (Boehm → custom mark-sweep)
│   ├── nsh_string.c/h      # UTF-8 string type
│   ├── nsh_io.c/h          # itba3/i2ra
│   ├── nsh_error.c/h       # Errors, panic, stack traces
│   ├── nsh_channel.c/h     # Channel impl
│   └── nsh_thread.c/h      # Thread spawning
├── stdlib/                 # Standard library (.nsh files)
│   ├── asas/ dhakire/ madkhalat/ nusoos/
│   ├── majmoo3at/ riyadiyat/ malafat/
│   ├── shabake/ khyoot/ tazamun/
│   └── tashfeer/ tarmeez/
├── tools/
│   ├── souq/               # Package manager
│   └── playground/         # Online playground
├── editor/
│   ├── tree-sitter-nashmic/
│   ├── vscode-nashmic/
│   └── nashmic.vim
├── spec/
│   ├── grammar.ebnf
│   └── reference/          # Language ref (mdBook)
├── docs/
│   ├── tutorial/           # "Tour of NashmiC"
│   └── cookbook/
├── examples/
│   ├── marhaba.nsh         # Hello World
│   ├── fibonacci.nsh
│   ├── echo_server.nsh
│   └── mansaf_calculator.nsh
├── tests/
├── mansaf.toml
├── Makefile
├── LICENSE
└── README.md
```

---

## 11. Phased Roadmap

| Phase | Timeline | Goal | Status |
|-------|----------|------|--------|
| **0** | Week 1-2 | `mansaf run marhaba.nsh` prints "مرحبا يا عالم" | ✅ DONE |
| **1** | Week 3-6 | Types, functions, control flow. Fibonacci compiles. | ✅ DONE |
| **2** | Week 7-10 | Boehm GC, `ihjez`/`fukk`, `3al_3amyani` blocks, `hares` guards | |
| **3** | Week 11-14 | `haikal` (structs), `ta3dad` (enums), `jarreb`/`imsek`/`irmi` errors | |
| **4** | Month 4-5 | `mansaf fmt`, `mansaf fahas`, tree-sitter, VS Code extension | |
| **5** | Month 5-7 | `qesm` modules, stdlib Tier 0+1, `mansaf.toml`, `souq` | |
| **6** | Month 7-9 | Language reference, playground, basic LSP | |
| **7** | Month 9-11 | Concurrency: `shaghghel`/`istanna`, channels | |
| **8** | Month 11-13 | QBE backend — native compilation | |
| **9** | Month 14-18 | LLVM backend — optimization, DWARF, cross-compilation | |
| **10** | Year 2+ | Generics, self-hosting, custom GC | |

### Phase 0 Deliverables (COMPLETE)

- [x] UTF-8 lexer with dual-mode Arabic + Franco keywords
- [x] Recursive descent parser with Pratt precedence
- [x] C transpiler backend
- [x] Minimal runtime (init, print, read, exit)
- [x] `mansaf build/run/lex` CLI
- [x] Hello World example (Franco + Arabic modes)
- [x] Fibonacci example (recursive functions, while loops, typed params)
- [x] Makefile build system

---

## 12. Compiler Output Style

### Jordanian Proverbs as Compiler Messages

| Proverb | Arabic | When Used |
|---------|--------|-----------|
| حبل الكذب قصير | "The rope of lies is short" | Type mismatch |
| عذر الراقصة إنو الأرض مايلة | "The dancer's excuse is the ground is tilted" | Invalid argument |
| اللي ما بوصل للعنب بقول عنه حامض | "Can't reach the grapes, says they're sour" | Unreachable code |
| اصبر على الحصرم تأكله عنب | "Be patient with sour grapes → sweet grapes" | Compilation progress |
| قدر لاقى غطاه | "A pot found its lid" | Pattern match success |
| اللي بحفر حفرة لأخوه بقع فيها | "Dig a pit for your brother, fall in it" | Infinite recursion / stack overflow |
| بكرا بتشرق الشمس | "Tomorrow the sun will rise" | Retry / recovery suggestion |

### Success

```
$ mansaf build main.nsh
✓ تم! Build succeeded — زي الفل 🇯🇴
  Binary: ./main (23.4 KB)
  Time: 0.12s
```

### Error

```
خطأ[E1003]: أنواع مش متوافقة
  --> src/main.nsh:12:18
   |
12 |     khalli x: adad = "مرحبا"
   |              ────    ^^^^^^^ متوقع `adad`، لقيت `nass`
   |
   = حبل الكذب قصير — Type mismatch detected
   = مساعدة: جرب tahweel("مرحبا")
```

### Warning

```
تحذير[W0012]: بلجيكي — unnecessarily complex code
  --> src/utils.nsh:45:5
   |
45 |     iza x > 0 { iza x > 0 { iza x > 0 { rajje3 x } } }
   |     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ nested redundant checks
   |
   = عذر الراقصة إنو الأرض مايلة — Simplify this
```

### Panic

```
=== فزعة! (PANIC) === 🚨
وين عقلك — Null pointer dereference at 0x0

Stack trace:
  0: process_data() at src/main.nsh:42
  1: handle_request() at src/server.nsh:128
  2: yalla() at src/main.nsh:15

الله يهديك — Fix your pointers ya zalameh
```

### All Tests Pass

```
$ mansaf fahas
Running 24 tests...

  ✓ math: should add two numbers (0.1ms)
  ✓ math: should handle overflow (0.1ms)
  ✓ string: should split on delimiter (0.3ms)
  ✓ http: should parse URL (1.2ms)

Results: 24 passed, 0 failed — تمام التمام 💪
الله يعطيك العافية
```

---

## Technical Foundation

All compiler/runtime/OS architecture:
- **Compiler:** Hand-written recursive descent + Pratt parsing, written in C
- **Backends:** C transpiler (Phase 1) → QBE (Phase 2) → LLVM (Phase 3)
- **GC:** Boehm GC (Phase 1) → custom mark-and-sweep (Phase 2) → generational (Phase 3)
- **Runtime:** Signal handling, UTF-8 strings (length-prefixed), startup/shutdown sequence
- **Linux:** ELF binaries, System V AMD64 ABI, DWARF debug info, C FFI compatible
- **Tools:** GDB/perf/valgrind/strace compatible
- **Strings:** Length-prefixed UTF-8, rune iteration for Arabic text
- **Errors:** Span-tracked, colored, with source context and suggestions
- **Memory:** Default GC + opt-in manual (`3al_3amyani` blocks) + guard blocks (`hares`)
- **Concurrency:** pthreads (Phase 1) → cooperative green threads (Phase 3)
