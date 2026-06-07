# csvfix truncate

> truncate CSV records

## Synopsis
```
csvfix truncate [options] [file ...]
```

## Description
Truncates CSV records by removing the rightmost fields, keeping only the first `-n` fields of each row. It reads CSV from the named input files (or standard input if none are given) and writes the truncated rows to standard output (or to the `-o` file). The command never adds fields: rows that already have fewer than `-n` fields are passed through unchanged. This differs from `order`, which would append empty fields to reach a fixed width, and from `pad`, which deliberately adds fields.

## Options
- `-n count` — number of fields to truncate each record to. Required; must be an integer greater than or equal to zero. Rows with more than `count` fields are cut to `count` fields; rows with `count` or fewer fields are left as-is.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token `#ALL,SKIP,PASS` means it supports the full common set: output `-o`; CSV separators `-sep` / `-rsep` / `-osep`; `-ibl` ignore blank lines; `-smq` smart-quote and `-sqf` quote-specific-fields; `-hdr` write a header record; `-ifn` ignore the field-name header record; plus `-skip`/`-pass` row filtering.

## Examples

Truncate each row to its first 2 fields (input `names.csv`):
```
"Charles","Dickens","M"
"Jane","Austen","F"
"Herman","Melville","M"
```
```
csvfix truncate -n 2 names.csv
```
Output:
```
"Charles","Dickens"
"Jane","Austen"
"Herman","Melville"
```

Truncate to 1 field, and the no-op behaviour for short rows. With `mixed.csv`:
```
"a","b","c","d"
"x","y"
"p"
```
```
csvfix truncate -n 2 mixed.csv
```
Output (the 4-field row is cut to 2; the 2-field and 1-field rows are unchanged):
```
"a","b"
"x","y"
"p"
```

## Notes
- `-n` is mandatory; omitting it fails with `ERROR: Required option -n missing`.
- A negative `-n` is rejected; `-n 0` is accepted and produces empty rows.
- Truncation only ever removes fields. If `count` is greater than or equal to a row's field count, that row is emitted unchanged (verified: `-n 4` on 3-field rows leaves them intact).
- `truncate` selects fields by position only (count from the left); it has no `-f` / `-fn` named-field selection. To keep arbitrary fields or reorder, use `order`. To grow rows to a fixed width, use `pad`.
- With `-pass`, rows matching the test are emitted as-is (not truncated); with `-skip`, matching rows are dropped entirely.

## See also
- [order](order.md)
- [pad](pad.md)
- [remove](remove.md)
