# csvfix sequence

> add sequence numbers

## Synopsis
```
csvfix sequence [options] [file ...]
```

## Description
Reads CSV input (from the named files or standard input) and inserts a generated sequence number into each output record. By default numbering starts at 1, increments by 1, and the number is placed in the first field of every row (shifting the existing fields right). You control the start value, the increment or decrement, zero padding, the target field position, and an optional text mask wrapped around the number. Output is written as CSV to standard output (or to the file given with `-o`).

## Options
- `-n start` — starting value for the sequence; must be an integer (default: `1`). May be negative.
- `-i inc` — positive increment between successive numbers; must be an integer, and must not be zero (default: `1`). Mutually exclusive with `-d`. Negative increments are not allowed here — use `-d` instead.
- `-d dec` — decrement between numbers, producing a counting-down sequence; must be a positive integer (default: none). Mutually exclusive with `-i`.
- `-p pad` — width to left-pad the number with leading zeros; must be an integer >= 0 (default: `0`, meaning no padding). Padding is only applied when the number is non-negative.
- `-f pos` — 1-based index of the field at which to insert the sequence number (default: `1`). Must be 1 or greater. If `pos` is beyond the end of the row, the number is appended as the last field.
- `-m mask` — text mask into which the (padded) number is inserted at the `@` marker. The mask must contain exactly one `@`; e.g. `-m 'A@Z'` yields `A1Z`, `A2Z`, ... All other number options (`-n`, `-i`/`-d`, `-p`) are applied before insertion into the mask.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Applicable groups (from the source token `#SMQ,SEP,IBL,IFN,OFL,SKIP,PASS`): smart-quote output `-smq` / `-sqf`; CSV separators `-sep` / `-rsep` / `-osep`; `-ibl` ignore blank input lines; `-ifn` ignore field-name header record; output `-o` and `-hdr`; row filtering `-skip` / `-pass`.

## Examples

Input `/tmp/names.csv`:
```
Charles,Dickens,M
Jane,Austen,F
Herman,Melville,M
```

Add numbers starting at 100, padded to 5 digits, in the first field:
```
csvfix sequence -n 100 -p 5 /tmp/names.csv
```
Output:
```
"00100","Charles","Dickens","M"
"00101","Jane","Austen","F"
"00102","Herman","Melville","M"
```

Append a masked, counting-up id at field 4 (past the original 3 fields, so it lands last):
```
csvfix sequence -f 4 -m 'ID@' /tmp/names.csv
```
Output:
```
"Charles","Dickens","M","ID1"
"Jane","Austen","F","ID2"
"Herman","Melville","M","ID3"
```

Count down from 3 using `-d`:
```
csvfix sequence -d 1 -n 3 /tmp/names.csv
```
Output:
```
"3","Charles","Dickens","M"
"2","Jane","Austen","F"
"1","Herman","Melville","M"
```

## Notes
- The number is *inserted* (not overwritten): existing fields at and after position `-f` shift one place to the right. To replace a field instead, combine with another command or remove the original field afterwards.
- `-i` and `-d` cannot be used together; using either with a zero value is an error. To go negative, use `-d N` (giving step `-N`), not `-i -N` — the help notes negative increments via `-i` do not work.
- Zero padding (`-p`) is applied only when the sequence value is non-negative; negative values are printed without padding.
- A mask without an `@` marker is rejected with an "Invalid sequence number mask" error.
- This command does not support field selection by header name (`-fn`); `-f` is numeric only.
- Filtered rows: with `-pass`, matching records are emitted unchanged (no number added); with `-skip`, matching records are dropped entirely. Skipped/passed rows do not consume a sequence value.
- See sibling commands `file_info` (prepend filename/line number), `put` (insert literal/env values), and `pad` for related field-insertion tasks.

## See also
[file_info](file_info.md), [put](put.md), [pad](pad.md), [order](order.md)
