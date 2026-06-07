# csvfix ascii_table

> produce ascii table

## Synopsis
```
csvfix ascii_table [options] [file ...]
```

## Description
Formats CSV input as an "ASCII art" table (the output is NOT CSV). It reads all input rows into memory first so it can compute the maximum width of each column, then writes the table bordered with `+`, `-` and `|` characters to standard output (or to `-o`). Each input field becomes a cell, left-aligned and space-padded to its column width. Useful for presentation and documentation. For XHTML tables, use `to_xml` instead.

## Options
- `-h head` — comma-separated list of column headers to print as the first (heading) row; the heading text is centred in its cell. Use `-h @` to instead interpret the first input line as the header row. Default: no header row (all input rows treated as data).
- `-ra fields` — comma-separated list of 1-based field indexes whose cells are right-aligned instead of the default left-aligned (e.g. `-ra 2,3`). Default: all cells left-aligned.
- `-s` — insert a separator line after every data row. Default: separators only at the top and bottom of the table.

Positional arguments are input filenames; if none are given, input is read from standard input.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Per the source generic-flag token `#IBL,IFN,OFL,SKIP`, the applicable groups are: `-ibl` (ignore blank input lines), `-ifn` (ignore field-name header record), `-o file` (write output to a file), and `-skip t` (skip records for which expression `t` is true). Note there is no `-fn` (select fields by name) support.

## Examples

Input `/tmp/names.csv`:
```
Charles,Dickens,M
Jane,Austen,F
Herman,Melville,M
```

Command with explicit headers:
```
csvfix ascii_table -h "Forename,Surname,Sex" /tmp/names.csv
```
Output:
```
+----------+----------+-----+ 
| Forename | Surname  | Sex |
+----------+----------+-----+
| Charles  | Dickens  | M   |
| Jane     | Austen   | F   |
| Herman   | Melville | M   |
+----------+----------+-----+
```

Same input, no headers (every row is data):
```
csvfix ascii_table /tmp/names.csv
```
Output:
```
+---------+----------+---+ 
| Charles | Dickens  | M |
| Jane    | Austen   | F |
| Herman  | Melville | M |
+---------+----------+---+
```

Input `/tmp/sales.csv` using the first line as the header, right-aligning the numeric columns, and a separator after every row:
```
Region,Units,Revenue
North,12,1200
South,3,450
West,150,18000
```
```
csvfix ascii_table -h @ -ra "2,3" -s /tmp/sales.csv
```
Output:
```
+--------+-------+---------+ 
| Region | Units | Revenue |
+--------+-------+---------+
| North  |    12 |    1200 |
+--------+-------+---------+
| South  |     3 |     450 |
+--------+-------+---------+
| West   |   150 |   18000 |
+--------+-------+---------+
```

## Notes
- Output is plain text, not CSV; do not pipe it into other csvfix commands expecting CSV.
- The very first (top) separator line is emitted with a trailing space before the newline (e.g. `+----------+----------+-----+ `). This is a known quirk of the source and is reproduced verbatim above.
- Header cells are centred; data cells are left-aligned by default, or right-aligned if their index is listed in `-ra`.
- Column widths are the maximum cell width seen, including the header text — so a wide header widens its column.
- `-ra` uses 1-based field indexes (the same indexing as `-f` elsewhere in csvfix).
- The usage line in `csvfix help ascii_table` prints `usage: csvfix echo ...`; this is a copy/paste artifact in the help string — the command name is `ascii_table`.
- Rows shorter than the widest row are padded with empty trailing cells so the table stays rectangular.

## See also
- [to_xml](to_xml.md)
- [order](order.md)
