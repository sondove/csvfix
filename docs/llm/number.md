# csvfix number

> convert formatted numeric fields to ordinary numeric

## Synopsis
```
csvfix number [options] [file ...]
```

## Description
Converts formatted numeric values containing thousands/decimal separators (e.g. `1,234.56`) into plain numbers on which arithmetic can be performed (e.g. `1234.56`). It reads CSV from the named files (or standard input if none are given) and writes the transformed CSV to standard output. The thousands separator is stripped and the decimal separator is normalised to `.`, according to the chosen input format (`EN` or `EU`). Only the selected fields are converted; if no fields are selected, conversion is attempted on every field. A field that does not parse as a number is, by default, passed through unchanged.

## Options
- `-f fields` — comma-separated list of 1-based field indexes to convert. If omitted, all fields are converted.
- `-fn names` — fields to convert, selected by header name (matched against the first input row). Mutually exclusive with `-f`.
- `-fmt fmt` — input number format: `EN` (US/UK English: `,` thousands, `.` decimal) or `EU` (European: `.` thousands, `,` decimal). Default: `EN`.
- `-es str` — if a field cannot be parsed as a number, output `str` in its place instead of the original value. Cannot be combined with `-ec`.
- `-ec` — treat a conversion failure as an error: print an error and exit with a non-zero status. Cannot be combined with `-es`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched against the first input row. `-f` and `-fn` are mutually exclusive. When using `-fn`, the header row is itself emitted as data (its name cells won't parse as numbers, so they pass through) unless you also pass `-ifn` to drop the field-name header from the output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token is `#ALL`, so all standard groups apply: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank input lines; `-smq` smart-quote and `-sqf` quote-specified-fields; `-hdr` write a header record; `-ifn` ignore the field-name header; and `-skip`/`-pass` row filtering (rows matching `-pass` are emitted unchanged, without conversion).

## Examples

Convert one field by index (`EN` is the default format):

```
item,price
widget,"1,234.56"
gadget,"2,000"
```
```
csvfix number -f 2 input.csv
```
```
"item","price"
"widget","1234.56"
"gadget","2000"
```

Select the field by name and drop the header row with `-ifn`:

```
csvfix number -fn price -ifn input.csv
```
```
"widget","1234.56"
"gadget","2000"
```

European format with an error-substitution string. Input:

```
"1.234,56","99,5"
"bad","3.000.000,00"
```
```
csvfix number -fmt EU -es N/A input.csv
```
```
"1234.56","99.5"
"N/A","3000000.00"
```

The same data with `-ec` aborts on the unparseable field:

```
csvfix number -fmt EU -ec input.csv
```
```
"1234.56","99.5"
ERROR: Invalid number: bad
```
(exit status 1; the second row is never written)

## Notes
- Default format is `EN`; `-fmt` only accepts `EN` or `EU` (any other value is an error).
- With no `-f`/`-fn`, every field is processed. Cells that fail to parse are left as their original value by default; `-es` overrides the output value, `-ec` aborts.
- `-es` and `-ec` are mutually exclusive; specifying both is an error.
- Output values are produced by stripping the thousands separator and converting the decimal separator to `.`. No padding, rounding, or fixed decimal places are applied; the digits present in the input are preserved (e.g. `3.000.000,00` EU -> `3000000.00`).
- Characters after the decimal point are kept verbatim; a thousands separator appearing after the decimal point is not stripped.
- Related: `money` (format numbers as currency strings — the inverse-style operation).

## See also
- [money](money.md)
- [eval](eval.md)
- [edit](edit.md)
- [conventions](conventions.md)
