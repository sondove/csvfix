# csvfix eval

> expression evaluation

## Synopsis
```
csvfix eval [options] [file ...]
```

## Description
Performs expression evaluation on CSV fields using CSVfix's expression language, evaluating one or more expressions once per input row. Each input field is bound to a positional parameter: `$1` is the first field, `$2` the second, and so on. By default the result of each `-e` expression is appended as a new field to the output row; `-r` replaces a named field instead. Reads CSV from the named files (or standard input if none) and writes CSV to standard output (or the `-o` file). At least one `-e` or `-r` option is required.

## Options
- `-e expr` — evaluate `expr` and append the result as a new field at the end of the row. May be given multiple times; each adds one field, in order.
- `-r f,expr` — evaluate `expr` and replace field `f` (1-based integer, must be `>= 1`) with the result instead of appending. The integer field index and the expression are separated by the first comma. Cannot be combined with `-d`.
- `-if expr` — conditional evaluation. If `expr` is true, the next `-e` is evaluated and the one after it is skipped; if false, the next `-e` is skipped and the one after it is evaluated. Requires exactly two `-e` options to follow it, and consecutive `-if` options are not allowed. Used to avoid evaluating both branches (the expression language's `if()` function always evaluates both of its arguments).
- `-d` — discard all input fields, so the output row contains only the results of the `-e`/`-r` expressions. Cannot be combined with `-r`.

Inside expressions you can also reference these special variables: `line` (current input line number), `file` (current input file name), and `fields` (number of fields in the current row). Note: in most shells `$1`, `$2` etc. are shell variables, so always single-quote expressions on Unix (double-quote on Windows).

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token is `#ALL,SKIP,PASS`, so the full ALL group applies: output `-o`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` (ignore blank input lines); `-smq` (smart quotes) and `-sqf` (force-quote fields); `-hdr` (write a header record); `-ifn` (ignore field-name header record); plus row filtering with `-skip`/`-pass`.

## Examples

### Append the average of two fields
Input (`minmax.csv`):
```
"2009-01-01","-5","2"
"2009-01-02","-6","0"
"2009-01-03","-5","2"
```
Command:
```
csvfix eval -e '($2 + $3)/2' minmax.csv
```
Output:
```
"2009-01-01","-5","2","-1.5"
"2009-01-02","-6","0","-3"
"2009-01-03","-5","2","-1.5"
```

### Replace a field in place with `-r`
Input (`names.csv`):
```
"Charles","Dickens","M"
"Jane","Austen","F"
```
Command:
```
csvfix eval -r '1,upper($1)' names.csv
```
Output:
```
"CHARLES","Dickens","M"
"JANE","Austen","F"
```

### Guard a division with `-if`
Input (`div.csv`):
```
"10","2"
"5","0"
```
Command:
```
csvfix eval -if '$2 == 0' -e '"divide by zero"' -e '$1 / $2' div.csv
```
Output:
```
"10","2","5"
"5","0","divide by zero"
```

### Discard input with `-d`
Input (`names.csv`, as above). Command:
```
csvfix eval -d -e '$1 . " " . $2' names.csv
```
Output:
```
"Charles Dickens"
"Jane Austen"
```

## Notes
- At least one `-e` or `-r` must be given, otherwise the command errors with "Need at least one of -e or -r options".
- `-r` field indexes are 1-based; an index of `0` or a non-integer is rejected. If the index is greater than the current row size, the result is appended (same as `-e`) rather than replacing.
- `-d` and `-r` are mutually exclusive.
- `-if` requires two `-e` options after it and forbids two consecutive `-if`s; violating either raises an error at runtime.
- String concatenation uses `.`; comparison uses `==`. The expression language is shared with the `-if`/`-e`-style expressions in the `find`, `remove`, and `exclude` commands. See the manual's "Expression Language" section for the full function list (arithmetic, `upper`/`lower`, `if`, `match`, etc.).
- Because the `if()` function always evaluates both branches, prefer the `-if` option for conditional logic that must avoid evaluating an erroring branch (e.g. division by zero).

## See also
[order](order.md), [find](find.md), [remove](remove.md), [exclude](exclude.md), [summary](summary.md), [map](map.md)
