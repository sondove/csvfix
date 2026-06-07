# csvfix stat

> produce record/field stats for CSV files

## Synopsis
```
csvfix stat [options] [file ...]
```

## Description
`stat` reads one or more CSV inputs (named files, or standard input if no file is given) and writes a statistical summary as CSV. By default it emits one summary row per input file (simple stats). With `-fs` it emits one row per field per file (full stats) reporting the inferred type and min/max length of each field. Statistics are computed per file: every input file produces its own row(s), and a counter resets at each file boundary. Counts are of CSV *records*, not lines — a single record may span multiple physical lines when fields contain embedded newlines.

## Options
- `-fs` — produce full per-field stats instead of the default simplified per-file output. No argument. Output is one CSV row per field: `filename,fieldname,type,minlen,maxlen` (see Examples).
- `-fn` — declare that the input begins with a header record listing field names. Only meaningful together with `-fs`: the first record of each file is consumed as field names and used in place of the default `1,2,3,...` positional names. Without `-fs`, `-fn` has no visible effect. No argument. (Note: here `-fn` means "field names header"; it is **not** field-selection-by-name as in some other commands.)

Default (no flags) simple-stats output is one row per file with the columns: `filename,records,min-field-count,max-field-count,maxlen-field-1,maxlen-field-2,...` — a max-length column is appended for each field position seen.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source help token is `#ALL`, so the full standard option set applies, including: output `-o file`; CSV separators `-sep`/`-rsep`/`-osep`; `-ibl` ignore blank input lines; `-smq` smart-quote output; `-sqf fields` force-quote fields; `-hdr s` write string `s` as an output header record; `-ifn` ignore (drop) an input field-name header record.

## Examples

### Example 1 — simple per-file stats (default)
Input `/tmp/stat_names.csv`:
```
Charles,Dickens,M
Jane,Austen,F
Herman,Melville,M
```
Command:
```
csvfix stat /tmp/stat_names.csv
```
Output:
```
"/tmp/stat_names.csv","3","3","3","7","8","1"
```
Meaning: 3 records; min and max field count are both 3; the max lengths of fields 1, 2 and 3 are 7 ("Charles"), 8 ("Melville") and 1 respectively.

### Example 2 — full per-field stats with a header record
Input `/tmp/stat_hdr.csv`:
```
first,last,sex
Charles,Dickens,M
Jane,Austen,F
Herman,Melville,M
```
Command:
```
csvfix stat -fs -fn /tmp/stat_hdr.csv
```
Output:
```
"/tmp/stat_hdr.csv","first","string","4","7"
"/tmp/stat_hdr.csv","last","string","6","8"
"/tmp/stat_hdr.csv","sex","string","1","1"
```
Columns are `filename,fieldname,type,minlen,maxlen`. With `-fn` the first record supplies the field names; without `-fn` they would be `1`, `2`, `3`.

### Example 3 — type inference with numbers and empty values
Input `/tmp/stat_mix.csv`:
```
a,1,
bb,22,x
ccc,333,
```
Command:
```
csvfix stat -fs /tmp/stat_mix.csv
```
Output:
```
"/tmp/stat_mix.csv","1","string","1","3"
"/tmp/stat_mix.csv","2","number","1","3"
"/tmp/stat_mix.csv","3","string","0","1"
```
Field 2 is reported as `number` because every non-empty value is numeric; field 3 has a min length of 0 because of the empty values.

## Notes
- **Three field types**: `empty` (all values empty), `number` (all non-empty values numeric), `string` (anything else). Type inference is a one-way transition — once a field has seen a non-numeric, non-empty value it is `string` and stays `string`. Empty values never change a field's type. A field that is empty in every record reports type `empty`.
- **min/max length** in `-fs` mode are character lengths of the field values; empty values count as length 0.
- **Per-file**: each input file (or `<stdin>` when reading standard input) gets its own row(s); stats do not aggregate across files.
- **`-fn` without `-fs`** is accepted but does nothing observable; `-fn` only affects field naming in full-stats mode.
- **Manual is stale**: the HTML manual shows the simple output as only four columns (`filename,records,min,max`); the current binary additionally appends per-field max-length columns.
- Related sibling command: `summary` produces aggregate functions (sum, average, median, mode, min/max, frequency) over fields; `file_info` reports file-level metadata.

## See also
- [summary](summary.md)
- [file_info](file_info.md)
- [check](check.md)
- [conventions](conventions.md)
