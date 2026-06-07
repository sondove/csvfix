# csvfix inter

> interleave fields from two CSV sources

## Synopsis
```
csvfix inter [options] [file ...]
```

## Description
Interleaves fields from two (and only two) CSV sources. It reads one row from each source in lockstep and emits a single output row built from fields drawn from either source, in the order given by `-f` (or `-fn`). Each field spec is prefixed with `L` (first/left source) or `R` (second/right source). Reading stops when the left-hand source is exhausted; if the right-hand source runs out first, its fields are treated as empty. If no `-f`/`-fn` flag is given, the command appends all fields of the right source to all fields of the left source. Exactly two input streams must be supplied (positional file arguments, or stdin counting as one).

## Options
- `-f fields` — comma-separated list of field specs defining the output field order. Each spec is a source prefix (`L` for the first/left input, `R` for the second/right input) followed by a 1-based field index, e.g. `L1,R2,L3`. Index must be 1 or greater. A spec that points past the end of its row yields an empty field. If omitted, the right source's fields are appended to the left source's fields.
- `-fn fields` — like `-f` but each spec is a source prefix (`L`/`R`) followed by a header name matched against that source's first row, e.g. `Lname,Rdob,Lsex`. Mutually exclusive with `-f`.

Positional arguments: exactly two input sources (files, or `-`/stdin). Supplying any other number is an error.

## Selecting fields by name
`-f` takes source-prefixed 1-based numeric indexes (`L1`, `R2`); `-fn` takes source-prefixed header names matched case-insensitively (`L` names resolve against the first input's header, `R` names against the second), e.g. `inter -fn Lname,Rdob`. `-f` and `-fn` are mutually exclusive. With `-fn` the header row of each source is consumed to resolve names and is passed through transformed like data (so the output's first row is the interleaved header names) unless you also pass `-ifn` to drop the header records.

## Common options
Also accepts the standard options — see [conventions](conventions.md). From the generic-flag token `#SMQ,SEP,IBL,IFN,OFL`: smart-quote output (`-smq`, `-sqf`), CSV separators (`-sep`, `-rsep`, `-osep`), ignore blank input lines (`-ibl`), ignore field-name header record (`-ifn`), and output file (`-o`). The binary also accepts `-hdr s` to write a header record.

## Examples

### Interleave by index
Input `a.csv`:
```
"Charles","M"
"Jane","F"
"Herman","M"
```
Input `b.csv`:
```
"1/12/1980"
"23/4/1964"
"3/3/1878"
```
Command — put the right source's field between the left source's two fields:
```
csvfix inter -f L1,R1,L2 a.csv b.csv
```
Output:
```
"Charles","1/12/1980","M"
"Jane","23/4/1964","F"
"Herman","3/3/1878","M"
```

### No -f: append right to left
With the same `a.csv` and `b.csv`:
```
csvfix inter a.csv b.csv
```
Output:
```
"Charles","M","1/12/1980"
"Jane","F","23/4/1964"
"Herman","M","3/3/1878"
```

### Right source shorter than left
With `a.csv` (3 rows) and `b2.csv` containing only:
```
"1/12/1980"
"23/4/1964"
```
Command:
```
csvfix inter -f L1,R1,L2 a.csv b2.csv
```
Output (the missing right-hand row becomes empty):
```
"Charles","1/12/1980","M"
"Jane","23/4/1964","F"
"Herman","","M"
```

### Interleave by header name (-fn)
Input `na.csv`:
```
name,sex
"Charles","M"
"Jane","F"
```
Input `nb.csv`:
```
dob
"1/12/1980"
"23/4/1964"
```
Command:
```
csvfix inter -fn Lname,Rdob,Lsex na.csv nb.csv
```
Output (header row interleaved like data):
```
"name","dob","sex"
"Charles","1/12/1980","M"
"Jane","23/4/1964","F"
```
Add `-ifn` to drop the header records:
```
csvfix inter -fn Lname,Rdob,Lsex -ifn na.csv nb.csv
```
Output:
```
"Charles","1/12/1980","M"
"Jane","23/4/1964","F"
```

## Notes
- Exactly two input sources are required; the command throws "Command requires exactly two input streams" otherwise.
- Iteration is driven by the left source: it stops when the left source is exhausted. Rows remaining in the right source after the left ends are never read.
- If the right source has fewer rows than the left, the absent right rows are treated as empty (each `R` spec yields an empty field for those rows).
- A field index/name that does not exist in its row resolves to an empty string rather than an error (out-of-range indexes are tolerated at runtime; `-fn` name lookup against the header is strict and will error if the name is not found).
- Field indexes for `-f` are 1-based and must be >= 1; `L0`/`R0` and non-integer indexes are errors. Each spec needs at least a prefix plus one character.
- With `-fn`, the header row is consumed only to resolve names and is otherwise interleaved and emitted as data unless `-ifn` is given.
- Without `-f`/`-fn`, output is the left row followed by the right row (simple horizontal concatenation), similar in spirit to `merge`/`join` but positional rather than key-based.

## See also
- [order](order.md)
- [join](join.md)
- [merge](merge.md)
