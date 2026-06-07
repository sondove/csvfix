# csvfix file_split

> split into files

## Synopsis
```
csvfix file_split [options] [file ...]
```

## Description
Reads CSV input from the named files (or standard input) and splits it into multiple output files based on the values of one or more fields. All records that share the same values in the chosen fields are written to the same file. Unlike most CSVfix commands, `file_split` writes nothing to standard output and ignores the `-o` flag entirely; output goes only to the generated files. Files are numbered by default (`file_0001.csv`, `file_0002.csv`, ...), or named from the field values when `-ufn` is given. Files are opened in append mode and any pre-existing file of the same name is added to, so stale output should be cleared between runs.

## Options
You must supply exactly one of `-f` or `-fn` to indicate the split fields.

- `-f fields` — fields to split on, as a comma-separated list of 1-based numeric indexes (e.g. `-f 2` or `-f 1,3`). Mutually exclusive with `-fn`.
- `-fn names` — fields to split on, by header name matched against the first input row. Mutually exclusive with `-f`.
- `-fd dir` — directory to place output files in (default: current directory).
- `-fp pre` — filename prefix used when generating names (default: `file_`).
- `-fx ext` — filename extension used when generating names, without the dot (default: `csv`).
- `-ufn` — use the split-field content to generate file names instead of sequential numbers. No check is made that the values form valid filename components; the command fails if they do not.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row (e.g. `file_split -fn country`). `-f` and `-fn` are mutually exclusive, and exactly one is required. When using `-fn`, the header row is otherwise treated like data (it lands in one of the output files) unless you also pass `-ifn` to drop the field-name record.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#IBL,SEP,IFN,SMQ,SKIP,PASS`, so it honours: `-ibl` (ignore blank input lines), CSV separators `-sep`/`-rsep`/`-osep`, `-ifn` (ignore field-name header), `-smq` (smart-quote output), and `-skip`/`-pass` row filtering. It also accepts `-hdr s` and `-sqf fields`. Note `-pass` writes matching rows to standard output rather than to a split file, and `-o` has no effect.

## Examples

### Split by numeric field into numbered files
Input `cities.csv`:
```
London,GB
Manchester,GB
Paris,FR
Lyon,FR
Berlin,DE
```
Command:
```
csvfix file_split -f 2 cities.csv
```
Produces `file_0001.csv`, `file_0002.csv`, `file_0003.csv` (one per distinct value, in first-seen order):
```
== file_0001.csv ==
London,GB
Manchester,GB
== file_0002.csv ==
Paris,FR
Lyon,FR
== file_0003.csv ==
Berlin,DE
```

### Name files from field content, with custom dir/prefix/extension
Same `cities.csv` as above. Command:
```
csvfix file_split -f 2 -ufn -fd out -fp city_ -fx txt cities.csv
```
Writes into `out/` using the country code in the filenames:
```
== out/city_DE.txt ==
Berlin,DE
== out/city_FR.txt ==
Paris,FR
Lyon,FR
== out/city_GB.txt ==
London,GB
Manchester,GB
```

### Split by header name, dropping the header row
Input `cities.csv`:
```
name,country
London,GB
Paris,FR
Berlin,DE
```
Command:
```
csvfix file_split -fn country -ufn -ifn cities.csv
```
`-ifn` drops the header so it does not end up in an output file; `-ufn` names files from the `country` value:
```
== file_DE.csv ==
Berlin,DE
== file_FR.csv ==
Paris,FR
== file_GB.csv ==
London,GB
```

## Notes
- A split field (`-f` or `-fn`) is required; the command errors with "Need fields specified by -f or -fn flag" if neither is given.
- Output files are opened in append mode. Existing files of the same name are appended to (not truncated), so re-running without clearing the target directory will accumulate duplicate rows.
- Nothing is written to standard output and the `-o` flag is ignored — the only effect is the created files.
- File names are built as `<dir>/<prefix><number-or-key>.<ext>`. Numbered names use 4-digit zero padding (`0001`). With `-ufn`, when splitting on multiple fields the key values are joined with `_` in the filename.
- With `-ufn` there is no validation of the field values as filenames; values containing path separators or illegal characters will cause the command to fail.
- `-pass` routes matching rows to standard output instead of into a split file; `-skip` drops them entirely.
- Related: `find` and `remove` (select/drop rows), and `file_merge`/`split_fixed`/`split_char` for other file-oriented operations.

## See also
- [file_merge](file_merge.md)
- [split_char](split_char.md)
- [split_fixed](split_fixed.md)
- [find](find.md)
- [remove](remove.md)
