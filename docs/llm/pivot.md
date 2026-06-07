# csvfix pivot

> simple pivot tables

## Synopsis
```
csvfix pivot [options] [file ...]
```

## Description
Performs simple pivot-table operations on CSV input. You pick one field for the
column headers, one field for the row headers, and one "fact" field to
aggregate; `pivot` then builds a grid whose cells contain the aggregated fact
values for each (row, column) combination. Input is read from the named files
(or standard input). Output is one CSV row per distinct row-header value, with a
leading header row listing the distinct column-header values. The first output
field of the header row is always empty (it sits above the row-label column).
Rows and columns are emitted in sorted order.

## Options
All four of these options are **required**:

- `-c field` — field to use for column headers. `field` is a 1-based numeric index or a header name (matched case-insensitively against the first input row). (no default)
- `-r field` — field to use for row headers. Same syntax as `-c`. Must differ from `-c`. (no default)
- `-a act` — aggregation action: one of `sum`, `avg`, `count`. (no default)
- `-f field` — fact field to aggregate. Same syntax as `-c`/`-r`. (no default)

For `sum` and `avg` the fact field must be numeric (a non-numeric value, e.g. a
leftover header label, raises `ERROR: Non-numeric fact: ...`). For `count` the
fact field's value is ignored — each matching row counts as 1.

## Common options
Also accepts the standard options — see [conventions](conventions.md).
The help token is `#ALL`, so all standard groups apply: output `-o`; CSV
separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; `-smq` smart-quote
and `-sqf` quote-specific-fields; `-hdr` write a header record; `-ifn` ignore
(drop) the field-name header row. There is **no** `-skip`/`-pass` support for
this command.

## Examples

### Sum of a fact field, fields chosen by header name
Input (`sales.csv`):
```
region,product,amount
North,Widget,100
North,Gadget,50
South,Widget,200
South,Gadget,75
North,Widget,30
```
Command (use `-ifn` to drop the header row so it is not pivoted as data):
```
csvfix pivot -ifn -c region -r product -a sum -f amount sales.csv
```
Output:
```
"","North","South"
"Gadget","50","75"
"Widget","130","200"
```

### Count by numeric field indexes
Same input as above. Columns/rows/fact selected by 1-based index
(`region`=1, `product`=2, `amount`=3); `count` ignores the fact value:
```
csvfix pivot -ifn -c 1 -r 2 -a count -f 3 sales.csv
```
Output:
```
"","North","South"
"Gadget","1","1"
"Widget","2","1"
```

### Average
```
csvfix pivot -ifn -c region -r product -a avg -f amount sales.csv
```
Output:
```
"","North","South"
"Gadget","50","75"
"Widget","65","200"
```
(North/Widget = (100+30)/2 = 65.)

## Notes
- All four of `-c`, `-r`, `-a`, `-f` are mandatory; omitting any is an error.
- `-c` and `-r` cannot resolve to the same field (`ERROR: Row and column options cannot have the same value`).
- Field selection: a numeric value is a 1-based index (must be > 0); any
  non-numeric value is treated as a header name resolved case-insensitively from
  the first input row. This per-option auto-detection replaces a separate `-fn`
  flag — there is no `-fn` for `pivot`.
- The header row is NOT auto-detected as a header. When you select fields by
  name, the header row is still read as data and will appear in the output (and
  break `sum`/`avg` with a non-numeric-fact error). Pass `-ifn` to drop it.
- Empty (missing) row/column combinations are output as `0` (the cell defaults
  to a zero sum/count).
- For `avg`, the cell value is `sum / count` over the matching rows.
- Rows and columns appear in sorted (lexicographic) order, not input order.

## See also
- [summary](summary.md)
- [order](order.md)
