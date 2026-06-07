# csvfix shuffle

> randomly shuffle rows or fields

## Synopsis
```
csvfix shuffle [options] [file ...]
```

## Description
Reads CSV records from the named files (or standard input if none are given) and writes them back out in a randomised order. By default it shuffles whole rows: all input is read into memory, then rows are emitted in random order, so the command can be slow or unusable on very large inputs. With `-f`/`-fn` it instead shuffles the values *within* the listed fields of each row, processing rows one at a time (no full buffering). Use `-n` to emit only the first N rows of the shuffled result, which effectively picks N records at random with no duplicates.

## Options
- `-n count` — output only the first `count` rows after shuffling (random sample of `count` distinct rows). `count` must be an integer >= 0. Default: output all rows. Applies to row shuffling only; ignored when `-f`/`-fn` is used.
- `-rs seed` — integer seed for the random number generator. Default: seeded from the current date/time (non-reproducible). A fixed non-negative seed gives reproducible output; a negative value is treated as "use the clock".
- `-f fields` — shuffle the listed fields' values within each row instead of shuffling rows. `fields` is a comma-separated list of 1-based numeric indexes, e.g. `-f 1,2,3`. A field index past the end of a row is an error.
- `-fn names` — like `-f` but selects fields by header name (comma-separated), e.g. `-fn first,last`.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row (e.g. `-fn first,last`). Use `-f` or `-fn`, not both. With `-fn` the header row is itself treated as data — it gets shuffled/passed through like any other row — unless you also pass `-ifn` to drop the header from processing and output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#SMQ,SEP,IBL,IFN,OFL`: smart-quote output (`-smq`, `-sqf`), CSV separators (`-sep`/`-rsep`/`-osep`), ignore blank lines (`-ibl`), ignore field-name header (`-ifn`), and output/header options (`-o`, `-hdr`). Note that row-filtering options like `-skip`/`-pass` are not in this group.

## Examples

Input `/tmp/sh.csv`:
```
1,2,3
4,5,6
7,8,9
10,11,12
5,5,5
```

Shuffle rows with a fixed seed (reproducible):
```
csvfix shuffle -rs 42 /tmp/sh.csv
```
```
"7","8","9"
"5","5","5"
"4","5","6"
"10","11","12"
"1","2","3"
```

Pick 2 rows at random with the same seed:
```
csvfix shuffle -rs 42 -n 2 /tmp/sh.csv
```
```
"7","8","9"
"5","5","5"
```

Shuffle the values within fields 1, 2 and 3 of every row (rows stay in order):
```
csvfix shuffle -rs 7 -f 1,2,3 /tmp/sh.csv
```
```
"3","1","2"
"6","5","4"
"8","7","9"
"10","11","12"
"5","5","5"
```

Shuffle fields by header name, dropping the header with `-ifn`. Input `/tmp/names.csv`:
```
first,last,sex
George,Elliot,F
Jane,Austen,F
Oscar,Wilde,M
```
```
csvfix shuffle -rs 3 -fn first,last -ifn /tmp/names.csv
```
```
"Jane","Austen","F"
"George","Elliot","F"
"Oscar","Wilde","M"
```
(Here field shuffling left these particular rows' first/last in place; only row order, never order, is affected by `-rs` for field mode — the two-element fields happened to not swap.)

## Notes
- Row shuffling reads the entire input into memory; avoid on very large files.
- A fixed `-rs` seed makes output reproducible across runs and platforms (Fisher–Yates with the library LCG for field mode; seeded RandGen for row mode). Without `-rs`, the seed comes from the clock and output varies each run.
- `-n` only makes sense for row shuffling. In field-shuffle mode (`-f`/`-fn`) every input row is emitted, so `-n` has no effect.
- `-f`/`-fn` field lists are comma-separated (e.g. `1,2,3`), not colon-separated. Field indexes are 1-based; an out-of-range index aborts with an error.
- When using `-fn` without `-ifn`, the header row is shuffled/passed through as data.

## See also
- [sort](sort.md)
- [order](order.md)
