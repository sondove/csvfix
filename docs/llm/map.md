# csvfix map

> map betwen CSV field values

## Synopsis
```
csvfix map [options] [file ...]
```

## Description
Maps CSV field values from one literal value to another on output. For each
selected field, the value is compared against the `-fv` "from" list; on a match
the field is replaced with the value at the corresponding position in the `-tv`
"to" list. The match is a full-value literal comparison (not a regular
expression or substring). Input is read from the named files or standard input,
and every row is written to standard output with the substitutions applied
(non-matching values, and fields outside the selection, pass through unchanged).
For pattern-based replacement use [edit](edit.md) instead.

## Options
- `-f fields` — fields to map on, as a comma-separated list of 1-based numeric indexes (range syntax allowed, e.g. `1,3` or `2:4`). Default: all fields. Mutually exclusive with `-fn`; if given, the list must not be empty.
- `-fn names` — fields to map on, by header name (see below). Default: all fields. Mutually exclusive with `-f`.
- `-fv vals` — comma-separated list of values to map *from*. Required. Items are literal values, not regular expressions.
- `-tv vals` — list of values to map *to*. Required. May be empty (`-tv ''` maps matches to the empty string) but cannot contain more items than `-fv`. A "from" value is replaced by the "to" value at the same position; if `-tv` is shorter than `-fv`, any "from" value past the end of `-tv` uses the **last** "to" value (so multiple "from" values can fold to one "to" value). A "to" value of `$N` is replaced by the value of field N (1-based) of the current input record; use `$$` for a literal `$`.
- `-ic` — ignore case when comparing values to the `-fv` list (default: respect case).

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched
case-insensitively against the first input row (e.g. `-fn status` or
`-fn name,country`). `-f` and `-fn` are mutually exclusive. The header row is
itself passed through and is subject to mapping like any data row (its values
simply won't match a typical `-fv` list); pass `-ifn` to drop the field-name
header instead of emitting it.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help
token `#ALL,SKIP,PASS` means the full common set applies: output `-o`; CSV
separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; `-smq` smart-quote
and `-sqf` quote-specific-fields; `-hdr` write a header record; `-ifn` ignore
field-name header; plus `-skip t` / `-pass t` row filtering (a row matching
`-pass` is output unchanged without mapping; a row matching `-skip` is dropped).

## Examples

### Map multiple values to a single value
Input `countries.csv`:
```
"GB","United Kingdom"
"FR","France"
"DE","Germany"
"NL","Netherlands"
"IT","Italy"
"US","United States"
```
Command — map Euro-zone country codes in field 1 to `EUR`:
```
csvfix map -f 1 -fv 'FR,NL,IT,DE' -tv 'EUR' countries.csv
```
Output:
```
"GB","United Kingdom"
"EUR","France"
"EUR","Germany"
"EUR","Netherlands"
"EUR","Italy"
"US","United States"
```

### Select a field by name and fold several values
Input `status.csv`:
```
id,status
1,Y
2,N
3,yes
4,maybe
```
Command — normalise the `status` column (`Y`/`yes` to `true`, `N` to `false`):
```
csvfix map -fn status -fv 'Y,yes,N' -tv 'true,true,false' status.csv
```
Output (the header row passes through; `maybe` has no match so is unchanged):
```
"id","status"
"1","true"
"2","false"
"3","true"
"4","maybe"
```

## Notes
- `-fv` and `-tv` are both required. `-tv` may be empty but may never be longer than `-fv` (that is an error: "List of 'to values' longer than list of 'from values").
- Shorter-`-tv` fallback: when `-tv` has fewer items than `-fv`, every "from" item beyond the length of `-tv` maps to the last `-tv` value. With a single `-tv` value this folds all `-fv` matches to that one value.
- Empty `-tv` (`-tv ''`) blanks out any matched value.
- `$N` in a `-tv` value copies field N (1-based) of the current record into the matched field; `$$` yields a literal `$` (e.g. `-tv '$$1.00'` produces `$1.00`). An out-of-range `$N` yields the empty string; a non-integer after `$` is an error.
- Matching is whole-value: a field equals a `-fv` item or it does not; no substring or regex matching. Use `-ic` for case-insensitive matching.
- Only the listed fields are considered; with no `-f`/`-fn` every field in the row is mapped.
- Available on all platforms (no external dependencies).

## See also
- [edit](edit.md)
- [eval](eval.md)
- [merge](merge.md)
- [order](order.md)
