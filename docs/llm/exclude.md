# csvfix exclude

> exclude fields from output

## Synopsis
```
csvfix exclude [options] [file ...]
```

## Description
Removes (excludes) selected fields from each CSV input record and writes the remaining fields. Reads CSV from the named files or from standard input, and writes the surviving fields to standard output (or to a file with `-o`). Unlike `order`, `exclude` is convenient when records have a variable number of fields, or when there are many fields and you only want to drop a few. Fields can be selected by numeric index (`-f`), by header name (`-fn`), or counted from the end of each record (`-rf`), and exclusion can be made conditional with an expression (`-if`).

## Options
- `-f fields` — comma-separated list of 1-based field indexes to exclude, counted from the start of the record. Supports ranges, e.g. `-f 3:6` is equivalent to `-f 3,4,5,6`.
- `-fn names` — comma-separated list of header names to exclude, matched against the first input row (see below). Mutually exclusive with `-f` and `-rf`.
- `-rf fields` — like `-f`, but field indexes are counted from the end of the record (1 = last field). Only numeric indexes are accepted. Cannot be combined with `-f` or `-fn`.
- `-if expr` — evaluate `expr` using the eval expression language; the fields named by `-f`/`-fn`/`-rf` are excluded only for rows where `expr` is true. Rows where `expr` is false are passed through unchanged.

One of `-f`, `-fn`, or `-rf` must be specified and must be non-empty.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive (and neither may be combined with `-rf`). By default the header row is transformed like any data row — the named fields are removed from it too. Pass `-ifn` to drop the field-name header from the output entirely.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token `#ALL,SKIP,PASS` means the full ALL group applies: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; `-smq` smart-quote and `-sqf` quote-specific-fields; `-hdr` header record; `-ifn` ignore field-name header; plus the `-skip`/`-pass` row-filtering options.

## Examples

Input `/tmp/people.csv`:
```
"Charles","Dickens","M"
"Jane","Austen","F"
"Herman","Melville","M"
```

Exclude field 2 (the surname) by index:
```
csvfix exclude -f 2 /tmp/people.csv
```
```
"Charles","M"
"Jane","F"
"Herman","M"
```

Exclude the last field using `-rf`:
```
csvfix exclude -rf 1 /tmp/people.csv
```
```
"Charles","Dickens"
"Jane","Austen"
"Herman","Melville"
```

Exclude by header name. Input `/tmp/hdr.csv`:
```
first,last,sex
"Charles","Dickens","M"
"Jane","Austen","F"
```
```
csvfix exclude -fn last /tmp/hdr.csv
```
```
"first","sex"
"Charles","M"
"Jane","F"
```
Note the header row is transformed too. Add `-ifn` to drop it:
```
csvfix exclude -fn last -ifn /tmp/hdr.csv
```
```
"Charles","M"
"Jane","F"
```

Conditional exclusion with `-if`. Input `/tmp/amounts.csv`:
```
"a","100","x"
"b","5","y"
```
Drop field 3 only when field 2 is greater than 10:
```
csvfix exclude -f 3 -if '$2 > 10' /tmp/amounts.csv
```
```
"a","100"
"b","5","y"
```

## Notes
- Exactly one selection mode is required: `-f`, `-fn`, or `-rf`. An empty field list is an error.
- `-rf` only accepts numeric indexes (names make no sense counted from the end) and cannot be combined with `-f` or `-fn`.
- Field indexes are 1-based. Ranges (`a:b`) are supported in `-f`/`-rf`.
- With `-if`, only the listed fields are removed and only for rows where the expression is true; non-matching rows are emitted unchanged, so output rows may have differing field counts.
- `-fn` requires a header row in the input; without `-ifn` that header row is itself processed (its named fields removed) and emitted.
- Related siblings: use `order` to reorder/select/duplicate fields, and `find`/`remove` plus `join -inv` to exclude whole rows rather than fields.

## See also
[order](order.md)
