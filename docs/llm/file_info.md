# csvfix file_info

> add file information to output

## Synopsis
```
csvfix file_info [options] [file ...]
```

## Description
`file_info` reads CSV input and adds the source file name and line number to each output record. By default this information is prepended as a single new field at the start of the record, formatted as `name (lineno)`. It is useful for tracking which file (and where within it) a given row came from when processing many files at once. If input comes from standard input, the special token `<stdin>` is used as the file name. Line numbers are 1-based and counted per input file.

## Options
- `-b` — strip any path information from the file name, leaving only the base name (default: full path as given on the command line).
- `-tc` — output the file name and line number as two separate CSV fields instead of one combined `name (lineno)` field (default: single combined field).

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#ALL,SKIP,PASS`, so this command accepts the full standard set, including: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; `-smq` smart-quote and `-sqf` quote-specific-fields; `-hdr` write a header record; `-ifn` ignore a field-name header record; and `-skip`/`-pass` row filtering. Rows matched by `-pass` are written through unchanged (no file info is added); rows matched by `-skip` are dropped entirely.

## Examples

### Prepend base name and line number (combined field)
Input `cities.csv`:
```
London,GB
Paris,FR
Edinburgh,GB
```
Input `names.csv`:
```
Charles,Dickens,M
Jane,Austen,F
```
Command:
```
csvfix file_info -b cities.csv names.csv
```
Output:
```
"cities.csv (1)","London","GB"
"cities.csv (2)","Paris","FR"
"cities.csv (3)","Edinburgh","GB"
"names.csv (1)","Charles","Dickens","M"
"names.csv (2)","Jane","Austen","F"
```

### Two separate fields with -tc
Command:
```
csvfix file_info -b -tc cities.csv
```
Output:
```
"cities.csv","1","London","GB"
"cities.csv","2","Paris","FR"
"cities.csv","3","Edinburgh","GB"
```

## Notes
- Line numbering restarts at 1 for each input file and is independent of `-skip`/`-pass`; the counter reflects the physical record read.
- Without `-b`, the file name is exactly the path as supplied on the command line (relative or absolute), not a normalized/absolute path.
- For standard input the file name is the literal string `<stdin>`.
- `-b` and `-tc` may be combined, and either may be used alone; both default to off.
- Output is always quoted CSV (each field wrapped in double quotes) as produced by csvfix's writer.
- Related: [sequence](sequence.md) adds sequence numbers to rows; [put](put.md) inserts a constant or computed field.

## See also
- [sequence](sequence.md)
- [put](put.md)
- [order](order.md)
