# csvfix find

> find rows matching regular expression or range

## Synopsis
```
csvfix find [options] [file ...]
```

## Description
`find` filters CSV input, passing through only the rows that match one or
more criteria: a regular expression, a literal string, a value range, a field
length, a field count, or an eval expression. It is like UNIX `grep` but
understands CSV records and can restrict matching to specific fields. Input is
read from the named files (or standard input if none are given) and matching
rows are written as CSV to standard output (or the `-o` file). The sibling
`remove` command is identical but outputs the rows that do NOT match.

## Options
- `-f fields` — comma-separated list of 1-based field indexes to search. If omitted, all fields are searched.
- `-fn names` — comma-separated list of header names to search (case-insensitive against the first input row); mutually exclusive with `-f`.
- `-e expr` — regular expression to match. May be repeated; a row matches if any `-e` matches.
- `-s expr` — like `-e` but `expr` is treated as a literal string, not a regex (regex metacharacters are escaped). May be repeated.
- `-ei expr` — like `-e` but matches ignoring case. May be repeated.
- `-si expr` — like `-ei` but `expr` is a literal string, not a regex (case-insensitive). May be repeated.
- `-r range` — value range `lo:hi` (inclusive). If both ends are numeric, numeric comparison is used and non-numeric fields never match; otherwise string comparison is used (e.g. `A:C` matches strings from "A" up to and including "C"). May be repeated.
- `-l length` — match fields of a given character length; a single number (`-l 20`) or a range (`-l 1:10`). May be repeated.
- `-fc count` — match rows by field count: a single number (`-fc 3`) or a range (`-fc 2:6`, open-ended forms `2:` and `:6` allowed). Applies to the whole row, not a single field.
- `-if expr` — output a row only if the eval expression evaluates to true (fields referenced as `$1`, `$2`, ...). See the expression language.
- `-n` — output only the integer count of matched rows (plain text, not CSV), instead of the rows themselves.

At least one of `-e`, `-ei`, `-s`, `-si`, `-r`, `-l`, `-fc` or `-if` must be
supplied. A field matches if any of its regex/range/length tests pass; a row
matches if any of its searched fields matches.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched
case-insensitively against the first input row. `-f` and `-fn` are mutually
exclusive. When `-fn` is used the header row is consumed to resolve the names
and is not written to output; without `-fn` (using `-f` or no field flag) the
first row is treated as ordinary data unless you pass `-ifn` to drop a
field-name header.

## Common options
Also accepts the standard options — see [conventions](conventions.md).
Applicable groups: output `-o`, CSV separator `-sep`, ignore blank lines
`-ibl`, ignore field-name header `-ifn`, and smart-quote output `-smq`.

## Examples

Input `emp.csv`:
```
"1","Alice","Sales","50000"
"2","Bob","Engineering","75000"
"3","Carol","Sales","62000"
"4","Dave","Marketing","48000"
```

Match rows containing "Sales" in any field:
```
csvfix find -e Sales emp.csv
```
```
"1","Alice","Sales","50000"
"3","Carol","Sales","62000"
```

Match a numeric range only in the salary field (field 4):
```
csvfix find -f 4 -r 60000:80000 emp.csv
```
```
"2","Bob","Engineering","75000"
"3","Carol","Sales","62000"
```

Count matching rows instead of printing them:
```
csvfix find -e Sales -n emp.csv
```
```
2
```

Filter with an eval expression (salary greater than 60000):
```
csvfix find -if '$4 > 60000' emp.csv
```
```
"2","Bob","Engineering","75000"
"3","Carol","Sales","62000"
```

Select a field by header name. Input `emp_hdr.csv`:
```
id,name,dept,salary
1,Alice,Sales,50000
2,Bob,Engineering,75000
```
```
csvfix find -fn dept -e Sales emp_hdr.csv
```
```
"1","Alice","Sales","50000"
```
(The header row is consumed by `-fn` and not emitted.)

## Notes
- At least one matching flag is required; otherwise the command errors.
- `-e`/`-ei` are regular expressions (CSVfix's regex engine is basic); `-s`/`-si` escape their argument so it is matched literally. `-ei`/`-si` ignore case.
- Ranges are inclusive. A numeric range (`-r 10:50`) never matches non-numeric field values; a non-numeric range (`-r A:C`) uses string comparison.
- `-l` measures field length in characters; `-fc` measures the number of fields in the whole row and is independent of `-f`/`-fn`.
- `-n` writes a bare number followed by a newline, not a CSV row.
- `remove` is the exact inverse of `find` (outputs non-matching rows), sharing the same flags.

## See also
[remove](remove.md), [edit](edit.md), [eval](eval.md), [exclude](exclude.md)
