# csvfix read_dsv

> convert DSV (delimiter separated variables) data to CSV

## Synopsis
```
csvfix read_dsv [options] [file ...]
```

## Description
Reads delimiter-separated values (DSV) data and writes it as CSV. DSV data
consists of values separated by a single delimiter character (the pipe `|` by
default); it is common in the UN*X world (e.g. `/etc/passwd`). Input is read
line by line from the named files (or standard input if none are given); each
line is split into fields on the delimiter and written as one CSV row to
standard output. A delimiter that must appear inside a value is escaped with a
backslash (`\|`), and a literal backslash is written as `\\`.

## Options
- `-s sep` — single-character DSV separator used in the input (default: `|`, the pipe symbol). The only supported escaped sequence is `\t` for tab; a space (`-s ' '`) is allowed as an ordinary separator. The separator cannot be empty and must be a single character (other than `\t`).
- `-f fields` — comma-separated list of 1-based field indexes to convert/keep, in the given order (default: all fields). Indexes beyond the number of fields on a line produce empty output fields.
- `-csv` — treat each parsed field's contents as CSV, so wrapping double quotes are stripped and doubled quotes (`""`) are unescaped (default: off, contents taken literally).
- `-cm` — collapse multiple consecutive separators into a single separator, so empty fields between repeated delimiters are dropped (default: off).

## Common options
Also accepts the standard options — see [conventions](conventions.md).
From the source generic-flag token `#SMQ,IBL,OFL`, the applicable groups are:
output `-o file` (write to a file instead of standard output); `-ibl` (ignore
blank input lines); and smart-quoting `-smq` plus `-sqf fields` (force the named
fields to be quoted on output).

## Examples

### Basic conversion (default pipe separator)
Input `names.dsv`:
```
Charles|Dickens|M
Jane|Austen|F
Flann|O'Brien|M
```
Command:
```
csvfix read_dsv names.dsv
```
Output:
```
"Charles","Dickens","M"
"Jane","Austen","F"
"Flann","O'Brien","M"
```

### Selecting and reordering fields with -f
```
csvfix read_dsv -f 1,3 names.dsv
```
Output:
```
"Charles","M"
"Jane","M"
```
(With the input above this keeps fields 1 and 3 of each row; e.g. row 1 becomes `"Charles","M"`.)

### Escaped delimiters
Input `operators.dsv` (the `\|` is an escaped pipe, `\\` an escaped backslash):
```
asterisk|*|multiplication
equals|=|assignment
pipe|\||bitwise OR
backslash|\\|not a C++ operator
```
Command:
```
csvfix read_dsv operators.dsv
```
Output:
```
"asterisk","*","multiplication"
"equals","=","assignment"
"pipe","|","bitwise OR"
"backslash","\","not a C++ operator"
```

### Tab-separated input
Input `tabs.dat` (fields separated by a literal tab):
```
a,b	c,d
```
Command:
```
csvfix read_dsv -s '\t' tabs.dat
```
Output:
```
"a,b","c,d"
```

### Collapsing repeated separators with -cm
Input `colons.dat`:
```
a:::b:c
```
Command (collapse):
```
csvfix read_dsv -s ':' -cm colons.dat
```
Output:
```
"a","b","c"
```
Without `-cm` the empty fields are preserved:
```
csvfix read_dsv -s ':' colons.dat
```
```
"a","","","b","c"
```

## Notes
- Default separator is the pipe `|`. Override with `-s`; only one character is
  allowed, with `\t` as the sole recognised escape sequence (a literal tab).
  A space is treated as an ordinary character, so `-s ' '` reads
  space-separated data.
- Backslash is always the escape character in DSV input regardless of the
  chosen separator: `\<sep>` yields a literal separator inside a field, and
  `\\` yields a single backslash. A trailing backslash at end of line is an
  error ("Escape at end of line").
- `-csv` changes only how a parsed field's *contents* are interpreted (CSV
  unquoting); the field splitting still happens on the DSV separator.
- `-f` uses 1-based numeric indexes only; there is no `-fn` (name-based)
  selection for `read_dsv`. The related `write_dsv` command does support `-fn`.
- read_dsv reads input strictly by lines and does not treat the first row as a
  header; every line, including any header, is converted to a CSV row.
- The inverse command is `write_dsv` (CSV -> DSV). See also `read_fixed` and
  `read_multi` for other input-format converters.

## See also
- [write_dsv](write_dsv.md)
- [read_fixed](read_fixed.md)
- [read_multi](read_multi.md)
