# csvfix write_multi

> convert CSV to multi-line records

## Synopsis
```
csvfix write_multi [options] [file ...]
```

## Description
`write_multi` converts flat CSV input into multi-line master/detail records. Each input row is split into a master part (the fields named by `-m`) and a detail part (the fields named by `-d`, or all non-master fields if `-d` is omitted). Input rows are read in order; whenever the master fields change from the previous row, the master record is written on its own line, followed by one detail line per row that shares that master. Consecutive groups can be separated by a record-separator line via `-rs`. Output is normal CSV, written to stdout unless `-o` is given.

## Options
- `-m fields` — comma-separated list of 1-based field indexes that make up the master record (required). Master grouping is based on consecutive equal master values, so input should already be sorted by the master fields.
- `-d fields` — comma-separated list of 1-based field indexes that make up each detail record (default: all fields not listed in `-m`).
- `-rs sep` — emit the literal string `sep` as a separator line after each master/detail group (default: none, i.e. no separator lines). Escape sequences in `sep` are interpreted. A trailing separator is also written after the final group.

## Common options
Also accepts the standard options — see [conventions](conventions.md). From the source generic-flag token `#SMQ,SEP,IBL,IFN,OFL`, the applicable groups are: smart-quote `-smq` (and `-sqf fields`); CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank input lines; `-ifn` ignore the field-name header record; output `-o file` and `-hdr s`. (Row filtering `-skip`/`-pass` is honoured by the code but not declared in the help token.)

## Examples

### Example 1: master/detail with separator and smart quotes
Input `/tmp/wm1.csv`:
```
bob,dylan,bringing it all back home
bob,dylan,blonde on blonde
bob,dylan,john wesley harding
nick,drake,bryter later
nick,drake,pink moon
```
Command:
```
csvfix write_multi -m 1,2 -rs '-----' -smq /tmp/wm1.csv
```
Output:
```
bob,dylan
bringing it all back home
blonde on blonde
john wesley harding
-----
nick,drake
bryter later
pink moon
-----
```

### Example 2: default detail fields (no -smq, default quoting)
Input `/tmp/wm2.csv`:
```
A100,widget,5
A100,gadget,2
B200,sprocket,9
```
Command:
```
csvfix write_multi -m 1 /tmp/wm2.csv
```
Output (fields are quoted by default; pass `-smq` to quote only when needed):
```
"A100"
"widget","5"
"gadget","2"
"B200"
"sprocket","9"
```

## Notes
- `-m` is mandatory; omitting it (or giving an empty list) is an error.
- Master grouping is purely positional: a new master record is emitted only when the master field values differ from the immediately preceding row. Unsorted input will produce repeated master groups.
- With `-rs`, a separator line is written between groups AND after the last group (trailing separator). With no `-rs`, no separator lines are produced.
- By default output is fully quoted CSV; use `-smq` for smart (minimal) quoting, matching the manual's example.
- The detail line for a row contains the detail fields as a CSV row; the master line contains the master fields as a CSV row.
- This is the inverse of `read_multi`, which reassembles multi-line master/detail records back into flat CSV.

## See also
- [read_multi](read_multi.md)
- [order](order.md)
