# csvfix date_format

> perform date formatting

## Synopsis
```
csvfix date_format [options] [file ...]
```

## Description
Reformats date values that are already in ISO `YYYY-MM-DD` form into a custom
output format. It reads CSV from the named files (or standard input if none are
given), and for each selected field that contains a valid ISO date, replaces the
value with the date rendered according to the `-fmt` template; all other fields
and rows are passed through unchanged. A selected field whose value is not a
parseable, valid ISO date is left untouched. Use the sibling `date_iso` command
first to convert user-format dates into ISO before formatting them here.

## Options
- `-f fields` — comma-separated list of 1-based field indexes to format (e.g. `-f 2` or `-f 1,3`). Required unless `-fn` is given.
- `-fn names` — comma-separated list of header names (case-insensitive, matched against the first input row) selecting fields to format. Mutually exclusive with `-f`.
- `-fmt fmt` — required. Format template applied to each selected ISO date. Recognised tokens:
  - `d` — day, 1 or 2 digits (no leading zero)
  - `dd` — day, 2 digits, zero-padded
  - `m` — month number, 1 or 2 digits (no leading zero)
  - `mm` — month number, 2 digits, zero-padded
  - `mmm` — short month name (e.g. `Jan`, `Aug`)
  - `M` — full month name (e.g. `January`)
  - `w` — short weekday name (e.g. `Mon`, `Tue`)
  - `W` — full weekday name (e.g. `Monday`)
  - `y` or `yyyy` — 4-digit year (both behave identically)

  Any character that is not part of a token is emitted literally (e.g. `/`, `-`, spaces, commas). The format-significant characters are `d m y D M Y w W`; 2-digit-year output is not supported.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched
case-insensitively against the first input row (e.g. `-fn dob` or `-fn start,end`).
`-f` and `-fn` are mutually exclusive. When using `-fn`, the header row is processed
like any other row, but because header cell values are not valid ISO dates they pass
through unchanged. Add `-ifn` to drop the header row from the output entirely.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source
generic-flag token is `#SMQ,SEP,IBL,IFN,OFL,SKIP,PASS`, so it supports smart-quote
output (`-smq`, `-sqf`), CSV separators (`-sep`, `-rsep`, `-osep`), ignore blank
lines (`-ibl`), ignore field-name header (`-ifn`), output file (`-o`), a written
header record (`-hdr`), and row filtering (`-skip`, `-pass`).

## Examples

### Format a date field with weekday, day, short month, year
Input (`birthdays.csv`):
```
"Peter","2000-01-20"
"Jane","1970-01-12"
"Bill","1971-01-14"
"Anna","1976-01-27"
```
Command:
```
csvfix date_format -f 2 -fmt 'w d mmm yyyy' birthdays.csv
```
Output:
```
"Peter","Tue 20 Jan 2000"
"Jane","Sat 12 Jan 1970"
"Bill","Tue 14 Jan 1971"
"Anna","Sun 27 Jan 1976"
```

### Literal separators, zero-padding, and pass-through of non-dates
Input (`sale.csv`):
```
"a","2009-06-03"
"b","not-a-date"
```
Command:
```
csvfix date_format -f 2 -fmt 'dd/mm/yyyy' sale.csv
```
Output:
```
"a","03/06/2009"
"b","not-a-date"
```
The slashes are emitted literally and `not-a-date` is not a valid ISO date, so it is left unchanged.

### Select by header name
Input (`hdr.csv`):
```
name,dob
"Peter","2000-01-20"
```
Command:
```
csvfix date_format -fn dob -fmt 'W, d M y' hdr.csv
```
Output:
```
"name","dob"
"Peter","Monday, 20 January 2000"
```
The header row is unchanged because `dob` is not an ISO date; add `-ifn` to drop the header.

## Notes
- Input dates must already be in ISO `YYYY-MM-DD`; this command does no parsing of other formats. Use `date_iso` upstream to convert them.
- Fields whose value is not a syntactically valid and calendar-valid ISO date are passed through verbatim (no error).
- `-fmt` is required and must be non-empty; an empty format is an error. An unrecognised letter run that looks like a token but is not (e.g. a stray uppercase `D`/`Y`) can raise "Invalid date format substring".
- `y` and `yyyy` are identical (4-digit year). There is no 2-digit-year output token.
- The full-month-name (`M`) and full-weekday-name (`W`) tokens use the built-in English names; weekday names are computed from the date.
- Available on all platforms in this build.

## See also
- [date_iso](date_iso.md)
- [edit](edit.md)
- [order](order.md)
