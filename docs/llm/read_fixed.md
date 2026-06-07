# csvfix read_fixed

> convert fixed format data to CSV

## Synopsis
```
csvfix read_fixed [options] [file ...]
```

## Description
Reads fixed-format (column-positional) text records and converts them to CSV. Each input line is treated as one record; you use `-f` to specify which character ranges to extract as fields. Input is read line by line from the named files (or standard input if none are given), and the extracted fields are written as a CSV row per line. By default each extracted value has its trailing spaces removed.

## Options
- `-f fields` — required. A comma-separated list of field specs, each of the form `start:width` (both 1-based, both must be positive integers). `start` is the 1-based character position in the line where the field begins; `width` is the number of characters to take. For example `-f 4:10,20:2` reads a 10-char field starting at position 4 and a 2-char field starting at position 20. Fields may appear in any order, may overlap, and may be repeated. If a field's start position is past the end of the line, an empty value is emitted.
- `-k` — keep (retain) trailing spaces in the extracted values rather than stripping them. Default: trailing spaces are removed (right-trimmed).

## Common options
Also accepts the standard options — see [conventions](conventions.md). From this command's generic-flag groups (`AIBL`, `SMQ`, `OFL`) it accepts: `-o file` (write output to a file instead of stdout), `-ibl` (ignore blank input lines), `-smq` (smart-quote output), and `-sqf fields` (force-quote the listed output fields).

## Examples

### Extract two fields by position
Input (`/tmp/fixed_names.dat`, name in cols 1-10, sex flag in col 11):
```
Geraldine F
Fred      M
Emmylou   F
```
Command:
```
csvfix read_fixed -f 1:10,11:1 /tmp/fixed_names.dat
```
Output:
```
"Geraldine","F"
"Fred","M"
"Emmylou","F"
```

### Keep trailing spaces with -k
Same input as above. Without `-k`, the name field is right-trimmed; with `-k` the original 10-char width is preserved.
```
csvfix read_fixed -k -f 1:10 /tmp/fixed_names.dat
```
Output:
```
"Geraldine "
"Fred      "
"Emmylou   "
```

### Force-quoting selected fields with -sqf
Input (`/tmp/f2.dat`):
```
AB12
```
Command (extract `1:2` and `3:2`, force-quote only field 1):
```
csvfix read_fixed -f 1:2,3:2 -sqf 1 /tmp/f2.dat
```
Output:
```
"AB",12
```

## Notes
- `-f` is mandatory; running without it fails with `Need fields specified with -f flag`.
- Field specs are validated: each must be exactly `int:int`, and neither value may be 0. Anything else (missing colon, non-integer, zero) raises `Invalid field specification`.
- Positions and widths are character counts, 1-based. There is no upper bound: a width extending past the end of the line simply yields the available characters; a start past the end yields an empty field.
- Default behaviour right-trims trailing spaces only; leading spaces inside the specified range are preserved.
- This command does not have an `-fn` (field-by-name) option and does not treat the first row as a header.
- The inverse command is [write_fixed](write_fixed.md) (CSV to fixed format). [split_fixed](split_fixed.md) splits a single existing CSV field into sub-fields by fixed positions, and [read_multi](read_multi.md) handles multi-line records.

## See also
- [write_fixed](write_fixed.md)
- [split_fixed](split_fixed.md)
- [read_multi](read_multi.md)
- [read_dsv](read_dsv.md)
