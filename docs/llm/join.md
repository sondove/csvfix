# csvfix join

> perform relational join

## Synopsis
```
csvfix join [options] [file ...]
```

## Description
Performs a relational join between two CSV inputs, typically to enrich one data set with fields from another. Unlike most CSVfix commands, `join` treats the **last** input stream specially: every other input stream is joined, in turn, against that last stream. So `join file1 file2 file3` joins `file1` with `file3` and emits output, then joins `file2` with `file3`. It therefore requires at least two input streams (any of which may be standard input). For each matching row, the fields of the left (non-last) row are written followed by the fields of the matched last-stream row; the join key fields from the last stream are removed from the output unless `-k` is given. Output is CSV written to standard output (or `-o`).

## Options
- `-f jlist` — join field specification by 1-based numeric index. A comma-separated list of `left:right` colon pairs, e.g. `-f 2:1` (join left field 2 to right field 1) or `-f 1:2,3:3` (multi-column join). Required unless `-fn` is given. Indices must be >= 1.
- `-fn jlist` — join field specification by header name, e.g. `-fn id:custid`. Same comma-list / `left:right` syntax as `-f`, but each side is a header name matched case-insensitively against the relevant stream's first row. Mutually exclusive with `-f`.
- `-oj` — perform an outer join: left rows with no match in the last stream are still output (without appended right fields), instead of being dropped. Mutually exclusive with `-inv`.
- `-inv` — invert the join: output only left rows that have **no** match in the last stream (an anti-join / exclusion list). Mutually exclusive with `-oj`.
- `-ic` — ignore character case when comparing join key values (e.g. `foo` matches `FOO`). Default: case-sensitive.
- `-k` — keep the join (key) fields from the last stream in the output. Default: the right-side join fields are dropped from the appended fields.

Positional arguments are the input files; at least two input streams are required (a file may be replaced by standard input).

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively. For `join` the spec is a list of `left:right` pairs, e.g. `join -fn code:custid` joins the left stream's `code` column to the last stream's `custid` column. Left names are resolved against each left stream's first row; right names against the last stream's first row. `-f` and `-fn` are mutually exclusive (specifying both is an error). Because the header rows are read as data, they are joined and passed through like any other row; pass `-ifn` so the field-name header is consumed for name resolution rather than treated as joinable data.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The help token is `#ALL`, so the full standard set applies: output `-o`; CSV separators `-sep` / `-rsep` / `-osep`; `-ibl` (ignore blank lines); `-smq` (smart quoting) and `-sqf` (force-quote fields); `-hdr s` (emit a header record); `-ifn` (ignore/consume the field-name header record, used with `-fn`); and the `-skip` / `-pass` row filters.

## Examples

### Inner join by index
Input `cities.csv`:
```
London,GB
Paris,FR
Rome,IT
Oslo,NO
```
Input `countries.csv`:
```
GB,United Kingdom
FR,France
IT,Italy
```
Command (join city field 2 to country field 1):
```
csvfix join -f 2:1 cities.csv countries.csv
```
Output (right join field dropped; rows with no match are dropped):
```
"London","GB","United Kingdom"
"Paris","FR","France"
"Rome","IT","Italy"
```

### Outer join, invert, and keep
Same inputs as above.

Outer join (`-oj`) keeps unmatched left rows:
```
csvfix join -oj -f 2:1 cities.csv countries.csv
```
```
"London","GB","United Kingdom"
"Paris","FR","France"
"Rome","IT","Italy"
"Oslo","NO"
```

Invert (`-inv`) outputs only unmatched left rows (exclusion list):
```
csvfix join -inv -f 2:1 cities.csv countries.csv
```
```
"Oslo","NO"
```

Keep join fields (`-k`) retains the right key column:
```
csvfix join -k -f 2:1 cities.csv countries.csv
```
```
"London","GB","GB","United Kingdom"
"Paris","FR","FR","France"
"Rome","IT","IT","Italy"
```

### Join by header name (`-fn`) with case-insensitive keys
Input `cities2.csv`:
```
city,code
London,gb
Paris,FR
```
Input `countries2.csv`:
```
code,name
GB,United Kingdom
FR,France
```
Command (`-ifn` consumes the header rows; `-ic` makes `gb` match `GB`):
```
csvfix join -fn code:code -ic -ifn cities2.csv countries2.csv
```
Output:
```
"London","gb","United Kingdom"
"Paris","FR","France"
```

## Notes
- Requires at least two input streams; fewer raises `ERROR: Need at least two input streams`.
- `-oj` and `-inv` cannot be combined; `-f` and `-fn` cannot be combined. Both raise an error and exit non-zero.
- `-f` / `-fn` is required; omitting the join spec raises `No join specified with -f flag`.
- The last stream is loaded entirely into an in-memory multimap keyed on the join columns, so duplicate keys on the right produce one output row per match (cartesian per key). The other streams are processed row by row, so they can be large.
- Output is always quoted CSV. The right-side join-key fields are removed from output unless `-k` is set; the left-side join fields are always retained.
- The multi-column join key concatenates the selected fields, so multi-key joins (`-f 1:2,3:3`) match only when all pairs are equal.
- Joins across more than two files are not supported in the sense of chaining; extra files are each joined independently against the last file.
- With `-fn` but without `-ifn`, header rows are treated as ordinary data and may themselves join (e.g. matching literal header text), which is usually not intended — pass `-ifn`.

## See also
- [order](order.md)
- [merge](merge.md)
- [find](find.md)
- [exclude](exclude.md)
- [pad](pad.md)
- [put](put.md)
