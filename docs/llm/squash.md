# csvfix squash

> squash duplicate rows to single row

## Synopsis
```
csvfix squash [options] [file ...]
```

## Description
Reads CSV input and groups rows that share the same value(s) in one or more
key fields (`-f`/`-fn`). For each group it emits a single output row consisting
of the key field(s) followed by the numeric sum of one or more value fields
(`-n`) across all rows in that group. Both `-f` (or `-fn`) and `-n` are
required. Reads from the named files or standard input and writes the squashed
rows to standard output (or `-o`). Output is unsorted by group key in input
order — the implementation accumulates groups in a map, so emitted rows come out
in an internal (effectively sorted) order, not the original row order.

## Options
- `-f fields` — key fields by 1-based numeric index (comma-separated list, e.g. `-f 1,2`). Required unless `-fn` is given; mutually exclusive with `-fn`.
- `-fn names` — key fields by header name (comma-separated), matched against the first input row. Alternative to `-f`.
- `-n fields` — value fields by 1-based numeric index whose values are summed per group (comma-separated). Required.
- `-nn val` — value to substitute when a `-n` field contains a non-numeric value. Without it, a non-numeric value in a `-n` field is an error. Must be an integer (or, with `-rn`, a real number).
- `-rn` — treat `-n` fields as real numbers rather than integers (default: integer summation).

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched against the first input row. `-f` and `-fn` are mutually exclusive, and exactly one of them must be supplied for the key. When using `-fn`, the first row is treated as a header for name resolution; pass `-ifn` to drop that header row so it is not processed as data. Names given to `-fn` are listed comma-separated (e.g. `-fn name,colour`). Note that `-n` is always specified by numeric index, even when the key is named.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The
generic-flag token `#SMQ,SEP,IBL,IFN,OFL,SKIP` applies: smart-quote output
(`-smq`, `-sqf`), CSV separators (`-sep`/`-rsep`/`-osep`), ignore blank lines
(`-ibl`), ignore field-name header (`-ifn`), header record (`-hdr`), output file
(`-o`), and row filtering (`-skip`).

## Examples

Sum a single value field grouped by one key field.

Input (`sq.csv`):
```
apple,red,3
banana,yellow,2
apple,green,5
banana,yellow,4
```
Command:
```
csvfix squash -f 1 -n 3 sq.csv
```
Output:
```
"apple","8"
"banana","6"
```

Group by a compound key (fields 1 and 2), summing field 3.

Command:
```
csvfix squash -f 1,2 -n 3 sq.csv
```
Output:
```
"apple","green","5"
"apple","red","3"
"banana","yellow","6"
```

Key by header name, dropping the header row, with real-number summation.

Input (`sqr.csv`):
```
name,qty
apple,1.5
banana,2.25
apple,0.5
```
Command:
```
csvfix squash -fn name -n 2 -rn -ifn sqr.csv
```
Output:
```
"apple","2"
"banana","2.25"
```

## Notes
- Both a key spec (`-f` or `-fn`) and `-n` are mandatory; omitting either is an error.
- Output rows are NOT in input order. Groups are stored in a map and emitted in that container's order (effectively sorted by the null-joined key), so do not rely on row ordering — pipe through `sort`/`order` if a specific order is needed.
- Each output row is: key field(s) followed by the per-group sums of the `-n` fields in the order they were listed.
- Default summation is integer. A non-integer value in a `-n` field is an error unless `-rn` (real summation) or `-nn` (substitute value) is used. With `-rn`, `-nn` must itself be a real number.
- Real-number sums are formatted with trailing zeros/decimals trimmed (e.g. `2.0` prints as `2`).
- This is a summation/aggregation command, not a deduplication command: to remove duplicate rows without aggregating, see `unique`.

## See also
- [order](order.md)
- [sort](sort.md)
- [pivot](pivot.md)
