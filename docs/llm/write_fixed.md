# csvfix write_fixed

> convert CSV to fixed format

## Synopsis
```
csvfix write_fixed [options] [file ...]
```

## Description
Reads CSV input (from the named files, or from standard input if none are given) and writes fixed-width, non-CSV text output: one input record becomes one output line, with no field separators or quoting. You list the input fields you want and the column width each should occupy via `-f`. Each selected value is left-justified and space-padded on the right to its width; values longer than the width are truncated. This is the inverse of `read_fixed` and is typically used to feed legacy / mainframe systems that expect fixed-format records.

## Options
- `-f fields` — **required.** Comma-separated list of `field:width` pairs specifying what to output. `field` is the 1-based input column index; `width` is the number of character columns to pad or truncate it to. Both must be positive integers. A field may be listed more than once and in any order (e.g. `-f 3:7,1:1,2:4`). If a referenced field is missing from a record it is treated as an empty string (padded to its width). Example: `-f 1:16,2:16,3:1`.
- `-ru` — output an 80-column ruler line (`123456789 ` repeated 8 times) before the data, to help check column alignment (default: no ruler).

## Common options
Also accepts the standard options — see [conventions](conventions.md). Applicable groups: output `-o`; input CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank input lines; `-smq` smart-quote; `-ifn` ignore (drop) a field-name header record; row filtering `-skip` (and `-pass`).

## Examples

### Pad to fixed widths
Input `/tmp/names.csv`:
```
Charles,Dickens,M
Jane,Austen,F
Herman,Melville,M
```
Command:
```
csvfix write_fixed -f 1:16,2:16,3:1 /tmp/names.csv
```
Output (surname 16 cols, forename 16 cols, sex 1 col; trailing spaces preserved):
```
Charles         Dickens         M
Jane            Austen          F
Herman          Melville        M
```

### Ruler plus truncation
Using the same input, request a ruler and narrow widths so values overflow:
```
csvfix write_fixed -ru -f 1:5,3:3 /tmp/names.csv
```
Output (field 1 truncated to 5 chars, field 3 padded to 3 chars; first line is the ruler):
```
123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 
CharlM  
Jane F  
HermaM  
```
(`Charles` and `Herman` are truncated to 5 characters; `Jane` is padded to 5; the single-character sex field is right-padded to 3 columns.)

## Notes
- `-f` is mandatory; running without it (or with an empty list) is an error.
- Each field spec must be exactly `int:int` with both values non-zero — anything else ("Invalid field specification") errors out.
- Output is plain text, not CSV: there is no field separator and no quoting between columns. Padding and truncation are always right-padding / right-truncation (left-justified values).
- The ruler emitted by `-ru` is fixed at 80 columns regardless of your field widths; it is a visual aid only.
- The `-f` index always refers to numeric input columns. There is no `-fn` (select-by-name) option for this command.
- Without `-ifn`, a header row in the input is treated as ordinary data and written out as a fixed-format line like any other record; pass `-ifn` to drop it.
- Minor cosmetic bug: the binary's own `help write_fixed` text prints `usage: csvfix read_fixed ...` and omits `-ru`; the command name and `-ru` flag described here are correct (confirmed against source and a live run).
- Sibling/related commands: `read_fixed` (fixed-format to CSV), `split_fixed`, and the other output formatters `write_dsv`, `printf` and `template`.

## See also
- [read_fixed](read_fixed.md)
- [split_fixed](split_fixed.md)
- [write_dsv](write_dsv.md)
- [printf](printf.md)
- [template](template.md)
