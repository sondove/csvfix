# csvfix trim

> trim leading/trailing spaces

## Synopsis
```
csvfix trim [options] [file ...]
```

## Description
The `trim` command removes leading and/or trailing whitespace (spaces and tabs) from input CSV fields, and can optionally truncate fields to fixed widths. It reads CSV from the named files (or standard input if none are given) and writes the transformed CSV to standard output (or to the `-o` file). By default, with no options, both leading and trailing whitespace are trimmed from every field of every row.

## Options
- `-f fields` — comma-separated 1-based field indexes to trim. If omitted, all fields are trimmed. Mutually exclusive with `-fn`.
- `-fn names` — header names to select the fields to trim (matched against the first input row). Mutually exclusive with `-f`.
- `-l` — trim leading whitespace only.
- `-t` — trim trailing whitespace only.
- `-a` — remove all whitespace from the field (everywhere, not just the ends). Cannot be combined with `-s`.
- `-s` — collapse runs of whitespace down to a single space each. Cannot be combined with `-a`.
- `-w widths` — comma-separated list of widths to truncate fields to, keeping the leftmost characters. A negative value means "do not truncate this field". If `-f`/`-fn` is also given, the widths line up with those selected fields in order; otherwise they are positional (1st width to 1st field, etc.).

Notes on flag interaction: if neither `-l` nor `-t` is given, both leading and trailing whitespace are trimmed. If either `-l` or `-t` is given, only the requested side(s) are trimmed. Whitespace trimming is always applied before `-w` width truncation.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched against the first input row. `-f` and `-fn` are mutually exclusive. With `-fn`, the header row is itself passed through and trimmed like any other row unless you also pass `-ifn` to treat the first row as a field-name record (so it is consumed for name matching rather than emitted as data).

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token `#ALL,SKIP,PASS` means the full standard set applies: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; `-smq` smart-quote and `-sqf` quote-specific-fields; `-hdr` header record; `-ifn` ignore field-name header; and `-skip`/`-pass` row filtering.

## Examples

Trim leading and trailing whitespace from all fields (default behaviour).

Input `sp.csv`:
```
"1","  leading"
"2","trailing  "
"3","  both  "
```
Command:
```
csvfix trim sp.csv
```
Output:
```
"1","leading"
"2","trailing"
"3","both"
```

Trim leading whitespace only (`-l`); trailing whitespace is preserved.
```
csvfix trim -l sp.csv
```
Output:
```
"1","leading"
"2","trailing  "
"3","both  "
```

Truncate every field to its first character with `-w`.

Input `names.csv`:
```
Christopher,David,M
Joseph,Albert,F
```
Command:
```
csvfix trim -w 1,1,1 names.csv
```
Output:
```
"C","D","M"
"J","A","F"
```

Collapse internal whitespace runs to single spaces with `-s`.

Input `multi.csv`:
```
"a   b","c  d"
```
Command:
```
csvfix trim -s multi.csv
```
Output:
```
"a b","c d"
```

## Notes
- Default (no flags) trims both leading and trailing whitespace. Whitespace means spaces and tabs.
- `-a` removes all whitespace anywhere in the field; `-s` collapses each run of whitespace to one space. They cannot be used together (doing so is an error).
- Width truncation (`-w`) always runs after whitespace trimming, and keeps the leftmost characters. A negative width leaves that field untouched.
- When `-w` is used without `-f`/`-fn`, widths are positional; supplying fewer widths than fields leaves the trailing fields untrimmed.
- Output is quoted CSV by default. Related commands: [pad](pad.md) (pad fields to a width), [edit](edit.md) (regex field editing), [truncate](truncate.md) (drop whole rows).

## See also
[pad](pad.md), [edit](edit.md), [truncate](truncate.md), [map](map.md)
