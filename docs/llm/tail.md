# csvfix tail

> list last CSV records

## Synopsis
```
csvfix tail [options] [file ...]
```

## Description
Reads CSV records from the named files (or standard input if none are given) and writes only the last N records to standard output. The default is the last 10 records. Because a single CSV record may span multiple physical lines (embedded newlines inside quoted fields), this is counted in CSV records, not text lines. Output fields are re-quoted on write (every field is double-quoted by default).

## Options
- `-n count` — display the last `count` records (default: `10`).

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#ALL,SKIP`, so it supports the full common-option set plus row skipping: output `-o`, CSV separators `-sep`/`-rsep`/`-osep`, `-ibl` (ignore blank input lines), `-smq` (smart quotes on output), `-sqf` (fields that must be quoted), `-hdr s` (write string `s` as a header record), `-ifn` (ignore field-name record), and `-skip t` (drop records where test `t` is true). It does not support `-pass`.

## Examples

### Default: last 10 records
Input (`/tmp/tail_in.csv`):
```
a,1
b,2
c,3
d,4
e,5
f,6
g,7
h,8
i,9
j,10
k,11
l,12
```
Command:
```
csvfix tail /tmp/tail_in.csv
```
Output:
```
"c","3"
"d","4"
"e","5"
"f","6"
"g","7"
"h","8"
"i","9"
"j","10"
"k","11"
"l","12"
```

### Last N with -n and an added header
Same input as above.
Command:
```
csvfix tail -n 2 -hdr "letter,num" /tmp/tail_in.csv
```
Output:
```
letter,num
"k","11"
"l","12"
```

## Notes
- Default count is 10; if the input has fewer than `count` records, all records are emitted.
- `tail` must read the entire input before it can emit anything, since the last records are only known at end of stream (it buffers a sliding window of the last `count` rows). It is therefore not suitable for unbounded streaming input.
- Records dropped by `-skip` are not counted toward the last N and are not emitted; the window holds the last N non-skipped records.
- The `-hdr` string is written verbatim (not re-quoted) before the data records, so include your own quoting if needed.
- Output is fully quoted by default; use `-smq` for smart quoting or `-sqf` to restrict which fields are quoted.
- Sibling command [head](head.md) does the inverse, emitting the first N records.

## See also
- [head](head.md)
- [shuffle](shuffle.md)
- [conventions](conventions.md)
