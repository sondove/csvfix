# csvfix edit

> edit fields

## Synopsis
```
csvfix edit [options] [file ...]
```

## Description
`edit` performs `sed`-style search-and-replace editing on CSV field values. Each `-e` command is a substitute expression of the form `s/find/replace/opts`, where `find` is a regular expression. Reads CSV from the named files (or standard input if none are given) and writes the edited CSV to standard output (or to `-o file`). Multiple `-e` commands are applied sequentially to each selected field. For simple value-to-value mapping that does not need regular expressions, consider the `map` command instead.

## Options
- `-e cmd` — specify an edit command (required, repeatable). Only the substitute command `s` is implemented: `s/find/replace/opts`. `find` is a regular expression; `replace` is the replacement text; `opts` may be `i` (case-insensitive) and/or `g` (replace all occurrences, not just the first). The separator after `s` may be any character except `\` (e.g. `s://:--:g` swaps the delimiter to `:`). Captured groups `\(...\)` in `find` are recalled in `replace` as `\1`..`\9`. Commands are applied in the order given.
- `-f fields` — comma-separated list of 1-based field indexes to edit (default: all fields).
- `-fn names` — comma-separated list of header names to edit, matched case-insensitively against the first input row (default: all fields). Mutually exclusive with `-f`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive. When you use `-fn`, the header row is itself treated as a data row and will be transformed by the `-e` commands like any other row unless you also pass `-ifn`, which drops the header from processing/output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The `#ALL,SKIP,PASS` token means all standard options apply: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank lines; `-smq` smart-quote and `-sqf` force-quote fields; `-hdr` write a header record; `-ifn` ignore the field-name header row; and `-skip`/`-pass` row filtering (rows matching `-skip` are dropped, rows matching `-pass` are output unchanged without editing).

## Examples

### Strip lowercase letters from the first field
Input (`names.csv`):
```
"Charles","Dickens","M"
"Jane","Austen","F"
"Herman","Melville","M"
```
Command:
```
csvfix edit -e 's/[a-z]//g' -f 1 names.csv
```
Output:
```
"C","Dickens","M"
"J","Austen","F"
"H","Melville","M"
```

### Sequential substitutions on a single field
Same input as above. Command (two `-e` commands applied in order to field 3):
```
csvfix edit -e 's/M/Male/' -e 's/F/Female/' -f 3 names.csv
```
Output:
```
"Charles","Dickens","Male"
"Jane","Austen","Female"
"Herman","Melville","Male"
```

### Backreference capture
Same input. Wrap the sex indicator in colons using a captured group:
```
csvfix edit -e 's/\([MF]\)/:\1:/' -f 3 names.csv
```
Output:
```
"Charles","Dickens",":M:"
"Jane","Austen",":F:"
"Herman","Melville",":M:"
```

### Select fields by header name (with -ifn)
Input (`names2.csv`):
```
fname,lname,sex
Charles,Dickens,M
Jane,Austen,F
```
Command:
```
csvfix edit -e 's/M/Male/' -e 's/F/Female/' -fn sex -ifn names2.csv
```
Output (header dropped by `-ifn`):
```
"Charles","Dickens","Male"
"Jane","Austen","Female"
```

## Notes
- Only the `s` (substitute) command is implemented; any other command letter raises an error ("Invalid edit sub-comand"). An empty or missing command, or a `\` used as the separator, is an error.
- The `find` part may not be empty ("Need expression to search for"). The `replace` part may be empty (deletes matches).
- Without `g`, only the first match in the field is replaced; with `g`, all matches are replaced.
- The separator character is whatever follows `s`; pick one that does not occur in your `find`/`replace` text to avoid quoting issues (e.g. `s|/|-|g`).
- When using `-fn` without `-ifn`, the header row is treated as data and the `s` commands will also rewrite header text — pass `-ifn` to skip the header.
- Regular expressions use CSVfix's regex engine; capture groups use the escaped `\(...\)` syntax and backreferences `\1`..`\9`. See the manual's Regular Expressions section.
- Rows matched by `-pass` are emitted unchanged; rows matched by `-skip` are not emitted at all.

## See also
- [map](map.md) — simpler value-to-value mapping without regular expressions
- [find](find.md) — filter rows by field-value matching
- [trim](trim.md) — strip whitespace / truncate fields
- [escape](escape.md) — escape characters for downstream systems
- [eval](eval.md) — expression-based field computation
