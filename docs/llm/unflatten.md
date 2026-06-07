# csvfix unflatten

> convert single row to multiple rows

## Synopsis
```
csvfix unflatten [options] [file ...]
```

## Description
`unflatten` is the inverse of [`flatten`](flatten.md): it takes rows that pack one or more key fields plus a list of repeated data values, and explodes each input row into several output rows. For every group of data fields (1 by default, set with `-n`), it emits one output row consisting of the key field(s) followed by that group of data values. Input is read from the named files or from standard input; the resulting CSV rows are written to standard output (or to a file with `-o`).

## Options
- `-k key` — comma-separated list of 1-based key field indexes that are repeated on each output row (default: `1`, the first field). Key fields are excluded from the data values; values out of range are skipped.
- `-n data` — number of data fields to place on each output row (default: `1`). Must be a positive integer; a non-integer or value `<= 0` is an error. The last output row of a group may contain fewer than `n` data values if the data does not divide evenly.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Applicable groups (from the source generic token `#SMQ,SEP,IBL,IFN,OFL,SKIP`): smart-quote output `-smq` (and `-sqf fields`), CSV separators `-sep`/`-rsep`/`-osep`, ignore blank lines `-ibl`, ignore field-name header `-ifn`, output file/header `-o` and `-hdr`, and row filtering `-skip`.

## Examples

### Default: first field is key, one data value per row
Input (`unflat.csv`):
```
1,a,b,c
2,d,e
3,f,g,h,i
```
Command:
```
csvfix unflatten unflat.csv
```
Output:
```
"1","a"
"1","b"
"1","c"
"2","d"
"2","e"
"3","f"
"3","g"
"3","h"
"3","i"
```

### Multiple data fields per row (`-n 2`)
Input (`uf2.csv`):
```
101,red,small,blue,large
102,green,medium
```
Command:
```
csvfix unflatten -n 2 uf2.csv
```
Output:
```
"101","red","small"
"101","blue","large"
"102","green","medium"
```
The second input row has a single remaining data value, so its last (and only) output row carries just one.

### Multiple key fields (`-k 1,2`)
Input (`uf3.csv`):
```
A,2024,x,y,z
B,2025,p,q
```
Command:
```
csvfix unflatten -k 1,2 uf3.csv
```
Output:
```
"A","2024","x"
"A","2024","y"
"A","2024","z"
"B","2025","p"
"B","2025","q"
```

## Notes
- Output is quoted by default; combine with `-smq` for smart (minimal) quoting.
- Key fields named in `-k` are removed from the pool of data values, regardless of their position in the row; the remaining non-key fields are emitted in input order, `n` per output row.
- A row that contains only key fields (no data) produces no output rows.
- `-k` accepts only numeric indexes; unlike its sibling `flatten`, `unflatten` has no `-fn` (header-name) form.
- Each input row is processed independently — there is no cross-row grouping, so input need not be sorted by key.

## See also
- [flatten](flatten.md) — the inverse operation (multiple rows to single row)
- [split_char](split_char.md)
- [split_fixed](split_fixed.md)
