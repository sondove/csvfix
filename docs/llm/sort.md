# csvfix sort

> sort CSV input on one or more fields

## Synopsis
```
csvfix sort [options] [file ...]
```

## Description
Reads CSV from the named files (or standard input if none are given) and writes the records to standard output sorted on one or more fields. Each sort field can be sorted alphabetically (optionally ignoring case) or numerically, in ascending or descending order. All input is read into memory before sorting, so the command can be slow or unusable on very large datasets. If no sort field is specified, the data is sorted alphabetically ascending on the first field.

## Options
- `-f fields` — comma-separated list of 1-based numeric field indexes to sort on, in priority order (default: `1`). Each entry is an index with an optional `:` followed by up to two flags. Mutually exclusive with `-fn`.
- `-fn fields` — comma-separated list of header names to sort on, matched case-insensitively against the first input row, in priority order. Each entry is a name with an optional `:` followed by up to two flags. Mutually exclusive with `-f`. No numeric default — a name must be given.
- `-rh` — retain header: treat the first input record as a header, do not sort it, and place it back as the first output record.

Field-spec flags (after the `:`): one direction flag and/or one comparison flag, in any order, e.g. `AN`, `DS`, `ID`.
- `A` — ascending (default)
- `D` — descending
- `S` — string/alphabetic comparison (default)
- `I` — string comparison ignoring case
- `N` — numeric comparison (all values in the field must be valid numbers)

Examples of valid specs: `1:AN`, `2:DS`, `3:D`, `price:AN`, `date:DS`. Invalid combinations (e.g. two direction flags, or more than two chars) are rejected.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive. For `sort`, each named field carries its own sort flags after a colon, e.g. `-fn price:AN,date:DS`. When using `-fn`, the first row is consumed as the name header; combine with `-rh` to re-emit that header at the top of the sorted output (otherwise the header row is dropped from output).

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#SMQ,SEP,IBL,IFN,OFL`: output `-o file`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank input lines; `-ifn` ignore field-name header record; `-smq` smart-quote output; `-sqf fields` force-quote fields; `-hdr s` write string `s` as a header record.

## Examples

Default sort (alphabetic ascending on the first field):

Input `/tmp/s1.csv`:
```
banana,3
apple,10
cherry,2
```
Command:
```
csvfix sort /tmp/s1.csv
```
Output:
```
"apple","10"
"banana","3"
"cherry","2"
```

Numeric ascending on the second field (note `2` sorts before `10`, unlike a string sort):
```
csvfix sort -f 2:AN /tmp/s1.csv
```
Output:
```
"cherry","2"
"banana","3"
"apple","10"
```

Sort by header name, numeric ascending on `price`, keeping the header:

Input `/tmp/s2.csv`:
```
name,price
Banana,3
apple,10
Cherry,2
```
Command:
```
csvfix sort -fn price:AN -rh /tmp/s2.csv
```
Output:
```
name,price
"Cherry","2"
"Banana","3"
"apple","10"
```

Case-insensitive sort by name (`I` flag) keeps `apple`, `Banana`, `Cherry` in alphabetical order regardless of case:
```
csvfix sort -fn name:AI -rh /tmp/s2.csv
```
Output:
```
name,price
"apple","10"
"Banana","3"
"Cherry","2"
```

## Notes
- Default comparison is alphabetic ascending; `S` (string, case-sensitive) is the default comparison type, so uppercase letters sort before lowercase. Use `I` for case-insensitive comparison.
- Multiple fields are applied in listed order: `-f 3:D,2` sorts primarily on field 3 descending, then field 2 ascending alphabetic.
- Numeric (`N`) sort requires every value in that field to be a valid number; non-numeric data will cause an error or misordering.
- Indexes are 1-based and must be non-zero integers; `0` or a non-integer in `-f` is rejected (use `-fn` for header names).
- `-f` and `-fn` cannot be combined, and each may appear only once.
- Without `-rh`, any header row is treated as ordinary data and sorted in with the rest. The `-rh` flag (and the `-fn` name resolution) consumes the first input record as the header.
- The header re-emitted by `-rh` is written using a simple comma join and is not re-quoted, while sorted data rows are written through the normal CSV output (and are quoted).
- The entire input is buffered in memory before sorting.

## See also
- [shuffle](shuffle.md)
- [unique](unique.md)
- [order](order.md)
