# csvfix split_char

> split using regular expression

## Synopsis
```
csvfix split_char [options] [file ...]
```

## Description
`split_char` splits one field of each input CSV record into several sub-fields, either at occurrences of a separator character (or character sequence), or at the first transition between alphabetic and numeric characters. Input is read from the named files or from standard input. Each output record has the chosen field replaced, in place, by the resulting sub-fields; the rest of the record is passed through unchanged and the result is written as CSV to standard output (or to `-o`).

## Options
- `-f field` — 1-based numeric index of the field to split. Mutually exclusive with `-fn`. One of `-f`/`-fn` is required.
- `-fn name` — header name of the field to split (matched against the first input row). Mutually exclusive with `-f`.
- `-c char` — character(s) to split at (default: a single space). If more than one character is given, the whole string is treated as a single separator. Mutually exclusive with `-tan`/`-tna`.
- `-tan` — split at the first transition from an alphabetic character to a digit (produces exactly two sub-fields). Mutually exclusive with `-c`, `-tna`.
- `-tna` — split at the first transition from a digit to an alphabetic character (produces exactly two sub-fields). Mutually exclusive with `-c`, `-tan`.
- `-k` — retain (keep) the field being split; it is appended as the last field of the output record. Default: the original field is discarded (replaced by the sub-fields).

## Selecting fields by name
`-f` takes a 1-based numeric field index; `-fn` takes a header name matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive. With `-fn`, the header row itself is passed through and transformed like any other data row (its named field is also split) unless you add `-ifn` to drop the header record from the output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#ALL,SKIP,PASS`, so this command honours: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; smart-quote `-smq` and `-sqf`; header record `-hdr`; ignore-field-name header `-ifn`; and row filtering `-skip`/`-pass`.

## Examples

Split a field on its default separator (space):

Input (`emp.csv`):
```
"1090M Jeff Smith"
"1099F Annette King"
```
Command:
```
csvfix split_char -f 1 emp.csv
```
Output:
```
"1090M","Jeff","Smith"
"1099F","Annette","King"
```

Split at the first digit-to-alpha transition with `-tna`:

Input (`idname.csv`):
```
1234fred,m
22bill,m
171171lynn,f
```
Command:
```
csvfix split_char -f 1 -tna idname.csv
```
Output:
```
"1234","fred","m"
"22","bill","m"
"171171","lynn","f"
```

Select the field by header name and split on a literal `-`, dropping the header with `-ifn`:

Input (`named.csv`):
```
name,combo
bob,a-b-c
```
Command:
```
csvfix split_char -fn combo -c - -ifn named.csv
```
Output:
```
"bob","a","b","c"
```

## Notes
- The built binary's `help split_char` text labels the transition flags `-tcn`/`-tnc`; those names are wrong and rejected with `ERROR: Invalid option`. The real flags are `-tan` (alpha-to-number) and `-tna` (number-to-alpha), matching the manual and the source (`FLAG_TRANA2N = "-tan"`, `FLAG_TRANN2A = "-tna"`).
- Transition splits always yield at most two sub-fields, cutting at the *first* matching boundary only. If no such transition exists, the field is left unchanged.
- `-c` and the transition flags are mutually exclusive (`ERROR: Cannot specify both character and transiton`), and `-tan`/`-tna` cannot be combined with each other.
- With `-k`, the original (unsplit) field value is appended at the end of the record, not left in its original position.
- If the chosen field index is past the end of a row, an empty string is split (yielding empty sub-fields).
- Sibling commands: [split_fixed](split_fixed.md) splits a field at fixed character positions/lengths; [merge](merge.md) joins fields back together.

## See also
- [split_fixed](split_fixed.md)
- [merge](merge.md)
- [order](order.md)
- [conventions](conventions.md)
