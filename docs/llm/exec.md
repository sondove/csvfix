# csvfix exec

> execute external command

## Synopsis
```
csvfix exec [options] [file ...]
```

## Description
`exec` runs an external shell command once for each CSV input record, substituting field values into the command line via `%N` placeholders (where `%1` is the first field). By default, the command's standard output is parsed as CSV and the resulting column(s) are appended to the input row, which is then written out. With `-r`, the command's output replaces the input record entirely and is emitted as plain text. It is intended for problems that CSVfix's built-in commands cannot solve.

## Options
- `-c cmd` — command line to execute (required). Use `%1`, `%2`, ... to reference 1-based CSV input fields; out-of-range references expand to empty. Use `%%` for a literal `%` character. On UNIX, substituted field values are escaped against `\ ' "` so they survive the shell. The command is run once per input record.
- `-r` — replace mode: emit the command's raw output as plain text instead of parsing it as CSV and appending it to the input row (default: off; output is parsed as CSV and appended).
- `-ix ecode` — ignore exit codes from the command that are less than or equal to `ecode` (integer); larger exit codes are treated as errors (default: `0`).
- `-d` — debug mode: print the expanded command line for each record but do not execute it (default: off).

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token `#ALL,SKIP,PASS` means the full input/output and CSV group applies: output `-o`, separators `-sep`/`-rsep`/`-osep`, `-ibl` ignore blank lines, `-smq` smart-quote, `-sqf` quote-fields, `-hdr` header record, `-ifn` ignore field-name header, plus row filtering with `-skip`/`-pass`. Note: `-skip` rows are dropped entirely; for a `-pass` row the command is still executed but the original record is written out unchanged.

## Examples

### Append command output as a new field (default mode)
Input `names.csv`:
```
"Charles","Dickens","M"
"Jane","Austen","F"
```
Command (uppercase the vowels in a concatenation of fields 1 and 2 with `tr`):
```
csvfix exec -c 'echo %1 %2 | tr aeiou X' names.csv
```
Output:
```
"Charles","Dickens","M","ChXrlXs DXckXns"
"Jane","Austen","F","JXnX AXstXn"
```

### Replace mode (`-r`)
Same `names.csv`. The command's output replaces the record and is emitted as plain text:
```
csvfix exec -r -c 'echo %2-%1' names.csv
```
Output:
```
Dickens-Charles
Austen-Jane
```

### Debug mode (`-d`)
Print expanded command lines without running them:
```
csvfix exec -d -c 'echo %1 %2' names.csv
```
Output:
```
echo Charles Dickens
echo Jane Austen
```

## Notes
- The command is spawned once per input record, so this is slow for large inputs. On Windows the sibling `call` command (call a DLL function) is faster but Windows-only and experimental.
- Field references are 1-based (`%1` = first field). References beyond the row's field count expand to an empty string rather than erroring.
- Literal percent signs require `%%`.
- In default (CSV) mode, each line of the command's stdout is parsed as a CSV row and appended to the input row; if the command emits multiple lines, multiple output rows are produced for that input record.
- UNIX vs Windows differ in how substituted values are quoted: UNIX escapes `\ ' "` in field values; Windows substitutes the raw value.
- `-ix` controls error tolerance for the command's exit status; by default any non-zero exit (`> 0`) is an error.
- The command line is passed to the system shell, so shell features such as pipes (`|`) and redirection work as shown in the examples.

## See also
- [eval](eval.md)
- [printf](printf.md)
- [echo](echo.md)
