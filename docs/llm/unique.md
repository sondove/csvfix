# csvfix unique

> filter duplicate CSV records

## Synopsis
```
csvfix unique [options] [file ...]
```

## Description
Reduces rows that contain duplicate field values to a single representative row, or (with `-d`) outputs only the rows that are duplicated. Reads CSV from the named files (or standard input if none are given) and writes CSV to standard output (or to `-o`). The first occurrence of a duplicate is the one kept. Input does not need to be sorted, but the command reads all data into memory, which can make it slow or unusable for very large datasets. With no `-f`/`-fn`, the entire record is used as the uniqueness key.

## Options
- `-f fields` — comma-separated list of 1-based field indexes to test for uniqueness. If omitted (and no `-fn`), the whole record is tested. Mutually exclusive with `-fn`.
- `-fn names` — comma-separated list of header names to test for uniqueness, matched case-insensitively against the first input row. Mutually exclusive with `-f`.
- `-d` — output only duplicate rows (the converse of the default, which outputs only unique rows). For a key seen N times, this emits the first row once when the second is encountered, then every subsequent occurrence — i.e. N rows total for that key (see Notes).

Positional arguments are zero or more input files; with multiple files they are concatenated, which is how `unique` is used to merge files while discarding duplicate rows.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row, e.g. `unique -fn dept`. `-f` and `-fn` are mutually exclusive. The header row is treated as ordinary data — it is passed through to the output (and counts toward uniqueness) unless you also pass `-ifn` to drop it from the output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Per the source flag token `#SMQ,SEP,IBL,IFN,OFL`, this command supports: output `-o` (OFL); CSV separators `-sep` / `-rsep` / `-osep` (SEP); `-ibl` ignore blank input lines (IBL); `-ifn` ignore field-name header record (IFN); `-smq` smart-quote and `-sqf` quote-specific-fields (SMQ). It also accepts `-hdr s` to write a header string. It does not document `-skip`/`-pass` row filtering.

## Examples

Input `post.csv`:
```
London,NW
London,W
Bristol,BS
London,E
Manchester,M
London,SE
London,SW
```

Remove rows duplicated on the whole record (here none are exact duplicates, so output equals input but with CSVfix quoting):
```
csvfix unique post.csv
```
```
"London","NW"
"London","W"
"Bristol","BS"
"London","E"
"Manchester","M"
"London","SE"
"London","SW"
```

Keep one row per distinct first field (`-f 1`):
```
csvfix unique -f 1 post.csv
```
```
"London","NW"
"Bristol","BS"
"Manchester","M"
```

List only the rows whose first field occurs more than once (`-d -f 1`):
```
csvfix unique -d -f 1 post.csv
```
```
"London","NW"
"London","W"
"London","E"
"London","SE"
"London","SW"
```

Select fields by header name with `-fn`, dropping the header from output via `-ifn`. Input `emp.csv`:
```
dept,name
sales,joe
eng,sue
sales,joe
eng,bob
```
```
csvfix unique -fn dept -ifn emp.csv
```
```
"sales","joe"
"eng","sue"
```

Merge multiple files, discarding duplicate rows, writing to a file:
```
csvfix unique -o merged.csv file1.csv file2.csv
```

## Notes
- Output is always re-quoted in CSVfix's canonical double-quoted form, even when the input was unquoted.
- The uniqueness key is built by concatenating the selected fields (or all fields) separated by NUL bytes; field indexes beyond the row length contribute an empty value rather than erroring.
- `-d` semantics: it does not collapse duplicates. When the second occurrence of a key is seen it emits the stored first row, then emits that second row, and emits every later occurrence too. So three identical rows produce three rows of output under `-d`.
- Default (no `-d`) keeps the first occurrence of each key and drops the rest.
- The whole dataset is held in memory; not suited to very large inputs.
- `-f` and `-fn` cannot be combined.
- Available on all platforms (built into this binary).

## See also
- [sort](sort.md)
- [order](order.md)
