# csvfix split_fixed

> split input using fixed sizes

## Synopsis
```
csvfix split_fixed [options] [file ...]
```

## Description
`split_fixed` splits a single field within each CSV record into several sub-fields, cutting at fixed character positions (`-p`) or by a list of fixed field lengths (`-l`). It reads CSV from the named files (or standard input if none are given), replaces the chosen field in-place with the resulting sub-fields, and writes the modified CSV to standard output (or to `-o`). By default the original field being split is discarded; use `-k` to retain it after the split fields. This is the inverse-style companion to building fixed-width data and is useful for unpacking packed columns (e.g. a combined `idsex` code into separate id and sex fields).

## Options
- `-f field` — 1-based numeric index of the field to split. Mutually exclusive with `-fn`. One of `-f`/`-fn` is required.
- `-fn name` — header name (from the first input row) of the field to split. Mutually exclusive with `-f`.
- `-p plist` — comma-separated list of `start:len` position pairs (e.g. `1:4,5:1`). `start` is 1-based; `len` is the number of characters. Each pair produces one output sub-field. Both values must be positive integers. Mutually exclusive with `-l`.
- `-l lengths` — comma-separated list of field lengths (e.g. `2,2,*`). Fields are taken sequentially from the start of the source field. Exactly one length may be `*`, meaning a variable-length field whose width is the remaining input after subtracting all fixed lengths. Lengths must be positive integers. Mutually exclusive with `-p`.
- `-k` — retain the field being split in the output, appended after the split sub-fields (default: discard it).

One of `-p` or `-l` must be supplied (they cannot be combined), and one of `-f` or `-fn` must be supplied.

## Selecting fields by name
`-f` takes a 1-based numeric field index; `-fn` takes a header name matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive. When you use `-fn`, the header row is read and treated like a data row unless you also pass `-ifn` to drop it — without `-ifn` the header itself gets split (e.g. `"code"` becomes `"code",""`), so in practice pair `-fn` with `-ifn`.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The `#ALL,SKIP,PASS` group applies: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; smart-quote `-smq` and `-sqf`; `-hdr` header record; `-ifn` ignore field-name header; and `-skip`/`-pass` row filtering.

## Examples

### Split by positions (`-p`)
Input `/tmp/emp.csv`:
```
1090M,Jeff Smith
1099F,Annette King
1170M,Bill Thompson
```
Command:
```
csvfix split_fixed -f 1 -p 1:4,5:1 /tmp/emp.csv
```
Output:
```
"1090","M","Jeff Smith"
"1099","F","Annette King"
"1170","M","Bill Thompson"
```

### Retain the source field (`-k`)
Same input as above.
```
csvfix split_fixed -f 1 -p 1:4,5:1 -k /tmp/emp.csv
```
Output (original field 1 appended after the split fields):
```
"1090","M","1090M","Jeff Smith"
"1099","F","1099F","Annette King"
"1170","M","1170M","Bill Thompson"
```

### Split by lengths with a variable-length field (`-l`)
Input `/tmp/codes.csv`:
```
AB12cd,first
XY99ef,second
```
Command (`*` absorbs the remaining characters):
```
csvfix split_fixed -f 1 -l 2,2,* /tmp/codes.csv
```
Output:
```
"AB","12","cd","first"
"XY","99","ef","second"
```

### Select the field by header name (`-fn` with `-ifn`)
Input `/tmp/named.csv`:
```
code,label
1090M,Jeff Smith
```
Command:
```
csvfix split_fixed -fn code -p 1:4,5:1 -ifn /tmp/named.csv
```
Output (header dropped by `-ifn`):
```
"1090","M","Jeff Smith"
```
Without `-ifn`, the header row is also split, giving `"code","","label"` as the first output line.

## Notes
- Positions in `-p` are 1-based start, with explicit length; the source code converts `start` to a 0-based substring offset internally. If a `start` is beyond the end of the field's value, that sub-field is emitted as empty (`""`) rather than erroring.
- With `-l`, if the fixed lengths exceed the actual field width, trailing sub-fields are taken as far as the data allows (a `*` field then becomes empty since the remaining length is clamped to 0). Only one `*` is permitted; two or more raise an error.
- `-p` and `-l` are mutually exclusive; supplying both is an error, and supplying neither is an error.
- All position and length values must be positive integers (`0` or negative is rejected). `-p` pairs must contain exactly one colon.
- The split sub-fields are inserted at the original field's position; fields before and after it are preserved in order. `-k` re-inserts the original (unsplit) value immediately after the generated sub-fields.
- Available on all platforms (built into the standard binary).
- Sibling commands sharing this code: `split_char` (split on a character or alpha/numeric transition; also supports `-fn`) and a regular-expression split variant.

## See also
- [split_char](split_char.md)
- [read_fixed](read_fixed.md)
- [write_fixed](write_fixed.md)
- [merge](merge.md)
