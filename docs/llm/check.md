# csvfix check

> check CSV record format conforms to CSV standard

## Synopsis
```
csvfix check [options] [file ...]
```

## Description
The `check` command verifies that its inputs conform to the CSV standard specified by [IETF RFC 4180](http://www.ietf.org/rfc/rfc4180.txt). It reads each input file (or standard input if no file is given) character-by-character and parses it as CSV. For any record that violates the standard it prints a diagnostic message (file name, line number, and the offending text) to standard output. It does not transform or emit the input data — it only reports problems and sets an exit code. The process exit code is `0` if all inputs validated and `1` if any input failed.

## Options
- `-nl` — allow newlines embedded inside quoted fields, as permitted by RFC 4180. Off by default, because most CSV files contain no embedded newlines and disabling this improves diagnostic quality (an unterminated quote is otherwise reported as an embedded-newline error). Takes no argument.
- `-q` — run quietly: produce no output, just return the exit code (`0` = valid, `1` = failure). On the first failing input the command returns `1` immediately. Takes no argument.
- `-s sep` — use `sep` as the single-character field separator instead of `,`. Using this means you are no longer testing against RFC 4180. `sep` must be a single punctuation character and may not be a space, `"` or `'`; an invalid value is rejected with an "Invalid separator" error.
- `-v` — verbose: print `<filename> - OK` for each input that passes validation. Takes no argument.

## Common options
This command reads its input directly (character-by-character) rather than through the normal CSV-parsing pipeline, and its help text declares no standard generic-option group. The usual CSV/output convention flags (`-o`, `-sep`/`-osep`, `-ibl`, `-smq`, `-ifn`, `-skip`/`-pass`, etc.) do not apply — use the command-specific `-s` to change the separator. See [conventions](conventions.md) for the project-wide flag reference.

## Examples

### Valid file
Input `good.csv`:
```
name,age
"Alice",30
"Bob",25
```
Command (verbose):
```
csvfix check -v good.csv
```
Output (exit code 0):
```
good.csv - OK
```

### File with an unterminated quote
Input `bad.csv`:
```
name,age
"Alice,30
Bob,25
```
Command:
```
csvfix check bad.csv
```
Output (exit code 1):
```
Embedded newline in bad.csv at line 3
Bob,25
```
With `-q` the same input produces no output and exits with code 1. (The diagnostic mentions "embedded newline" because, with `-nl` off, the open quote on line 2 is read until end of input.)

### Embedded newline: default vs `-nl`
Input `nl.csv` (the value of the `note` field spans two physical lines):
```
note
"line one
line two"
```
By default this is rejected:
```
csvfix check nl.csv
```
```
Embedded newline in nl.csv at line 3
line two"
```
Allowing embedded newlines makes it pass:
```
csvfix check -nl -v nl.csv
```
```
nl.csv - OK
```

### Non-comma separator
Input `semi.csv`:
```
a;b
"x";"y"
```
Command:
```
csvfix check -s ';' -v semi.csv
```
Output (exit code 0):
```
semi.csv - OK
```

## Notes
- Exit code is the primary signal: `0` = all inputs valid, `1` = at least one input failed. Combine with `-q` for use in scripts.
- Without `-q`, all failing inputs are reported and counted; the command still exits `1` if any failed. With `-q`, it short-circuits and returns `1` on the first failure.
- Multiple files may be given; each is checked independently and (with `-v`) gets its own `- OK` line.
- `-s` accepts only a single punctuation character; space, `"` and `'` are rejected. Changing the separator means you are validating against a CSV dialect rather than strict RFC 4180.
- `check` performs only syntactic CSV validation. To validate field contents against rules (required fields, allowed values, dates, lengths, etc.) use the sibling `validate` command, which uses a rules file.

## See also
- [validate](validate.md)
- [echo](echo.md)
- [order](order.md)
