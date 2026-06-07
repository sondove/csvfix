# csvfix head

> list first CSV records

## Synopsis
```
csvfix head [options] [file ...]
```

## Description
Reads CSV from the named files (or standard input if none are given) and writes the first N CSV records to output. By default N is 10. Note that this counts CSV *records*, not lines: a single record may span multiple physical lines if it contains embedded newlines inside quoted fields, so `head` is not equivalent to the Unix `head` line counter. Output is written as CSV (fields re-quoted) to standard output unless `-o` is given.

## Options
- `-n count` — display the first `count` records (default: 10). `count` must be an integer greater than zero, or the command errors.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#ALL,SKIP`, so it supports the full common I/O set — output `-o`, CSV separators `-sep`/`-rsep`/`-osep`, `-ibl` (ignore blank lines), `-smq` (smart quoting), `-sqf` (fields to force-quote), `-hdr s` (emit header record), `-ifn` (ignore field-name header) — plus row filtering with `-skip`/`-pass`. There is no `-fn` named-field support in this command.

## Examples

Input `/tmp/h12.csv`:
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

Default takes the first 10 records:
```
csvfix head /tmp/h12.csv
```
Output:
```
"a","1"
"b","2"
"c","3"
"d","4"
"e","5"
"f","6"
"g","7"
"h","8"
"i","9"
"j","10"
```

Limit to the first 3 records with `-n`:
```
csvfix head -n 3 /tmp/h12.csv
```
Output:
```
"a","1"
"b","2"
"c","3"
```

A header row is treated as an ordinary data record and is included in the count. Input `/tmp/hdr.csv`:
```
name,age
Alice,30
Bob,25
Carol,40
```
```
csvfix head -n 2 /tmp/hdr.csv
```
Output:
```
"name","age"
"Alice","30"
```

## Notes
- `-n` must be a positive integer; `-n 0` or a negative/non-integer value is an error.
- The command does not understand header rows specially. The header is the first record and is counted by `-n` (use `-ifn` if you want to drop a field-name header from output).
- Output fields are always re-quoted; whitespace and quoting in the input are normalised in the output, as shown above.
- `-skip`/`-pass` filtering is applied first: filtered-out records are neither counted toward `-n` nor written.
- Sibling command [tail](tail.md) does the same for the last N records.

## See also
- [tail](tail.md)
- [order](order.md)
