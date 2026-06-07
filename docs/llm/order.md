# csvfix order

> change field order

## Synopsis
```
csvfix order [options] [file ...]
```

## Description
Re-orders, duplicates or removes fields in a CSV stream. Reads CSV from the named files (or standard input if none are given) and writes the transformed CSV to standard output. You select which input fields appear in the output, and in what order, using one of `-f`, `-rf`, `-xf` or `-fn`. A field may be listed more than once (to duplicate it). By default, if a requested field index does not exist in an input row, an empty field is emitted in its place; `-nc` suppresses that.

## Options
Exactly one of `-f`, `-rf`, `-xf` or `-fn` must be given.

- `-f fields` — comma-separated list of 1-based field indexes giving the output order. Fields may repeat (duplication). Ranges are allowed with `:`, so `-f 1:5,10` equals `-f 1,2,3,4,5,10`; ranges may descend, e.g. `-f 4:1`. Missing indexes produce empty fields unless `-nc` is set.
- `-rf fields` — like `-f`, but indexes count from the END of the record (1 = last field, 2 = second-to-last, ...). Also accepts ranges.
- `-xf fields` — exclude mode: output ALL fields EXCEPT the 1-based indexes listed; surviving fields keep their original order.
- `-fn names` — select fields by header name (comma-separated, matched case-insensitively against the first input row). Requires the input to have a field-name header record. See below.
- `-nc` — do not create fields that are missing in the input; the field is simply omitted rather than emitted as an empty string (default: missing fields become empty strings).

## Selecting fields by name
`-f` (and `-rf`/`-xf`) take 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row, e.g. `order -fn rank,name`. `-f`, `-rf`, `-xf` and `-fn` are mutually exclusive — exactly one must be supplied. With `-fn` the header row is itself reordered and passed through like data unless you also pass `-ifn` to drop the field-name header from the output. `order` is the only CSVfix command that accepts `-fn` directly; to use named fields with other commands, run `order` first in a pipeline to put fields into a known numeric order.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#SMQ,SEP,IBL,IFN,OFL,SKIP,PASS`: smart-quote output (`-smq`, plus `-sqf fields`), CSV separators (`-sep`, `-rsep`, `-osep`), ignore blank lines (`-ibl`), ignore field-name header (`-ifn`), output file (`-o file`) and header record (`-hdr s`), and row filtering (`-skip t` / `-pass t`).

## Examples

### Re-order and select fields by index
Input (`/tmp/names.csv`):
```
Charles,Dickens,M
Jane,Austen,F
Herman,Melville,M
```
Command — put field 3 (sex) before field 1 (forename), dropping field 2:
```
csvfix order -f 3,1 /tmp/names.csv
```
Output:
```
"M","Charles"
"F","Jane"
"M","Herman"
```

### Select fields by name, dropping the header
Input (`/tmp/army.csv`):
```
name,rank,serial
jones,sgt,1
smith,maj,2
black,maj,3
```
Command — reorder to rank,name by name and drop the header row with `-ifn`:
```
csvfix order -ifn -fn rank,name /tmp/army.csv
```
Output:
```
"sgt","jones"
"maj","smith"
"maj","black"
```

### Missing fields: default vs -nc
Using `/tmp/names.csv` (3 fields per row), request a non-existent field 5.

Default (missing field becomes empty):
```
csvfix order -f 1,2,5 /tmp/names.csv
```
Output:
```
"Charles","Dickens",""
"Jane","Austen",""
"Herman","Melville",""
```

With `-nc` (missing field omitted):
```
csvfix order -f 1,2,5 -nc /tmp/names.csv
```
Output:
```
"Charles","Dickens"
"Jane","Austen"
"Herman","Melville"
```

### Index from the end with -rf
```
csvfix order -rf 1,2 /tmp/names.csv
```
`-rf 1` is the last field, `-rf 2` is the second-to-last. Output:
```
"M","Dickens"
"F","Austen"
"M","Melville"
```

### Exclude fields with -xf
```
csvfix order -xf 2 /tmp/names.csv
```
Output (field 2 removed, others keep original order):
```
"Charles","M"
"Jane","F"
"Herman","M"
```

## Notes
- Exactly one of `-f`, `-rf`, `-xf`, `-fn` is required; supplying none or more than one is an error.
- A field listed multiple times in `-f`/`-rf` is duplicated in the output.
- Ranges (`a:b`) accepted by `-f`/`-rf` work in either direction (`1:5` or `4:1`) and are usable by other commands that take numeric field-index lists.
- Unlike `truncate` (which only removes trailing fields and never adds), `order` will append empty fields for out-of-range indexes unless `-nc` is set.
- With `-fn`, the header is consumed to resolve names; the header is then reordered and emitted as a data row unless `-ifn` is given. The HTML manual lists only `-f`, `-rf`, `-fn`, `-nc`; the built binary's help also exposes `-xf` (exclude mode), documented above.
- Available on all platforms (not an ODBC/Windows-only command).

## See also
- [pad](pad.md)
- [truncate](truncate.md)
- [exclude](exclude.md)
- [inter](inter.md)
- [printf](printf.md)
