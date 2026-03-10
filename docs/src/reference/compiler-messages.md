# Compiler Messages

NashmiC's compiler has personality. Errors come with Jordanian proverbs. Build successes get celebrated. It's not a cold tool -- it's your teammate.

## Error Format

Errors follow this format:

```
خطأ[E0001]: expected expression — شو هاد الحكي؟
  --> src/main.nsh:12:18
     |
  12 |     khalli x = ???
     |                ^^^
     |
      = اصبر على الحصرم تاكله عنب — patience turns sour to sweet
```

Each error includes:
1. **Error code** in Arabic (`خطأ` = error)
2. **Error description** in English with a Jordanian exclamation
3. **Source location** (file, line, column)
4. **Source context** with the offending code highlighted
5. **A Jordanian proverb** with English translation for wisdom (and levity)

## Proverb Categories

The compiler picks proverbs based on the error type:

| Error Type | Proverb (Arabic) | Translation | Meaning |
|-----------|------------------|-------------|---------|
| Type mismatch | حبل الكذب قصير | "The rope of lies is short" | You can't fake types |
| Invalid argument | عذر الراقصة إنو الأرض مايلة | "The dancer blames the floor" | Don't blame the language for bad arguments |
| Unreachable code | اللي ما بوصل للعنب بقول عنه حامض | "Can't reach the grapes? Call them sour" | That code will never run |
| Stack overflow | اللي بحفر حفرة لأخوه بقع فيها | "Dig a pit for your brother, fall in it" | Your recursion caught up to you |
| Unused variable | بلجيكي | "Belgian" (unnecessarily complicated) | Why declare something you don't use? |
| Unhandled error | شايف حالك | "You think you're safe?" | Handle your errors |

## Build Success Messages

When a build succeeds, the compiler celebrates with a random quip:

```
✓ تم! زي الفل         ("Done! Like jasmine" — everything's perfect)
✓ تم! نشمي والله        ("Done! Nashmi, wallah" — noble work)
✓ تم! والله ما قصرت    ("Done! You didn't fall short")
✓ تم! اسرع من طلقة     ("Done! Faster than a bullet")
```

"تم" (tam) means "done" -- the one-word reply Jordanians send when a task is complete.

## First Build Banner

The very first time you build a NashmiC project, the compiler shows an ASCII art banner. This only happens once -- consider it the compiler's way of saying "welcome to the family."

## The `--tarab` Flag

Build with `--tarab` for a celebratory audio on success:

```bash
mansaf build --tarab program.nsh
```

"Tarab" (طرب) means musical ecstasy -- that feeling when an Arabic song hits just right. The flag attempts to play a short audio clip when your build succeeds. Because shipping code should feel like a celebration.

## Error Philosophy

NashmiC's error messages are designed to be:

1. **Precise** -- exact file, line, and column with source context
2. **Helpful** -- clear description of what went wrong
3. **Human** -- proverbs add wisdom and prevent compiler-induced rage
4. **Cultural** -- the errors speak Jordanian, just like the code

The proverbs aren't random decoration. Each one is matched to the error category and carries genuine wisdom about the programming mistake you just made.
