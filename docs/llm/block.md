# csvfix block

> perform actions on blocks of CSV records

## Synopsis
```
csvfix block [options] [file ...]
```

## Description
`block` identifies contiguous runs ("blocks") of CSV records, delimited by a begin record and an end record, each matched by an expression in the [eval expression language](conventions.md). It reads CSV from the named files (or standard input if none are given) and processes records sequentially as a stream. Depending on the chosen action it can keep only block records (`-k`), remove block records (`-r`), or pass every record through with a marker field prepended indicating whether the record is inside a block (`-m`). A block starts at the first record where `-be` evaluates true and ends at the next record where `-ee` evaluates true; if no end record is found before end-of-file, end-of-file ends the block.

## Options
- `-be expr` — expression (eval language) marking the start of a block; a record where this evaluates true begins a block. Required.
- `-ee expr` — expression marking the end of a block; a record where this evaluates true ends the block. Required.
- `-k` — keep blocks in the output and remove records not in a block.
- `-r` — remove blocks from the output (keep only records not in a block).
- `-m mark` — mark mode: a comma-separated list of one or two values. Each output record gets a new first field: the first list value for block records, the second (or empty string if omitted) for non-block records. All records are written.
- `-x` — exclude the begin/end (marker) records themselves from the block.

Exactly one of `-k`, `-r` or `-m` must be supplied; otherwise the command errors with "Need only one of ...". Both `-be` and `-ee` are required.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token is `#ALL`, so all standard groups apply: output `-o`; CSV separators `-sep` / `-rsep` / `-osep`; `-ibl` ignore blank input lines; `-smq` smart-quote output and `-sqf` quote specific fields; `-hdr` write a header record; `-ifn` ignore a field-name header record.

## Examples

Input (`countries.csv`):
```
GB,United Kingdom
FR,France
DE,Germany
NL,Netherlands
IT,Italy
US,United States
```

### Mark Euro members (`-m`)
Mark every record from `FR` to `IT` (inclusive) with `EUR`, and all others with `---`. The marker becomes the new first field.
```
csvfix block -be '$1 == "FR"' -ee '$1 == "IT"' -m 'EUR,---' countries.csv
```
Output:
```
"---","GB","United Kingdom"
"EUR","FR","France"
"EUR","DE","Germany"
"EUR","NL","Netherlands"
"EUR","IT","Italy"
"---","US","United States"
```

### Keep only the block (`-k`)
```
csvfix block -be '$1 == "FR"' -ee '$1 == "IT"' -k countries.csv
```
Output:
```
"FR","France"
"DE","Germany"
"NL","Netherlands"
"IT","Italy"
```

### Remove the block (`-r`)
```
csvfix block -be '$1 == "FR"' -ee '$1 == "IT"' -r countries.csv
```
Output:
```
"GB","United Kingdom"
"US","United States"
```

### Keep block but exclude its delimiters (`-k -x`)
With `-x`, the begin (`FR`) and end (`IT`) records are not treated as part of the block.
```
csvfix block -be '$1 == "FR"' -ee '$1 == "IT"' -k -x countries.csv
```
Output:
```
"DE","Germany"
"NL","Netherlands"
```

## Notes
- Block detection is stateful and sequential: while outside a block only `-be` is evaluated; while inside a block only `-ee` is evaluated. A record cannot simultaneously close one block and open the next on the same row.
- An unterminated block (no `-ee` match before EOF) extends to end-of-file.
- `-m` always writes all records and prepends a field; `-k`/`-r` filter records and do not add a field. With `-m` and a single mark value, non-block records get an empty leading field.
- A mark list with more than two entries is an error ("Invalid mark string").
- Field references in expressions use `$1`, `$2`, ... (1-based). See the expression language docs for operators and functions.
- This command has no `-f`/`-fn` field-name selection; fields are referenced only inside the `-be`/`-ee` expressions.

## See also
- [find](find.md) — select records matching an expression
- [remove](remove.md) — remove records
- [eval](eval.md) — evaluate expressions over fields
