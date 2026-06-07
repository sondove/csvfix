# csvfix write_dsv

> convert CSV to DSV format

## Synopsis
```
csvfix write_dsv [options] [file ...]
```

## Description
Reads CSV input (from the named files or standard input) and writes each record as a delimiter-separated values (DSV) line, with fields joined by a single separator character (a pipe `|` by default). This is the inverse of `read_dsv`. Any occurrence of the separator character or a backslash inside a field value is escaped with a backslash on output. By default every field of every input row is written; use `-f`/`-fn` to select and reorder a subset of fields.

## Options
- `-f fields` — comma-separated list of 1-based field indexes to write, in the given order (default: all fields). Out-of-range indexes produce empty fields.
- `-fn names` — comma-separated list of header names (matched case-insensitively against the first input row) selecting the fields to write. Mutually exclusive with `-f`.
- `-s sep` — single-character DSV output separator (default: `|`). Use `-s '\t'` for a tab. Must be exactly one character (or the two-character escape `\t`); empty or other multi-character values are an error.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive. When `-fn` is used, the header row itself is written out (transformed like a data row) unless you also pass `-ifn`, which drops the field-name record from the output. Example: `write_dsv -fn first,last -ifn`.

## Common options
Also accepts the standard options — see [conventions](conventions.md). From the source generic-flag token `#IFN,IBL,OFL,SEP,SKIP`, this command honours: `-ifn` (ignore/drop the field-name header record), `-ibl` (ignore blank input lines), `-o file` (write output to a file instead of stdout), the CSV input separator options (`-sep`, `-rsep`, `-osep`), and `-skip t` (skip records for which expression `t` is true).

Note: `-s` sets the DSV *output* separator; `-sep`/`-rsep`/`-osep` control parsing/handling of the *CSV input*.

## Examples

### Example 1: default — all fields joined with pipe
Input `names.csv`:
```
Charles,Dickens,M
Jane,Austen,F
Flann,O'Brien,M
```
Command:
```
csvfix write_dsv names.csv
```
Output:
```
Charles|Dickens|M
Jane|Austen|F
Flann|O'Brien|M
```

### Example 2: select fields with -f and use a custom separator
Command:
```
csvfix write_dsv -f 1,2 names.csv
```
Output:
```
Charles|Dickens
Jane|Austen
Flann|O'Brien
```
With a colon separator (`-s :`), all fields:
```
csvfix write_dsv -s : names.csv
```
Output:
```
Charles:Dickens:M
Jane:Austen:F
Flann:O'Brien:M
```

### Example 3: select by header name and drop the header (-fn / -ifn)
Input `hdr.csv`:
```
first,last,sex
Charles,Dickens,M
Jane,Austen,F
```
Command (header passes through, transformed):
```
csvfix write_dsv -fn first,last hdr.csv
```
Output:
```
first|last
Charles|Dickens
Jane|Austen
```
With `-ifn` to drop the header record:
```
csvfix write_dsv -fn first,last -ifn hdr.csv
```
Output:
```
Charles|Dickens
Jane|Austen
```

### Example 4: separator and backslash escaping
Input `pipe.csv` (first field contains a literal pipe):
```
a|b,c
```
Command:
```
csvfix write_dsv pipe.csv
```
Output (the embedded `|` is escaped):
```
a\|b|c
```

## Notes
- Field values containing the output separator or a backslash are escaped with a backslash; `read_dsv` reverses this. No quoting is used — only backslash escaping.
- The separator must be a single character. Tabs can be given as the two-character escape `\t`; any other multi-character value is rejected with an error.
- `-f` indexes are 1-based and may reorder or duplicate fields; an index past the end of a row yields an empty field rather than an error.
- `-fn` requires the first input row to be a header; matching is case-insensitive. Use `-ifn` to keep that header from appearing in the DSV output.
- `-f` and `-fn` cannot be combined.
- Sibling/related commands: `read_dsv` (DSV to CSV, the inverse), and `write_fixed` for fixed-width output.

## See also
- [read_dsv](read_dsv.md)
- [write_fixed](write_fixed.md)
- [order](order.md)
