# csvfix printf

> printf-style formatting

## Synopsis
```
csvfix printf [options] [file ...]
```

## Description
`printf` formats each input CSV row using a C-`printf`-style format string supplied with `-fmt`. The fields of the row are consumed in order, one per conversion specifier in the format string. By default the formatted line is written to standard output as plain text (one line per input row); it is **not** CSV unless you write it that way yourself, or use `-csv` to append the formatted result as a new field on the original row. Reads CSV from the named files or standard input.

Conversion-to-field mapping: each `%...` specifier consumes the next field. Fields with no matching specifier are dropped; specifiers with no matching field produce the empty string. Numeric conversions (`d i o x X u c` integer, `f e E g G` real) coerce non-numeric / non-integer fields to `0`. String conversion is `s`. `%%` emits a literal `%`. The non-standard `%@` consumes a field but outputs nothing (use it to skip a field). Backslash escapes `\n \r \t` are recognised in literal text. Only the conversion types `d i o x X u c s f e E g G` are valid; any other type is an error.

## Options
- `-fmt fmt` — printf-style format string (required). Supports flags/width/precision (e.g. `%-10s`, `%8.2f`, `%1.1s`).
- `-f fields` — reorder fields before formatting, by 1-based numeric index (comma-separated, e.g. `-f 2,3,4`). Works like the `order` command's `-f`. Mutually exclusive with `-fn`.
- `-fn names` — reorder fields before formatting, by header name matched case-insensitively against the first input row (comma-separated). Mutually exclusive with `-f`.
- `-q` — apply CSV-style quoting (double up internal double-quotes) to strings produced by `%s` conversions; does not affect literal text. Mutually exclusive with `-csv`. (default: off)
- `-csv` — instead of replacing the row, append the formatted string as a new CSV field on the end of the input row and write the whole row as CSV. Mutually exclusive with `-q`. (default: off)

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive, and both simply define the order in which fields are fed to the `-fmt` specifiers. The header row is itself fed through the formatter like any data row unless you pass `-ifn` to drop the field-name record first.

## Common options
Also accepts the standard options — see [conventions](conventions.md). From the source generic-flag token `#SMQ,SEP,IBL,IFN,OFL,SEP,SKIP,PASS`: smart-quote `-smq`; CSV separators `-sep`/`-rsep`/`-osep`; ignore blank lines `-ibl`; ignore field-name header `-ifn`; output to file `-o`; row filtering `-skip`/`-pass`. (`-smq`/`-osep`/`-sqf`/`-hdr` only affect output when `-csv` produces CSV.)

## Examples

### Example 1 — initial and surname
Input `names.csv`:
```
Charles,Dickens
Jane,Austen
```
Command:
```
csvfix printf -fmt "%1.1s. %s" names.csv
```
Output:
```
C. Dickens
J. Austen
```

### Example 2 — numeric formatting with reordering and header skip
Input `sales.csv`:
```
id,product,qty,price
1,Widget,3,9.5
2,Gadget,10,19.99
```
Command (`-ifn` drops the header row; `-f 2,3,4` feeds product, qty, price to the format):
```
csvfix printf -ifn -fmt "%-10s qty=%d total=%8.2f" -f 2,3,4 sales.csv
```
Output:
```
Widget     qty=3 total=    9.50
Gadget     qty=10 total=   19.99
```

### Example 3 — append formatted text as a CSV field with -csv
Command (using the same `sales.csv`):
```
csvfix printf -ifn -csv -fmt "%s x %d" -f 2,3 sales.csv
```
Output:
```
"1","Widget","3","9.5","Widget x 3"
"2","Gadget","10","19.99","Gadget x 10"
```

### Example 4 — select fields by name with -fn
Command (using the same `sales.csv`):
```
csvfix printf -ifn -fn product,price -fmt "%s costs %.2f" sales.csv
```
Output:
```
Widget costs 9.50
Gadget costs 19.99
```

## Notes
- `-fmt` is mandatory; an empty format string is an error.
- `-q` and `-csv` cannot be combined.
- An invalid conversion type (anything outside `d i o x X u c s f e E g G`) aborts with an error; an unterminated `%` at end of string aborts with "Unexpected end of format".
- A lone `%` at the very end of the format string is treated as a literal `%`.
- Non-numeric fields formatted with an integer conversion become `0`; non-numeric fields formatted with a real conversion become `0.0`.
- Without `-f`/`-fn`, specifiers consume fields left-to-right in their natural order. If fields are out of order and you do not want to use `-f`/`-fn`, pipe through `order` first.
- Default output is plain (non-CSV) text. Use `-csv` (or build CSV syntax into `-fmt`) if you need CSV output. Related sibling commands: `template` (named-placeholder templating), `order` (field reordering), `echo`/`put` (constant/literal output).

## See also
[template](template.md), [order](order.md), [put](put.md)
