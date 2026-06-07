# csvfix diff

> compare two CSV files

## Synopsis
```
csvfix diff [options] file1 file2
```

## Description
Compares two (and only two) CSV files and reports the changes needed to turn the
first ("left"/source) file into the second ("right"/destination) file. Diffing is
done on parsed CSV field *contents*, not raw text, so results will differ from a
plain text diff — this is intentional. The output is **not** standard CSV-to-process
data: each emitted row is prefixed with an indicator field. `diff` requires exactly
two file arguments (it does not read standard input as a single stream); supplying
zero, one, or more than two inputs is an error. The process exit code is `0` when the
files are identical, `1` when they differ, and non-zero on error — useful in scripts.

## Options
- `-f fields` — comma-separated 1-based field indexes to compare; only these fields are used to decide whether two rows are equal (default: compare all fields). Mutually exclusive with `-fn`.
- `-fn names` — comma-separated header names to compare, matched case-insensitively against the first row of `file1`. Mutually exclusive with `-f`.
- `-q` — quiet: do not print the diff report, only set the exit status (0 = same, 1 = different).
- `-ic` — ignore case differences when comparing field values.
- `-is` — ignore leading and trailing spaces when comparing field values.

`file1` and `file2` are positional and both required.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched
case-insensitively against the first input row of `file1`. `-f` and `-fn` are mutually
exclusive. Names are resolved against the header of `file1`; the header itself is
still read as a data row and participates in the diff (so a row-1 header difference
between the two files is reported) unless you also pass `-ifn` to drop the field-name
record. Example: `csvfix diff -fn name file1.csv file2.csv`.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source
generic-flag token is `#ALL`, so the full standard group applies: output `-o`, CSV
separators `-sep`/`-rsep`/`-osep`, `-ibl` (ignore blank input lines), `-smq` (smart
quotes on output), `-sqf` (fields to force-quote), `-hdr` (write a header record),
and `-ifn` (ignore field-name header record).

## Examples

### Added, deleted and replaced rows
Input `left.csv`:
```
1,one
2,two
3,three
4,four
```
Input `right.csv`:
```
1,one
2,two
5,six
4,four
7,seven
```
Command:
```
csvfix diff left.csv right.csv
```
Output (exit status 1):
```
"-","3","3","three"
"+","3","5","six"
"+","5","7","seven"
```
The first field is the indicator: `-` = a row present in `file1` that must be
removed, `+` = a row from `file2` that must be added. The second field is the
1-based row number in the relevant file; the remaining fields are the original row.

### Restrict comparison to one field, with case folding
Input `left.csv`:
```
A,one
B,two
C,three
```
Input `right.csv`:
```
a,one
B,xxx
C,three
```
Comparing only field 1, case-sensitively, reports the `A` vs `a` change:
```
csvfix diff -f 1 left.csv right.csv
```
Output (exit status 1):
```
"-","1","A","one"
"+","1","a","one"
```
Adding `-ic` makes `A` and `a` equal, and field 2 is not compared, so the files are
considered identical:
```
csvfix diff -f 1 -ic left.csv right.csv
```
Output: none (exit status 0).

## Notes
- The command is described as "experimental" in the manual: it works but has not been heavily tested.
- Equality is decided per row over the selected fields (or all fields by default). When `-f`/`-fn` is given, only those fields affect matching; other field differences are invisible to the diff.
- The diff algorithm is a longest-common-subsequence style match; a changed line typically shows as a `-` (old) followed by `+` (new) pair.
- Row numbers in the output are 1-based positions within each file. The header row counts as row 1, so with `-fn` a difference in the first data row appears at index 2 (use `-ifn` to drop the header from consideration).
- Exit status: `0` identical, `1` different, non-zero on error (e.g. wrong number of input files). With `-q` the report is suppressed but the status is still set, making `diff -q a.csv b.csv` a scriptable equality test.
- The output is not re-ingestible CSV-for-processing; it is a CSVfix-specific patch-like report and its format may change in future versions.

## See also
- [check](check.md)
- [unique](unique.md)
- [order](order.md)
