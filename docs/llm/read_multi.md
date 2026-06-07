# csvfix read_multi

> read multi-line data

## Synopsis
```
csvfix read_multi [options] [file ...]
```

## Description
`read_multi` converts multi-line input data into CSV. It reads input one line at a time and groups consecutive lines into a single CSV record, with each input line becoming one field of that record. Records are delimited either by a fixed number of lines (`-n`) or by a separator line (`-s`). The resulting CSV rows are written to standard output (or to `-o`). Use exactly one of `-n` or `-s` — one is required.

When using `-s`, records may have differing numbers of fields. If you need uniform field counts, pipe the output through the [pad](pad.md) command.

## Options
- `-n lines` — number of lines in each multi-line record. Each group of `lines` consecutive input lines becomes one CSV row. Must be an integer between 1 and 200 inclusive. (no default)
- `-s sep` — separator string marking the end of a record. Any input line equal to `sep` ends the current record; intervening lines become its fields. Allows variable field counts per record. Must be non-empty. (no default)

`-n` and `-s` are mutually exclusive, and exactly one of them must be supplied.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#SMQ,OFL`, so only the smart-quote and output-file groups apply: `-smq` (smart-quote output), `-sqf fields` (force-quote the listed fields), and `-o file` (write output to a file instead of standard output).

## Examples

### Fixed number of lines per record (`-n`)
Input (`heroes.txt`):
```
Superman
Fortress of Solitude
Batman
The Batcave
```
Command:
```
csvfix read_multi -n 2 heroes.txt
```
Output:
```
"Superman","Fortress of Solitude"
"Batman","The Batcave"
```

### Separator-delimited records (`-s`)
Input (`heroes2.txt`):
```
Superman
Fortress of Solitude
-
Batman
The Batcave
Gotham City
-
```
Command:
```
csvfix read_multi -s - heroes2.txt
```
Output:
```
"Superman","Fortress of Solitude"
"Batman","The Batcave","Gotham City"
```
Note how records can have different numbers of fields when using `-s`.

### Suppressing quotes with `-smq`
Using the first input above:
```
csvfix read_multi -n 2 -smq heroes.txt
```
Output:
```
Superman,Fortress of Solitude
Batman,The Batcave
```

## Notes
- Exactly one of `-n` or `-s` is required; supplying both, or neither, is an error.
- `-n` is bounded: values below 1 or above 200 are rejected ("Invalid number of lines"). The 200 cap guards against mistaken huge values.
- `-s` matches a whole input line (the entire line must equal `sep`); it is not a substring match. An empty separator is rejected ("Empty separator").
- With `-s`, a separator immediately following another separator produces an empty record (no fields). At end of input, any partially accumulated record (no trailing separator) is still written out.
- With `-n`, a trailing partial group of fewer than `lines` lines at end of input is written out as a short final record.
- By default fields are always quoted on output; pass `-smq` for smart quoting (quote only when needed).
- Related sibling readers: [read_fixed](read_fixed.md) (fixed-column data) and [read_dsv](read_dsv.md) (delimiter-separated values). The inverse operation is `write_multi`.

## See also
- [read_fixed](read_fixed.md)
- [read_dsv](read_dsv.md)
- [pad](pad.md)
