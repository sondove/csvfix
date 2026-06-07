# csvfix date_iso

> convert dates to ISO format

## Synopsis
```
csvfix date_iso [options] [file ...]
```

## Description
Reads CSV input (from the named files or standard input) and converts the dates in the selected fields to ISO `YYYY-MM-DD` format, writing the modified CSV to standard output. The input date layout is described with a 5-character mask given via `-m` (which is required). A field whose value cannot be parsed as a valid date under the mask is left unchanged. This is the companion of [date_format](date_format.md): convert raw dates to ISO with `date_iso`, then reformat them for display with `date_format`.

## Options
- `-f fields` — fields to convert, given as a comma-separated list of 1-based numeric indexes (e.g. `-f 2` or `-f 1,3`). Mutually exclusive with `-fn`.
- `-fn names` — fields to convert, given by header name matched against the first input row (see below). Mutually exclusive with `-f`.
- `-m mask` — REQUIRED. Specifies the order of date components and their separators. The mask must be exactly 5 characters: positions 1, 3, 5 are one each of `d`, `m`, `y` (in any order, giving the day/month/year order of the input); positions 2 and 4 are the two separator characters, which must be non-alphanumeric. Examples: `d/m/y` reads `19/Aug/2005`; `m d,y` reads `Aug 20,2000`; `d m y` reads `4 Jul 2002`. An invalid mask (wrong length, missing a dmy letter, or alphanumeric separator) is an error.
- `-cy year` — base year for interpreting 2-digit year values (default: `1930`). A 2-digit year less than `base - 1900` is taken as `20xx`, otherwise `19xx`. With the default 1930: `01`→2001, `29`→2029, `30`→1930, `36`→1936.
- `-mn names` — comma-separated list of month names used to parse textual months; must be exactly 12 names in month order (default: English month names `January,February,...,December`). Matching is by case-insensitive prefix of at least 3 characters.
- `-bdl` — output only the records that FAIL date conversion (list bad records). Mutually exclusive with `-bdx`. (Note: the binary's help text mislabels this flag as `-bxl`; the accepted flag is `-bdl`.)
- `-bdx` — silently exclude records that fail date conversion from the output. Mutually exclusive with `-bdl`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row (e.g. `-fn dob` or `-fn start,end`). `-f` and `-fn` are mutually exclusive. The header row is itself passed through the conversion like a data row (the header values normally won't parse as dates, so they pass through unchanged) unless you also pass `-ifn`, which drops the header record from the output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#SMQ,SEP,IBL,IFN,OFL,SKIP,PASS`: smart-quote output `-smq` (and `-sqf fields`), CSV separators `-sep`/`-rsep`/`-osep`, ignore blank lines `-ibl`, ignore field-name header `-ifn`, output file `-o file`, header record `-hdr s`, and row filtering `-skip t`/`-pass t`.

## Examples

### 1. Convert a date column to ISO
Input (`bdays.csv`):
```
Peter,20/8/00
Jane,12/2/70
Bill,14/6/71
Anna,27/12/76
```
Command:
```
csvfix date_iso -f 2 -m 'd/m/y' bdays.csv
```
Output:
```
"Peter","2000-08-20"
"Jane","1970-02-12"
"Bill","1971-06-14"
"Anna","1976-12-27"
```

### 2. Select by name, drop the header, and reject invalid dates
Input (`named.csv`):
```
name,dob
Peter,20/8/2000
Jane,12-2-1970
```
Command (the second row uses `-` separators, which do not match the `d/m/y` mask, so it is left unchanged):
```
csvfix date_iso -fn dob -m 'd/m/y' -ifn named.csv
```
Output:
```
"Peter","2000-08-20"
"Jane","12-2-1970"
```

### 3. List only records that fail conversion
Input (`mix.csv`):
```
Peter,20/8/00
Bad,99/99/99
```
Command:
```
csvfix date_iso -f 2 -m 'd/m/y' -bdl mix.csv
```
Output:
```
"Bad","99/99/99"
```
With `-bdx` instead of `-bdl`, the same input produces only `"Peter","2000-08-20"`.

## Notes
- `-m` is mandatory; running without it produces a mask error.
- A field that does not parse as a valid date under the mask is passed through unchanged (it is not blanked or removed). Use `-bdl`/`-bdx` to instead select or drop the whole record based on conversion failure.
- The mask separators are matched literally in the input: `find` locates the first separator and `find_last_of` the second, so the day part runs up to the first separator and the year part starts after the last separator character.
- 2-digit year handling depends on `-cy` (default base 1930) as described above; 4-digit years are used as-is. Year strings that are not 2 or 4 digits long are not parsed.
- Textual months are matched by case-insensitive prefix (minimum 3 characters) against the `-mn` list (English by default); supply `-mn` for other languages (exactly 12 comma-separated names).
- Output is quoted CSV. ISO output here is plain `YYYY-MM-DD` with no time component.
- Availability: present in this build (standard, cross-platform command).
- Sibling command: [date_format](date_format.md) reformats ISO dates into arbitrary display formats.

## See also
- [date_format](date_format.md)
- [conventions](conventions.md)
