# csvfix rmnew

> remove embedded newlines

## Synopsis
```
csvfix rmnew [options] [file ...]
```

## Description
`rmnew` removes newline (`\n`) characters that appear *inside* CSV fields. Although the CSV format can represent embedded newlines (inside quoted fields), some downstream applications cannot handle them. `rmnew` reads CSV from the named files (or standard input if none are given), and for each selected field replaces every embedded newline with a separator string (empty by default, or whatever you pass with `-s`). The transformed CSV is written to standard output (or to the file given with `-o`).

## Options
- `-s sep` — separator text used to replace each embedded newline. The escape sequence `\t` in `sep` is expanded to a literal tab; other `\`-escapes yield the following character literally. Default: empty string (newlines are removed with nothing inserted).
- `-x` — exclude (discard) all data after the first newline in a field; only the text before the first newline is kept. Mutually exclusive with `-s`.
- `-f fields` — comma-separated list of 1-based numeric field indexes to apply the command to. Default: all fields.
- `-fn names` — comma-separated list of header names (matched against the first input row) to apply the command to. Mutually exclusive with `-f`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive. When you use `-fn`, the header row is emitted as a normal (quoted) data row unless you also pass `-ifn` to drop the field-name record from the output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Applicable groups (from the source token `#SMQ,SEP,IBL,IFN,OFL,SKIP,PASS`): smart-quote `-smq` and `-sqf`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank input lines; `-ifn` ignore field-name header; output `-o` and `-hdr`; row filtering `-skip`/`-pass`.

## Examples

### Join address lines with a separator (`-s`)
Input `/tmp/addr.csv` (the second field contains embedded newlines):
```
"Joe Public","101 Somwhere St
Anytown
USA"
```
Command:
```
csvfix rmnew -s ', ' /tmp/addr.csv
```
Output:
```
"Joe Public","101 Somwhere St, Anytown, USA"
```

### Default (no `-s`): newlines removed with nothing inserted
Command:
```
csvfix rmnew /tmp/addr.csv
```
Output:
```
"Joe Public","101 Somwhere StAnytownUSA"
```

### Keep only text before the first newline (`-x`)
Command:
```
csvfix rmnew -x /tmp/addr.csv
```
Output:
```
"Joe Public","101 Somwhere St"
```

### Target a single field by header name (`-fn`)
Input `/tmp/n2.csv`:
```
name,notes
"Bob","line1
line2","x"
"Al","one
two"
```
Command:
```
csvfix rmnew -fn notes -s ' | ' /tmp/n2.csv
```
Output (note the header row is passed through and quoted):
```
"name","notes"
"Bob","line1 | line2","x"
"Al","one | two"
```

## Notes
- `-s` and `-x` cannot be combined; doing so raises the error `Flags -x and -s are mutually exclusive`.
- The default separator is the empty string, so `rmnew` with no `-s` simply deletes embedded newlines, concatenating the surrounding text.
- Only `\n` is treated as the embedded newline. Carriage returns (`\r`) are not specially handled by this command.
- In `sep`, only `\t` is given special meaning (tab); a trailing lone `\` raises `Invalid escape at end of string`.
- Fields without any embedded newline are left untouched.
- With `-fn`, the header record is reproduced as a quoted data row unless `-ifn` is also supplied.

## See also
- [edit](edit.md)
- [trim](trim.md)
- [order](order.md)
