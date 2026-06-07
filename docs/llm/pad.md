# csvfix pad

> pad CSV records to fixed number of fields

## Synopsis
```
csvfix pad [options] [file ...]
```

## Description
`pad` reads CSV from the named files (or standard input if none are given) and pads each record out to a fixed number of fields by appending new fields to the right-hand side of the record. It is useful when the input contains rows with varying numbers of fields but a downstream consumer requires a fixed field count. Rows that already have more than the target number of fields are left untouched — `pad` never removes or truncates fields (use `truncate` for that). The padded records are written as CSV to standard output (or to the file given by `-o`).

## Options
- `-n count` — number of fields to pad each record to. `count` must be an integer >= 0. Required. (If a row has more fields than `count`, it is passed through unchanged.)
- `-p vals` — comma-separated list of values to pad with (default: the empty string). The first missing field gets the first value, the second missing field the second value, and so on; once the list is exhausted, the last value in the list is reused for all remaining padding positions. For example `-p NULL` pads every added field with `NULL`; `-p NULL,0` pads the first added field with `NULL` and all subsequent added fields with `0`.

This command takes zero or more positional input file arguments after the options; with none it reads standard input.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source help token is `#ALL,SKIP,PASS`, so the full ALL group applies: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; `-smq` smart-quote and `-sqf` quote-specific-fields; `-hdr` write a header record; `-ifn` ignore (drop) a field-name header record; plus `-skip`/`-pass` row filtering.

## Examples

Input file `sales_quarter.csv`:
```
2000,200,550
2001,178,200,233,140
2002,55,104,119
2003,77
```

Pad every record out to 5 fields, using `NULL` for added fields:
```
csvfix pad -n 5 -p NULL sales_quarter.csv
```
Output (verified):
```
"2000","200","550","NULL","NULL"
"2001","178","200","233","140"
"2002","55","104","119","NULL"
"2003","77","NULL","NULL","NULL"
```
Note the second row already had 5 fields, so it is unchanged.

Pad to 4 fields, using `NULL` for the first added field and `0` for the rest:
```
csvfix pad -n 4 -p NULL,0 sales_quarter.csv
```
Output (verified):
```
"2000","200","550","NULL"
"2001","178","200","233","140"
"2002","55","104","119"
"2003","77","NULL","0"
```
Here the second row (5 fields) exceeds 4 and is left untouched, and the third row (already 4 fields) needs no padding.

## Notes
- `-n` is effectively required: without it the command errors with `Need -n flag to specify field count`.
- Default padding (no `-p`) uses the empty string, which is rendered as `""` in the quoted output.
- `pad` only ever adds fields to the right; it never removes fields, never reorders, and never drops rows. Rows with more than `-n` fields pass through unchanged.
- When `-p` supplies more values than are needed for a given row, only the leading values needed are used; when it supplies fewer, the final value is repeated.
- A typical pipeline use is to give every record the same field count before feeding into a fixed-schema consumer (e.g. `csvfix read_multi ... | csvfix pad -n 4`), or to append a constant tag field, e.g. `csvfix pad -n 4 -p 'Y'` to add a `Y` field to 3-field rows before an SQL update.
- Related siblings: `truncate` removes rightmost fields (the inverse), `order` reorders/duplicates/removes fields, and `put` inserts a literal value.

## See also
[order](order.md), [truncate](truncate.md), [put](put.md), [read_multi](read_multi.md)
