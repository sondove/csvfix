# csvfix money

> format fields as money/currency values

## Synopsis
```
csvfix money [options] [file ...]
```

## Description
`money` formats selected CSV fields as currency/money amounts. It reads CSV from the named files (or standard input) and writes CSV to standard output. Each targeted field that is numeric is reformatted to two decimal places with a configurable thousands separator, decimal point, currency symbol and sign prefixes (for example `12400.6` becomes `12,400.60`). Non-numeric values in targeted fields are passed through unchanged. By default the formatted value is appended as a new field; use `-r` to replace the original in place. Only the dollar-and-cents (two-decimal) model is supported.

## Options
- `-f fields` — comma-separated list of 1-based numeric field indexes to format. If omitted, all fields are formatted. Mutually exclusive with `-fn`.
- `-fn names` — comma-separated list of header names (matched against the first input row) selecting fields to format. Mutually exclusive with `-f`.
- `-dp chr` — single character to use as the decimal point symbol (default: `.`).
- `-ts chr` — single character to use as the thousands separator (default: `,`). An empty value (`-ts ""`) suppresses the separator entirely.
- `-cs sym` — string used as a currency symbol, prefixed to the amount after the sign (default: none).
- `-ms minus` — string prefixed to negative values (default: `-`).
- `-ps plus` — string prefixed to positive values (default: none).
- `-cn` — treat the input amount as an integer number of cents, dividing it by 100 before formatting (so `12345` becomes `123.45`). Takes no argument.
- `-r` — replace the targeted fields with the formatted value, rather than appending new fields (default: append).
- `-w width` — total output field width; the formatted amount is right-aligned within it (default: `0`, i.e. no alignment). Must be an integer in the range 0-50.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched against the first input row. `-f` and `-fn` are mutually exclusive. The header row is processed like data: a non-numeric header name is left unchanged but, without `-r`, still produces an appended (passed-through) copy of that field. Pass `-ifn` to drop the field-name header record from output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The generic-flag token is `#ALL,SKIP,PASS`, so all standard groups apply: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; `-smq` smart-quote and `-sqf` quote-specific-fields; `-hdr` write a header record; `-ifn` ignore field-name header; and `-skip`/`-pass` row filtering.

## Examples

### Append formatted currency for one field
Input (`money1.csv`):
```
item,price
Widget,12400.6
Gadget,-3.5
Gizmo,abc
```
Command:
```
csvfix money -f 2 money1.csv
```
Output (non-numeric values pass through; formatted copy appended):
```
"item","price","price"
"Widget","12400.6","12,400.60"
"Gadget","-3.5","-3.50"
"Gizmo","abc","abc"
```

### Replace in place with a currency symbol
Command:
```
csvfix money -f 2 -r -cs '$' money1.csv
```
Output (the sign is emitted before the currency symbol):
```
"item","price"
"Widget","$12,400.60"
"Gadget","-$3.50"
"Gizmo","abc"
```

### Treat input as cents
Input (`money2.csv`):
```
12345
99
-500
```
Command:
```
csvfix money -cn -r -cs '$' money2.csv
```
Output:
```
"$123.45"
"$0.99"
"-$5.00"
```

### European-style decimal point and thousands separator
Command:
```
csvfix money -r -dp ',' -ts '.' money2.csv
```
Output:
```
"12.345,00"
"99,00"
"-500,00"
```

## Notes
- Only numeric fields are reformatted; any non-numeric value in a targeted field is emitted unchanged.
- All amounts are formatted to exactly two decimal places and the input is rounded/truncated to cents by fixed-precision conversion.
- The output order is sign prefix (`-ms`/`-ps`), then currency symbol (`-cs`), then the right-aligned numeric amount. With `-w`, only the numeric amount is padded; the sign prefix is emitted before the padding, so a negative value like `-500.00` at width 12 renders as `-         500.00` (the minus sits left of the padded number).
- `-dp` and `-ts` each take a single character; supplying a multi-character decimal point raises "Invalid decimal point value". `-w` must be an integer in 0-50 or the command errors.
- Without `-r`, formatted fields are appended after all original fields, so the column count grows by the number of targeted fields.
- This is the formatting counterpart of [number](number.md), which parses formatted numeric strings back into plain arithmetic values.

## See also
- [number](number.md)
- [order](order.md)
- [conventions](conventions.md)
