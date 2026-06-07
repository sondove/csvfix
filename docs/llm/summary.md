# csvfix summary

> summarise CSV data

## Synopsis
```
csvfix summary [options] [file ...]
```

## Description
Applies a single summarisation function across all input rows and writes the result to standard output as CSV. Input is read from the named files or standard input. Exactly one summary flag must be supplied; supplying none or more than one is an error. Most functions (`-avg`, `-sum`, `-med`, `-mod`, `-min`, `-max`, `-frq`) take a comma-separated list of 1-based field indexes; numeric functions treat the named fields as numbers. The command buffers all rows in memory before producing output (except `-siz`, which streams).

## Options
Exactly one of the following must be given (they are mutually exclusive):

- `-avg fields` — numeric average of each listed field, output as one row, one value per field.
- `-sum fields` — numeric sum of each listed field, output as one row, one value per field.
- `-med fields` — median of each listed field (numeric; rows sorted per field), output as one row.
- `-mod fields` — mode: outputs every input row (with original fields) whose combined `fields` value is among the most frequent, each prefixed with the maximum frequency count.
- `-frq fields` — frequency: outputs every input row in original order, prefixed with the count of rows sharing the same combined `fields` value.
- `-min fields` — outputs every input row whose `fields` value(s) equal the minimum (comparison is numeric when both values are numbers, else string).
- `-max fields` — outputs every input row whose `fields` value(s) equal the maximum (same comparison rule as `-min`).
- `-siz` — for every field column, outputs `index: minlen,maxlen` (1-based index, min and max string lengths). Takes no field argument; covers all fields.

`fields` is a comma-separated list of 1-based field indexes, e.g. `1,2`. Referencing a field index beyond a row's width is an error for the numeric/comparison functions.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Per the source token `#SMQ,SEP,IBL,IFN,OFL`, the applicable groups are: output `-o`; CSV separators `-sep` / `-rsep` / `-osep`; `-ibl` (ignore blank input lines); `-ifn` (ignore field-name header record); `-smq` (smart quoting) and `-sqf fields` (force-quote fields); `-hdr s` (write string `s` as a leading header record). Row-filtering `-skip` / `-pass` are not part of this command's option set.

## Examples

Input `nums.csv`:
```
1,30
3,49
5,40
```

Average of fields 1 and 2:
```
csvfix summary -avg 1,2 nums.csv
```
```
"3","39.6667"
```

Sum of fields 1 and 2:
```
csvfix summary -sum 1,2 nums.csv
```
```
"9","119"
```

Median of fields 1 and 2:
```
csvfix summary -med 1,2 nums.csv
```
```
"3","40"
```

Input `cities.csv`:
```
London,GB
Paris,FR
Edinburgh,GB
London,GB
Amsterdam,NL
```

Prepend frequency of field 2 (country) to every row:
```
csvfix summary -frq 2 cities.csv
```
```
"3","London","GB"
"1","Paris","FR"
"3","Edinburgh","GB"
"3","London","GB"
"1","Amsterdam","NL"
```

Mode of field 2 — emits the rows tied for the highest frequency, prefixed with that count:
```
csvfix summary -mod 2 cities.csv
```
```
"3","London","GB"
"3","Edinburgh","GB"
"3","London","GB"
```

Input `vals.csv`:
```
10,a
5,b
20,c
20,d
```

Max of field 1 — all rows holding the maximum value are emitted:
```
csvfix summary -max 1 vals.csv
```
```
"20","c"
"20","d"
```

Min of field 1:
```
csvfix summary -min 1 vals.csv
```
```
"5","b"
```

Field-size report. Input `siz.csv`:
```
al,bobby
x,longvalue
```
```
csvfix summary -siz siz.csv
```
```
1: 1,2
2: 5,9
```
Note: `-siz` output is plain text (`index: min,max`), not CSV.

## Notes
- Exactly one summary flag is required; zero flags gives `Need a summary flag` and two or more gives `Only one summary flag allowed`.
- With no input rows (and a non-`-siz` flag) the command errors with `No input`.
- `-avg` / `-sum` / `-med` interpret fields via numeric conversion; non-numeric text converts to 0. `-med` sorts numerically per field.
- `-frq` and `-mod` build a composite key by concatenating the listed fields (separated by NUL), so multiple fields define a compound grouping key.
- `-min` / `-max` compare on the listed field(s): numeric when both compared values are numbers, otherwise lexicographic string comparison. All rows equal to the extreme value are emitted.
- `-siz` reports per-column min and max string lengths over all fields and ignores the `fields` syntax entirely.
- `-frq` is closely related to the `stat` and `unique` commands for distribution/dedup style work; `eval` and `map` can compute derived values.

## See also
[eval](eval.md), [stat](stat.md), [unique](unique.md), [order](order.md)
