# csvfix echo

> echo input to output

## Synopsis
```
csvfix echo [options] [file ...]
```

## Description
`echo` reads CSV input and writes it back out unchanged in CSVfix's standard CSV format. It reads from the files named on the command line, or from standard input if no files are given (the special name `-` also means standard input), and writes to standard output unless `-o` redirects it. It is intended mainly for debugging, sanity-checking CSVfix itself, and listing CSV files in a normalised format. Combined with the separator flags (`-sep` / `-osep`) it provides a simple way to change the inter-field separator, and with `-smq` it can strip or alter output quoting.

## Options
`echo` has no command-specific options of its own — it only accepts the standard options listed below. Any number of input files may be given as positional arguments; they are processed in order and concatenated to the output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source help token is `#ALL,SKIP`, so all standard option groups apply plus row filtering via `-skip` (note: `-pass` is **not** supported). Relevant standard flags include: `-o file` (write to file instead of stdout), `-sep s` / `-rsep s` / `-osep s` (input, input-retained, and output field separators), `-ibl` (ignore blank input lines), `-smq` (smart-quote output, i.e. only quote fields that need it), `-sqf fields` (force-quote listed fields), `-hdr s` (write string `s` as a header record first), `-ifn` (ignore/drop a field-name header record), and `-skip t` (skip records for which expression `t` is true).

## Examples

Input file `/tmp/cities.csv`:
```
"London","GB"
"Paris","FR"
"Rome","IT"
```

Echo it unchanged (normalised CSV, all fields quoted):
```
csvfix echo /tmp/cities.csv
```
Output:
```
"London","GB"
"Paris","FR"
"Rome","IT"
```

Change the separator to a semicolon and use smart quoting (fields needing no quotes are emitted unquoted):
```
csvfix echo -smq -osep ';' /tmp/cities.csv
```
Output:
```
London;GB
Paris;FR
Rome;IT
```

Add a header record and drop rows matching an expression with `-skip`:
```
csvfix echo -hdr "city,country" -skip '$2 == "IT"' /tmp/cities.csv
```
Output:
```
city,country
"London","GB"
"Paris","FR"
```

## Notes
- `echo` makes no changes to field content; only formatting (quoting and separators) and row filtering can alter the output.
- By default every output field is quoted. Use `-smq` for "smart quotes" (quote only when necessary) or `-sqf` to force-quote specific fields.
- `-skip` uses the CSVfix expression language. Field references are `$1`, `$2`, ... (1-based), and equality is `==`, not `=` (a single `=` is assignment and will silently misbehave). A record is dropped when the expression evaluates true.
- Only `-skip` is available for row filtering; unlike many CSVfix commands `echo` does not support `-pass`.
- Multiple files (and `-`/stdin) can be mixed on the command line and are echoed in order.

## See also
- [order](order.md)
- [escape](escape.md)
- [trim](trim.md)
