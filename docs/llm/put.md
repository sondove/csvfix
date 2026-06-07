# csvfix put

> put literal or env variable into CSV output

## Synopsis
```
csvfix put [options] [file ...]
```

## Description
`put` reads CSV from the named files (or standard input) and writes each
record back out with a single extra field added. The new field holds either a
literal string (`-v`) or the value of an environment variable (`-e`). By
default the field is appended at the end of each record; `-p` places it at a
specific 1-based position. Useful for tagging output, stamping rows with a
date, recording the field count, or filling in a missing constant column.

## Options
- `-v val` — the literal value to put into every record. Mutually exclusive with `-e`; exactly one of `-v`/`-e` is required.
- `-e env` — name of an environment variable whose value is put into every record. Mutually exclusive with `-v`. Three special tokens are recognised instead of an env var name:
  - `@DATETIME` — current timestamp as `yyyy-mm-dd hh:mm:ss`.
  - `@DATE` — current date as `yyyy-mm-dd`.
  - `@COUNT` — the number of fields in each input record (computed per row, before the new field is added).
- `-p pos` — 1-based field position at which to insert the value (default: append at end). If `pos` is greater than the number of fields in a record, the value is appended at the end of that record instead. `pos` must be a positive integer; a non-integer or zero/negative value is an error.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The
help token is `#ALL`, so the full standard set applies: output `-o`; CSV
separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank input lines; `-smq`
smart-quote and `-sqf` quote-specific-fields on output; `-hdr` write a header
record; `-ifn` ignore field-name header; plus `-skip`/`-pass` row filtering.

## Examples

Input (`in.csv`):
```
a,b,c
1,2,3
```

Append a literal at the end of every record:
```
csvfix put -v HELLO in.csv
```
```
"a","b","c","HELLO"
"1","2","3","HELLO"
```

Insert a literal at position 2 (shifting later fields right):
```
csvfix put -p 2 -v X in.csv
```
```
"a","X","b","c"
"1","X","2","3"
```

Put the value of an environment variable:
```
MYVAR=somevalue csvfix put -e MYVAR in.csv
```
```
"a","b","c","somevalue"
"1","2","3","somevalue"
```

Append the per-record field count with `@COUNT`:
```
csvfix put -e @COUNT in.csv
```
```
"a","b","c","3"
"1","2","3","3"
```

Append today's date with `@DATE` (output shown for 2026-06-07):
```
csvfix put -e @DATE in.csv
```
```
"a","b","c","2026-06-07"
"1","2","3","2026-06-07"
```

## Notes
- Exactly one of `-v` or `-e` must be supplied; supplying both, or neither, is an error.
- `put` adds exactly one field per record. To add several constants, chain multiple `put` commands (or pipe through `csvfix put` repeatedly).
- `-p` uses 1-based positions. Position 1 makes the new field the first field. A position beyond the current record length appends rather than padding with empty fields — use [pad](pad.md) if you need empty placeholder fields.
- `@COUNT` reflects each input record's own field count, so rows with differing field counts get differing values.
- For more control over the timestamp format than `@DATETIME`/`@DATE` provide, use the timestamp command.
- The header row (if any) is treated like any other data record and has the value added too, unless you drop it with `-ifn`.

## See also
[pad](pad.md), [sequence](sequence.md), [eval](eval.md), [order](order.md)
