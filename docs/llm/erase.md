# csvfix erase

> erase fields using regular expressions

## Synopsis
```
csvfix erase [options] [file ...]
```

## Description
Reads CSV from the named files (or standard input if none are given) and, for each selected field, removes the field from the record when its value matches (`-r`) or does not match (`-n`) a regular expression. Erasing removes the whole cell, so matching cells are dropped and the surviving cells shift left — the output row is shorter, it is not blanked in place. You must supply at least one of `-r` or `-n`; both may be given (and each may be repeated), in which case a field is erased if it satisfies any of the expressions. By default a row whose every (selected) field is erased is dropped entirely; use `-k` to keep it as an empty row.

## Options
- `-f fields` — list of 1-based field indexes to consider for erasure (default: all fields). Mutually exclusive with `-fn`.
- `-fn names` — list of fields to consider, by header name (matched against the first input row). Mutually exclusive with `-f`.
- `-r regexp` — erase fields that match this regular expression. May be repeated.
- `-n regexp` — erase fields that do NOT match this regular expression. May be repeated.
- `-k` — if all of a row's selected fields are erased, retain it as an empty row (default: such rows are deleted).

At least one of `-r` or `-n` is required, or the command errors with `Need at least one of -r or -n`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive. When `-fn` is used the header row is processed like any other data row (its cells are tested and may themselves be erased), so pass `-ifn` if you want the header name record ignored rather than treated as data.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token is `#ALL,SKIP,PASS`: ALL output/CSV options (`-o`, `-sep`/`-rsep`/`-osep`, `-ibl`, `-smq`, `-sqf`, `-hdr`, `-ifn`) plus `-skip`/`-pass` row filtering.

## Examples

Erase any field consisting entirely of digits (all fields considered):
```
apple,123,banana
orange,foo,456
```
```
csvfix erase -r '^[0-9]+$' input.csv
```
```
"apple","banana"
"orange","foo"
```

Restrict erasure to field 2 only (field 3 `456` is left untouched because it is not selected):
```
apple,123,banana
orange,foo,456
```
```
csvfix erase -f 2 -r '^[0-9]+$' input.csv
```
```
"apple","banana"
"orange","foo","456"
```

Use `-n` to erase the `score` cell wherever it is NOT all digits, selecting by header name. The header row is treated as data, so its `score` cell (non-numeric) is also erased:
```
name,score
zed,99
ann,abc
```
```
csvfix erase -fn score -n '^[0-9]+$' input.csv
```
```
"name"
"zed","99"
"ann"
```

## Notes
- Erasing deletes the matching cell entirely, shortening the record; it does not replace the value with an empty string. To blank rather than remove a field, use `edit` or `sub`.
- `-k` only has an effect when every selected field in a row is erased; rows that still have at least one surviving field are always output regardless of `-k`.
- Both `-r` and `-n` may be specified together and either may be repeated; a field is erased if it satisfies any supplied expression (OR semantics).
- With `-fn`, the header record is tested and possibly erased like data. Add `-ifn` to keep the header name record out of processing.
- Regular expressions use CSVfix's built-in regex engine; anchor with `^`/`$` for whole-field matches.

## See also
- [exclude](exclude.md)
- [remove](remove.md)
- [edit](edit.md)
- [find](find.md)
- [order](order.md)
