# csvfix mixed

> convert to mixed case

## Synopsis
```
csvfix mixed [options] [file ...]
```

## Description
Reads CSV input from the named files (or standard input if none are given) and converts alphabetic data to mixed (capitalised) case, where the first letter of each whitespace-delimited word is upper-cased and all remaining letters are lower-cased. By default every field of every row is transformed; use `-f` or `-fn` to restrict conversion to specific fields. The result is written as CSV to standard output (or to the file given by `-o`).

## Options
- `-f fields` — comma-separated list of 1-based numeric field indexes to convert (default: all fields). Mutually exclusive with `-fn`.
- `-fn names` — comma-separated list of header names (matched against the first input row) identifying the fields to convert (default: all fields). Mutually exclusive with `-f`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched against the first input row. `-f` and `-fn` are mutually exclusive. The header row is passed through and itself case-converted like any other data row unless you also pass `-ifn`, which makes the first row be treated purely as the name source and excluded from transformation (it is still emitted unchanged). Example: `csvfix mixed -fn name -ifn data.csv`.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Applicable groups: output `-o`; CSV separator `-sep`; ignore blank input lines `-ibl`; smart-quote output `-smq`; ignore field-name header `-ifn`.

## Examples

### Convert all fields
Input (`/tmp/mx1.csv`):
```
john smith,NEW YORK
MARY-JANE o'brien,los angeles
```
Command:
```
csvfix mixed /tmp/mx1.csv
```
Output:
```
"John Smith","New York"
"Mary-jane O'brien","Los Angeles"
```
Note that only whitespace marks a word boundary: the letter after the `-` in `Mary-jane` and after the `'` in `O'brien` is not capitalised.

### Convert one field by name, keeping the header intact
Input (`/tmp/mx2.csv`):
```
name,city
john smith,new york
MARY jones,SAN diego
```
Command:
```
csvfix mixed -fn name -ifn /tmp/mx2.csv
```
Output:
```
"John Smith","new york"
"Mary Jones","SAN diego"
```
The `name` column is converted; `city` is left untouched, and the `name,city` header is emitted unchanged because `-ifn` excludes it from transformation.

## Notes
- Word-boundary detection uses C `isspace`; only spaces, tabs and similar whitespace separate words. Punctuation such as `-`, `'`, `.` and `/` does not start a new word, so `o'brien` becomes `O'brien`, not `O'Brien`.
- Each selected field is first fully lower-cased, then the first alphabetic character of each word is upper-cased; existing internal capitals are therefore flattened (e.g. `MARY-JANE` → `Mary-jane`, `NEW YORK` → `New York`).
- Without `-ifn`, the first row is treated as data and is itself case-converted; with `-ifn` the first row is read as the header (for `-fn` matching) and passed through unchanged.
- Non-alphabetic fields (numbers, symbols) are emitted unchanged apart from any case folding of letters they contain.
- Sibling commands `upper` and `lower` perform unconditional upper-/lower-casing and share the same option set.

## See also
- [upper](upper.md)
- [lower](lower.md)
- [trim](trim.md)
- [edit](edit.md)
