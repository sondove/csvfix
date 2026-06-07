# csvfix merge

> merge fields

## Synopsis
```
csvfix merge [options] [file ...]
```

## Description
`merge` combines two or more input fields into a single output field, joining their values with a separator (a single space by default). It is typically used to build composite values, such as a full name from forename and surname fields. Input is read as CSV from the named files (or standard input if none are given), and the transformed CSV is written to standard output. By default the merged source fields are removed and replaced by the single merged field; the merged field is placed at the position of the first merged field unless `-p` overrides it.

## Options
- `-f fields` — comma-separated list of 1-based field indexes to merge. If used, at least two fields must be specified. If omitted, ALL fields in each row are merged into one field. Mutually exclusive with `-fn`.
- `-fn names` — comma-separated list of header names (matched against the first input row) to merge instead of numeric indexes. Mutually exclusive with `-f`.
- `-s sep` — separator string placed between merged values (default: a single space). Supports C-style escapes: `\t` (tab), `\n` (newline), `\r` (carriage return), `\\` (backslash); all other characters are literal. (Note: this is `-s`, not `-sep` — `-sep` is the unrelated input CSV field separator.)
- `-p pos` — 1-based position at which to insert the merged field in the output row. Must be a positive integer. Default: the position of the first field given to `-f` (or 1 when no `-f`/`-fn` is used). A position past the end of the row appends the merged field at the end.
- `-k` — keep (retain) the original merged source fields in the output as well as the new merged field. Default: source fields are removed.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive. With `-fn`, the header row is treated like a data row and is itself merged on output (e.g. `forename` and `surname` headers become a single `"forename surname"` field) unless you also pass `-ifn` to drop the header record before processing.

## Common options
Also accepts the standard options — see [conventions](conventions.md). From the source generic-flag token `#SMQ,SEP,IBL,IFN,OFL,SKIP,PASS`, the applicable groups are: smart-quoting `-smq` / `-sqf`; CSV separators `-sep` / `-rsep` / `-osep`; ignore blank lines `-ibl`; ignore field-name header `-ifn`; output `-o` plus header record `-hdr`; and row filtering `-skip` / `-pass`.

## Examples

### Merge two fields, keep originals, append at end
Input (`names.csv`):
```
"Charles","Dickens","M"
"Jane","Austen","F"
"Herman","Melville","M"
```
Command:
```
csvfix merge -f 1,2 -k -p 4 names.csv
```
Output:
```
"Charles","Dickens","M","Charles Dickens"
"Jane","Austen","F","Jane Austen"
"Herman","Melville","M","Herman Melville"
```

### Merge all fields into one with a custom separator
Using the same `names.csv`, omitting `-f` merges every field; here with a comma separator on the first two fields only:
```
csvfix merge -f 1,2 -s , names.csv
```
Output:
```
"Charles,Dickens","M"
"Jane,Austen","F"
"Herman,Melville","M"
```
With no `-f` at all, every field in the row is merged into a single field:
```
csvfix merge names.csv
```
Output:
```
"Charles Dickens M"
"Jane Austen M"
```
(Each row collapses to one space-joined field.)

### Select fields by name
Input (`h.csv`):
```
"forename","surname","sex"
"Charles","Dickens","M"
"Jane","Austen","F"
```
Command:
```
csvfix merge -fn forename,surname h.csv
```
Output:
```
"forename surname","sex"
"Charles Dickens","M"
"Jane Austen","F"
```
The header row is merged like data. Add `-ifn` to drop it before merging.

## Notes
- Default separator is a single space. Use `-s` (not `-sep`) to change the join separator; `-sep`/`-osep` control the CSV field delimiter of input/output instead.
- `-f` requires two or more fields; a single field triggers the error "Need to specify two or more fields with -f flag". The check is skipped when `-fn` is used.
- Omitting both `-f` and `-fn` merges every field of each row into one (this is the documented "all fields" behaviour); `-p` then defaults to 1.
- `-p` is validated as a positive integer; values past the row length append the merged field at the end.
- When merging by index, source fields outside the row's actual length are silently skipped.
- Output is requoted by CSVfix's normal rules; see [conventions](conventions.md) for quoting and separator behaviour.

## See also
- [split_char](split_char.md) — the inverse operation: split one field into several
- [printf](printf.md) — format fields into a string with full control
- [template](template.md) — build output from a template referencing fields
- [order](order.md) — reorder, select, or drop fields
