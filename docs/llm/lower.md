# csvfix lower

> convert to lowercase

## Synopsis
```
csvfix lower [options] [file ...]
```

## Description
Reads CSV from the named files (or standard input if none are given) and converts the alphabetic characters in fields to lower case. By default every field in every row is converted; use `-f` or `-fn` to restrict conversion to specific fields. Non-alphabetic characters are left unchanged. The result is written as CSV to standard output (or to the file given by `-o`). Sibling commands `upper` and `mixed` perform the equivalent upper-case and title-case transformations.

## Options
- `-f fields` — comma-separated list of 1-based field indices to convert (default: all fields). Mutually exclusive with `-fn`.
- `-fn names` — comma-separated list of header names to convert, matched against the first input row (default: all fields). Mutually exclusive with `-f`.

This command takes no positional arguments other than input file names.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched against the first input row. `-f` and `-fn` are mutually exclusive. When you use `-fn`, the header row itself is also processed (and thus lower-cased) and passed through like data unless you add `-ifn`, which drops the header record from the output entirely. Example: `csvfix lower -fn Last file.csv` lower-cases only the column whose header is `Last`.

## Common options
Also accepts the standard options — see [conventions](conventions.md).
Applicable groups for this command: output `-o`; CSV separator `-sep`; `-ibl` (ignore blank input lines); `-smq` (smart quotes on output); `-ifn` (ignore/drop the field-name header record). This command does not document the `#ALL,SKIP,PASS` generic groups, so `-skip`/`-pass` row filtering and `-rsep`/`-osep` are not part of its help.

## Examples

### Convert all fields to lowercase
Input (`/tmp/names.csv`):
```
"Charles","Dickens","M"
"Jane","Austen","F"
"Herman","Melville","M"
```
Command:
```
csvfix lower /tmp/names.csv
```
Output:
```
"charles","dickens","m"
"jane","austen","f"
"herman","melville","m"
```

### Convert only one field by index
Command (`-f 2` lower-cases just the second field):
```
csvfix lower -f 2 /tmp/names.csv
```
Output:
```
"Charles","dickens","M"
"Jane","austen","F"
"Herman","melville","M"
```

### Convert a field by header name, dropping the header
Input (`/tmp/hdr.csv`):
```
First,Last,Sex
"Charles","DICKENS","M"
"Jane","AUSTEN","F"
```
Command:
```
csvfix lower -fn Last -ifn /tmp/hdr.csv
```
Output:
```
"Charles","dickens","M"
"Jane","austen","F"
```
(Without `-ifn`, the header row is also output and itself lower-cased: `"First","last","Sex"`.)

## Notes
- Default behaviour with no `-f`/`-fn` is to convert every field of every row.
- Field indices for `-f` are 1-based.
- With `-fn` but without `-ifn`, the header row is treated as data: it is passed through to the output and any targeted columns in it are lower-cased too. Use `-ifn` to remove the header from output.
- Only alphabetic characters change; digits, punctuation, and separators are untouched. Quoting in the output follows the standard CSVfix rules (use `-smq` for smart quotes).
- `-f` and `-fn` cannot be combined.
- Available on all platforms (pure CSV processing, no ODBC/DLL dependency).

## See also
- [upper](upper.md)
- [mixed](mixed.md)
- [edit](edit.md)
