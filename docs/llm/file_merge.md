# csvfix file_merge

> merge multiple sorted CSV files

## Synopsis
```
csvfix file_merge [options] [file ...]
```

## Description
Performs a classic merge of two or more already-sorted CSV inputs into a single
CSV stream. On each step it reads the current row from every input, compares
them, and writes out the lowest row, advancing only that input — like the merge
phase of a merge-sort. The inputs MUST already be sorted in ascending order for
the result to be sorted; `file_merge` does not sort, it only merges. Comparison
is performed field-by-field as **strings** (lexicographic), not numerically.
The result is written to standard output (or to `-o file`); the command name
`file_merge` is an alias for the internal command `fmerge`, so
`csvfix fmerge ...` also works.

## Options
- `-f fields` — comma-separated list of 1-based field indexes to compare when deciding which row is lowest (default: compare all fields). Mutually exclusive with `-fn`.
- `-fn names` — comma-separated list of header names (matched case-insensitively against the first input row) to compare when merging. Mutually exclusive with `-f`.

Positional arguments are the input files to merge; if none are given, input is
read from standard input (a single stream). Pass two or more files to actually
merge.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched
case-insensitively against the first input row. `-f` and `-fn` are mutually
exclusive. With `-fn`, the field names are resolved once from the header of the
first input that yields a row. The header row of each input is otherwise passed
through like data unless you also pass `-ifn`, which drops the field-name record
from every input.

Example: `csvfix file_merge -fn name a.csv b.csv` merges on the `name` column.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The
source help token is `#ALL`, so every standard group applies: output `-o`; CSV
separators `-sep` / `-rsep` / `-osep`; `-ibl` ignore blank input lines; `-smq`
smart-quote and `-sqf` quote-specific-fields on output; `-hdr s` write a header
record; `-ifn` ignore (drop) the field-name header record; and the `-skip` /
`-pass` row-filtering options.

## Examples

### Merge two sorted files (default: compare all fields)
Input `a.csv`:
```
apple,1
cherry,3
mango,5
```
Input `b.csv`:
```
banana,2
date,4
fig,6
```
Command:
```
csvfix file_merge a.csv b.csv
```
Output (verified):
```
"apple","1"
"banana","2"
"cherry","3"
"date","4"
"fig","6"
"mango","5"
```

### Merge on a named field, dropping headers
Input `h1.csv`:
```
name,qty
apple,1
mango,5
```
Input `h2.csv`:
```
name,qty
banana,2
date,4
```
Command:
```
csvfix file_merge -fn name -ifn h1.csv h2.csv
```
Output (verified):
```
"apple","1"
"banana","2"
"date","4"
"mango","5"
```

## Notes
- Inputs must be pre-sorted ascending. `file_merge` only merges; it does not
  sort. If the inputs are not sorted, the output will not be sorted either.
- The merge picks, at each step, the row that compares lowest across all current
  input rows. Because comparison stops as soon as the chosen input is exhausted
  relative to the others, the final tail of output reflects whichever single
  input still has rows — so an unsorted input (or a non-default `-f`/`-fn` key
  whose ties leave secondary fields unordered) can yield output that is not
  globally sorted. In the first example above `"fig","6"` precedes `"mango","5"`
  because `fig < mango` lexicographically, which is correct for the merge key.
- Comparison is purely lexicographic (string) on the selected fields; numeric
  values are compared as text (e.g. `"10" < "9"`).
- The older HTML manual documents a required `-c fields` flag; this is stale.
  The built binary uses `-f` (numeric) and `-fn` (by name), and neither is
  required (default is to compare all fields).
- Related: use [sort](sort.md) to produce the sorted inputs, and
  [unique](unique.md) to remove duplicate rows from the merged result.

## See also
[sort](sort.md), [unique](unique.md), [order](order.md), [merge](merge.md)
