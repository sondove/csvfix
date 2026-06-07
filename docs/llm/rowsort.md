# csvfix rowsort

> in-row sort of CSV fields

## Synopsis
```
csvfix rowsort [options] [file ...]
```

## Description
`rowsort` reorders the field *values within each individual CSV record* (it sorts left-to-right across a row, not the records themselves). It reads CSV from the named files or from standard input, sorts the selected fields of every data row among themselves, places the sorted values back into the same field positions, and writes the records to standard output. By default it sorts all fields of each row in ascending lexicographic order. The fields chosen for sorting must occupy a contiguous block of columns.

## Options
- `-f fields` — fields to sort, by 1-based numeric index. Accepts lists/ranges (e.g. `2:4`, `1,2,3`). The selected indexes must be contiguous or the command errors with `Fields to sort must be contiguous`. (default: all fields)
- `-fn names` — fields to sort, by header name matched against the first input row (e.g. `b:d`). The resolved columns must also be contiguous.
- `-a` — sort ascending (default).
- `-d` — sort descending. Mutually exclusive with `-a`.
- `-l` — lexicographic (string) order (default).
- `-n` — numeric order: each value is parsed as a real number for comparison. Mutually exclusive with `-l`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row (e.g. `rowsort -fn b:d`). `-f` and `-fn` are mutually exclusive. When `-fn` is used the header row is passed through unchanged (only data rows are sorted) unless you also pass `-ifn` to drop it from the output. With both `-f`/`-fn` the chosen columns must be contiguous.

## Common options
Also accepts the standard options — see [conventions](conventions.md). From the source generic-flag token `#SMQ,SEP,IBL,IFN,OFL,SKIP,PASS`: smart-quote output (`-smq`, `-sqf`), CSV separators (`-sep`/`-rsep`/`-osep`), ignore blank input lines (`-ibl`), ignore field-name header (`-ifn`), output-file/header (`-o`, `-hdr`), and row filtering (`-skip`/`-pass`).

## Examples

Sort all fields of each row, ascending lexicographic (default):
```
banana,apple,cherry
delta,bravo,alpha
```
```
csvfix rowsort rs1.csv
```
```
"apple","banana","cherry"
"alpha","bravo","delta"
```

Same input, descending (`-d`):
```
csvfix rowsort -d rs1.csv
```
```
"cherry","banana","apple"
"delta","bravo","alpha"
```

Numeric sort of a contiguous block (fields 2-4), leaving field 1 in place:
```
id,30,4,200
x,9,100,5
```
```
csvfix rowsort -f 2:4 -n rs2.csv
```
```
"id","4","30","200"
"x","5","9","100"
```
Without `-n` the same fields sort lexicographically (`"100" < "200" < "30"`):
```
csvfix rowsort -f 2:4 rs2.csv
```
```
"id","200","30","4"
"x","100","5","9"
```

Sort by header name with `-fn`; the header row passes through unchanged:
```
name,b,c,d
row,banana,apple,cherry
```
```
csvfix rowsort -fn b:d rs3.csv
```
```
"name","b","c","d"
"row","apple","banana","cherry"
```
Add `-ifn` to drop the header from the output:
```
csvfix rowsort -fn b:d -ifn rs3.csv
```
```
"row","apple","banana","cherry"
```

## Notes
- This command sorts values **within each row**; it never reorders the records. To sort whole records use [sort](sort.md).
- The fields to sort must be contiguous. `-f 1,3` (a gap) fails with `ERROR: Fields to sort must be contiguous`. With `-fn` the resolved positions are checked for contiguity after the header is read.
- Sorted values are written back starting at the lowest selected column position, so the block of fields keeps its location in the row; only the values' order changes.
- Numeric order (`-n`) compares values via real-number parsing; non-numeric fields parse as 0 and sort accordingly.
- Default ordering is ascending (`-a`) and lexicographic (`-l`).
- This command is not documented in the HTML manual (`docs/index.html`); the description above is derived from the built binary and `csvfix/src/csved_rsort.cpp`. The `-fn` flag is supported in this build.

## See also
- [sort](sort.md)
- [order](order.md)
- [edit](edit.md)
