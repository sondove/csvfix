# csvfix timestamp

> add timestamp to CSV data

## Synopsis
```
csvfix timestamp [options] [file ...]
```

## Description
Reads CSV from the named files (or standard input) and writes each input row to output with a single new timestamp field prepended as the first column. By default the stamp uses the format `YYYY-MM-DD HH:MM:SS` reflecting the local time when the command runs. By default a single stamp is captured once at startup and applied to every row; pass `-rt` to re-read the clock for each row. The original fields are passed through unchanged after the new field.

## Options
- `-d` — display only the date part of the timestamp (`YYYY-MM-DD`). Mutually exclusive with `-t`.
- `-t` — display only the time part of the timestamp (`HH:MM:SS`). Mutually exclusive with `-d`.
- `-n` — display the timestamp as a numeric value with no date/time separators (e.g. `20260607183420`). Combines with `-d`/`-t`.
- `-rt` — update the stamp in real time as records are written. By default the same stamp is used for all rows.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The generic flag token is `#ALL,SKIP`, so it supports the full common-option set: output `-o`, CSV separators `-sep`/`-rsep`/`-osep`, `-ibl` ignore blank lines, `-smq` smart-quote, `-sqf` quote-specific-fields, `-ifn` ignore field-name header, `-hdr` write a header record, and `-skip` row filtering. (Note: there is no `-pass`; only `-skip` is in this group.)

## Examples

Add a default timestamp to each row (stamp value reflects the run time):
```
"Charles","Dickens","M"
"Jane","Austen","F"
"Herman","Melville","M"
```
```
csvfix timestamp names.csv
```
```
"2026-06-07 18:34:20","Charles","Dickens","M"
"2026-06-07 18:34:20","Jane","Austen","F"
"2026-06-07 18:34:20","Herman","Melville","M"
```

Date-only, numeric (no separators) using `-d -n`:
```
"Charles","Dickens","M"
"Jane","Austen","F"
"Herman","Melville","M"
```
```
csvfix timestamp -d -n names.csv
```
```
"20260607","Charles","Dickens","M"
"20260607","Jane","Austen","F"
"20260607","Herman","Melville","M"
```

Time-only (`-t`) produces `"18:34:20",...`; `-n` alone produces a full numeric stamp like `"20260607183420",...`.

## Notes
- The timestamp field is always prepended as the new first column; it is never appended.
- `-d` and `-t` cannot be used together (the command errors). Using neither gives the full date-and-time stamp.
- Without `-rt`, the clock is sampled once before any rows are read, so all rows share an identical stamp. With `-rt` the stamp is refreshed per row, so values can differ on slow/large inputs.
- Time is local time (via `localtime`), not UTC.
- Numeric (`-n`) format simply strips the `-`, `:`, and space separators; the digit order is unchanged.
- For more flexible/custom date formatting of existing fields rather than stamping a new field, see the `date_format` and `date_iso` commands. To insert arbitrary literal or environment-variable values, see `put`.

## See also
- [put](put.md)
- [sequence](sequence.md)
- [order](order.md)
