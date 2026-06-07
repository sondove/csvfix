# csvfix upper

> convert to uppercase

## Synopsis
```
csvfix upper [options] [file ...]
```

## Description
Reads CSV input (from the named files, or from standard input if none are given) and converts alphabetic characters in selected fields to upper-case, writing the transformed CSV to standard output. By default every field of every row is converted. Use `-f` (numeric indexes) or `-fn` (header names) to restrict the conversion to specific fields; non-selected fields pass through unchanged. Only alphabetic characters are affected; digits, punctuation and quoting are preserved.

## Options
- `-f fields` — comma-separated list of 1-based field indexes to convert (default: all fields). Mutually exclusive with `-fn`.
- `-fn names` — comma-separated list of header names (matched case-insensitively against the first input row) identifying the fields to convert (default: all fields). Mutually exclusive with `-f`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive. When you use `-fn`, the header row is itself passed through and transformed like any other row (so a named field's header text is also upper-cased) unless you also pass `-ifn` to drop the header record from the output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Applicable here: output `-o file`, CSV separator `-sep sep`, ignore blank input lines `-ibl`, smart-quote output `-smq`, and ignore the field-name header record `-ifn` (used with `-fn`).

## Examples

Convert every field of every row (default):

Input (`names.csv`):
```
"Charles","Dickens","M"
"Jane","Austen","F"
"Flann","O'Brien","M"
```
Command:
```
csvfix upper names.csv
```
Output:
```
"CHARLES","DICKENS","M"
"JANE","AUSTEN","F"
"FLANN","O'BRIEN","M"
```

Convert only the second field with `-f 2`:
```
csvfix upper -f 2 names.csv
```
Output:
```
"Charles","DICKENS","M"
"Jane","AUSTEN","F"
"Flann","O'BRIEN","M"
```

Convert a field by header name with `-fn`, dropping the header with `-ifn`:

Input (`people.csv`):
```
first,last,sex
Charles,Dickens,m
Jane,Austen,f
```
Command:
```
csvfix upper -fn last -ifn people.csv
```
Output:
```
"Charles","DICKENS","m"
"Jane","AUSTEN","f"
```
Without `-ifn`, the header row is kept and the matched column's header is also upper-cased, producing `"first","LAST","sex"` as the first output line.

## Notes
- Default scope is ALL fields; supply `-f` or `-fn` to narrow it.
- `-f` and `-fn` cannot be combined.
- With `-fn` the header row is transformed/passed through unless `-ifn` is given; with `-f` there is no special header handling (row 1 is treated as data).
- Field selections that exceed the row width are simply ignored for rows that lack them.
- Sibling case commands: `lower` (to lower-case) and `mixed` (capitalise the initial letter of each word). The expression language also provides an `upper(str)` function.

## See also
- [lower](lower.md)
- [mixed](mixed.md)
- [order](order.md)
