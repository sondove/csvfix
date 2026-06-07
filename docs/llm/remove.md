# csvfix remove

> remove rows matching regular expression or range

## Synopsis
```
csvfix remove [options] [file ...]
```

## Description
`remove` is the inverse of `find`: it performs a grep-like search over CSV
records and writes only the records that do NOT match to standard output. It is
the CSV-aware equivalent of `grep -v`. Input is read from the named files (or
standard input if none are given) as CSV; matched (filtered) rows are dropped
and every surviving row is written out as CSV. You must supply at least one of
`-e`, `-s`, `-ei`, `-si`, `-r`, `-l`, `-fc` or `-if`; otherwise the command
errors. Tests combine with OR semantics: a row is removed if any supplied
expression/range/length/count test matches.

## Options
- `-f fields` — comma-separated list of 1-based field indices to apply the search to. If omitted, all fields are searched. Mutually exclusive with `-fn`.
- `-fn names` — comma-separated list of header names (case-insensitive, matched against the first input row) selecting the fields to search. Mutually exclusive with `-f`.
- `-e expr` — regular expression to search for; a row is removed if a searched field matches. May be given multiple times (any match removes the row).
- `-s expr` — like `-e` but `expr` is treated as a literal string, not a regex. May be repeated.
- `-ei expr` — like `-e` but matches case-insensitively. May be repeated.
- `-si expr` — like `-s` (literal string), matched case-insensitively. May be repeated.
- `-r range` — numeric/lexical range to search for, e.g. `40:50`. A row is removed if a searched field falls in the range. May be repeated.
- `-l length` — remove rows where a searched field has the given length; may itself be a range (e.g. `3:5`).
- `-fc count` — remove rows by CSV field count; `count` may be a single integer or a range `min:max`.
- `-if expr` — remove the row if the eval-style expression evaluates to true (fields referenced as `$1`, `$2`, ...). See [eval](eval.md) for the expression language.
- `-n` — output only the count of non-matching (surviving) rows, not the rows themselves.

Positional arguments are zero or more input file names; with none, input is read from standard input.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched
case-insensitively against the first input row (e.g. `remove -fn city -e Paris`).
`-f` and `-fn` are mutually exclusive. The header row is treated as ordinary
data and passed through (it is not searched specially), so it appears in the
output unless you also pass `-ifn` to drop the field-name record.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Based on
this command's help, the applicable groups are: output `-o file`; CSV separator
`-sep`; `-ibl` (ignore blank input lines); `-ifn` (ignore field-name header
record); and `-smq` (smart quotes on output). Note this command does not list
the full separator family (`-rsep`/`-osep`) or the `-skip`/`-pass` row filters.

## Examples

Input `names.csv`:
```
"Alice","London","30"
"Bob","Paris","45"
"Carol","Berlin","25"
```

Remove rows where field 2 matches the regex `Paris`:
```
csvfix remove -f 2 -e Paris names.csv
```
```
"Alice","London","30"
"Carol","Berlin","25"
```

Remove rows where field 3 falls in the numeric range 40-50:
```
csvfix remove -f 3 -r 40:50 names.csv
```
```
"Alice","London","30"
"Carol","Berlin","25"
```

Count surviving (non-matching) rows only:
```
csvfix remove -f 2 -e Paris -n names.csv
```
```
2
```

Remove rows using an eval expression (drop where age > 40):
```
csvfix remove -if '$3 > 40' names.csv
```
```
"Alice","London","30"
"Carol","Berlin","25"
```

Select the search field by header name and drop the header from output. Input
`hdr.csv`:
```
"name","city","age"
"Alice","London","30"
"Bob","Paris","45"
```
```
csvfix remove -fn city -e Paris -ifn hdr.csv
```
```
"Alice","London","30"
```

Remove rows by CSV field count (drop rows with 3 fields). Input `fc.csv`:
```
"a","b"
"c","d","e"
"f"
```
```
csvfix remove -fc 3 fc.csv
```
```
"a","b"
"f"
```

## Notes
- At least one test flag (`-e`/`-s`/`-ei`/`-si`/`-r`/`-l`/`-fc`/`-if`) is required.
- Multiple tests combine with OR: a row is removed if it matches any of them.
- `-e` uses regular expressions; `-s` uses literal strings. The `-ei`/`-si` variants are the case-insensitive forms.
- Without `-f`/`-fn`, the search is applied to all fields in the row.
- For `-if`, reference fields with `$N` (1-based), e.g. `$3 > 40`. Using `@3` syntax is invalid and raises a compilation error.
- The header row is just data unless you pass `-ifn`; when using `-fn`, the names are still matched against that first row to resolve indices.
- `remove` is exactly `find` with inverted output. To keep matching rows instead, use `find`.

## See also
- [find](find.md)
- [eval](eval.md)
- [exclude](exclude.md)
- [unique](unique.md)
