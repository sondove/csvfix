# csvfix escape

> escape special characters

## Synopsis
```
csvfix escape [options] [file ...]
```

## Description
`escape` prefixes selected characters in CSV field values with an escape string (a backslash by default), or performs SQL single-quote escaping. It reads CSV from the named files (or standard input if none are given) and writes the transformed CSV to standard output. Use it when a downstream consumer of the data has its own escaping requirements; CSVfix already handles ordinary CSV and SQL escaping internally, so this command is only for application-level escaping needs.

## Options
- `-s chars` — list of characters to be escaped. Required unless `-sql` is used. There is no default; omitting it (without `-sql`) is an error.
- `-e esc` — string placed before each escaped character (default: `\`, a single backslash). If the escape string is a single character, that character is automatically added to the set of escaped characters too, so occurrences of the escape character in the input are themselves escaped.
- `-sql` — perform SQL single-quote escaping: each single quote `'` is replaced with two single quotes `''`. Cannot be combined with `-s` or `-e`.
- `-f fields` — comma-separated 1-based field indexes to apply escaping to (default: all fields).
- `-fn names` — header names to apply escaping to (default: all fields). See below.
- `-noc` — turn off CSVfix's internal CSV escaping on output (passed as the second argument to the row writer).

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row (e.g. `escape -fn last -sql`). `-f` and `-fn` select the same target fields and should not be combined. When `-fn` is used the header row is recognised from the first input record; pass `-ifn` to ignore/drop a field-name header record.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token `#ALL,SKIP,PASS` means it accepts the full standard set: output `-o`, CSV separators `-sep`/`-rsep`/`-osep`, `-ibl` ignore blank lines, `-smq` smart-quote, `-sqf` quote-specific-fields, `-hdr` header record, `-ifn` ignore field-name header, plus `-skip`/`-pass` row filtering (rows matching `-skip` are dropped; rows matching `-pass` are emitted unchanged).

## Examples

Escape all lower-case vowels in every field:
```
Charles,Dickens,M
Flann,O'Brien,M
George,Elliot,F
```
```
csvfix escape -s "aeiou" names.csv
```
```
"Ch\arl\es","D\ick\ens","M"
"Fl\ann","O'Br\i\en","M"
"G\e\org\e","Ell\i\ot","F"
```

SQL single-quote escaping (note `O'Brien` becomes `O''Brien`):
```
Charles,Dickens,M
Flann,O'Brien,M
George,Elliot,F
```
```
csvfix escape -sql names.csv
```
```
"Charles","Dickens","M"
"Flann","O''Brien","M"
"George","Elliot","F"
```

Escape only one named field with `-fn`:
```
first,last,sex
Flann,O'Brien,M
George,Elliot,F
```
```
csvfix escape -fn last -sql idname.csv
```
```
"first","last","sex"
"Flann","O''Brien","M"
"George","Elliot","F"
```

## Notes
- With `-e` set to a single character, that escape character is added to the escaped set automatically, so it is doubled where it already appears (e.g. `-s '"' -e '\'` turns `C:\temp` into `C:\\temp` and `"` into `\"`).
- `-sql` is mutually exclusive with `-s` and `-e`; using them together is an error. `-sql` always operates on `'` only and ignores `-s`.
- Output is normally re-quoted by CSVfix's internal CSV escaping; `-noc` disables that internal escaping on the written row.
- The header row (when present) is passed through and transformed like data unless dropped with `-ifn`.
- Available on all platforms (pure CSV transform, no DB/DLL dependency).

## See also
- [edit](edit.md)
- [map](map.md)
- [trim](trim.md)
- [sql_insert](sql_insert.md)
