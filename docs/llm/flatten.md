# csvfix flatten

> flatten to single row

## Synopsis
```
csvfix flatten [options] [file ...]
```

## Description
Flattens multiple input rows that share an identical key into a single output row, by appending each row's data fields to the row already accumulated for that key. Input is read as CSV from the named files (or standard input). A flattened row is written whenever the key value changes from one input row to the next, and the final accumulated row is written at end of input. Note that grouping is by *consecutive* runs of equal keys, not globally: if the same key reappears after a different key, it starts a new output row (so input is usually sorted by key first). An alternative master/detail mode (`-me`) appends each detail row to the most recently seen master row instead.

## Options
- `-k key` — comma-separated list of 1-based numeric key field indexes that define the group (default: `1`, the first field). Key fields are matched only by numeric index, never by name.
- `-r` — remove the key field(s) from the output row (default: retain the key at the start of each flattened row).
- `-f data` — data fields to collect, by 1-based numeric index (default: all fields except the key field(s)).
- `-fn data` — data fields to collect, by header name (see below). Mutually exclusive with `-f`.
- `-me expr` — master/detail mode. `expr` is an expression evaluated against each row; rows where it is true become the master, and subsequent non-matching rows are appended to that master and written out. Cannot be combined with `-k`, `-f`, or `-r`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row (e.g. `flatten -fn temp` selects the column headed `temp`). `-f` and `-fn` are mutually exclusive, and `-fn` controls only the *data* fields — the key is always given numerically via `-k`. The header row is processed like a data row (so it appears, flattened, in the output) unless you also pass `-ifn` to drop the field-name record.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Per the source generic-flag token `#SMQ,SEP,IBL,IFN,OFL,SKIP`: smart-quote output `-smq` (and `-sqf`), CSV separators `-sep`/`-rsep`/`-osep`, ignore blank lines `-ibl`, ignore field-name header `-ifn`, output file `-o` plus header record `-hdr`, and row filtering `-skip`.

## Examples

### Flatten consecutive rows by key
Input (`flat.csv`):
```
"A","a1"
"A","a2"
"A","a3"
"A","a4"
"B","b1"
"B","b2"
"A","a5"
"A","a6"
"C","c1"
"C","c2"
"C","c3"
"C","c4"
```
Command:
```
csvfix flatten flat.csv
```
Output:
```
"A","a1","a2","a3","a4"
"B","b1","b2"
"A","a5","a6"
"C","c1","c2","c3","c4"
```
Note the two separate `A` rows: grouping is over consecutive runs, so the later `A` block is not merged with the first.

### Master/detail flattening with -me
Input (`expend.csv`) — two-field rows are masters, three-field rows are details:
```
Joe,Blow
2012-01-13,Phone bill,105.10
Jane,Doe
2012-01-10,Groceries,5.10
2012-01-12,Drinks,7.15
```
Command:
```
csvfix flatten -me '$fields == 2' expend.csv
```
Output:
```
"Joe","Blow","2012-01-13","Phone bill","105.10"
"Jane","Doe","2012-01-10","Groceries","5.10"
"Jane","Doe","2012-01-12","Drinks","7.15"
```
Each detail row is prefixed with the most recent master. (`$fields` is the field count of the current row; a detail seen before any master is an error.)

### Select data field by name (-fn)
Input (`temps.csv`):
```
year,temp
1990,0
1990,4
1990,2
1991,1
1991,3
```
Command:
```
csvfix flatten -k 1 -fn temp -ifn temps.csv
```
Output:
```
"1990","0","4","2"
"1991","1","3"
```
Without `-ifn` the header row is itself processed, yielding a leading `"year","temp"` row in the output.

## Notes
- Grouping is by adjacent equal keys only; sort the input by the key field(s) first if you want all rows for a key collected together.
- The key is always specified by numeric index (`-k`); only the data field selection supports names (`-fn`).
- Default behaviour with no flags: key is field 1 (retained), data is every other field.
- `-me` is a distinct mode: it ignores/forbids `-k`, `-f`, and `-r`. It evaluates a master-record expression and appends details to the last master; there is no key-change grouping.
- Output is quoted CSV. Use `-smq` for smart quoting and `-sqf` to force quoting on specific fields.
- The inverse operation is [unflatten](unflatten.md).

## See also
- [unflatten](unflatten.md)
- [order](order.md)
- [sort](sort.md)
