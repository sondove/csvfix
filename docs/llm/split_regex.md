# csvfix split_regex

> split at character or character type transition

## Synopsis
```
csvfix split_regex [options] [file ...]
```

## Description
`split_regex` splits a single field of each input CSV record into new sub-fields using a regular expression. The regex is matched against the target field; each **bracketed capture group** in the regex becomes one new field, inserted into the record at the position of the original field. Reads CSV from the named files (or standard input) and writes the transformed CSV to standard output. By default the original field is discarded; the new fields are whatever the capture groups matched.

Note: CSVfix uses a POSIX **basic** regular-expression dialect, so capture groups are written with **escaped** parentheses `\( ... \)`. Bare `(` and `)` are literal characters and produce no captures. If the regex contains no capture group, no sub-fields are produced and the target field is simply removed (unless `-k` is given).

## Options
- `-f field` — 1-based numeric index of the field to split. Required unless `-fn` is used (mutually exclusive with `-fn`).
- `-fn name` — header name (from the first input row, matched case-insensitively) of the field to split. Mutually exclusive with `-f`.
- `-r regex` — regular expression used to perform the split. **Required.** Capture groups (`\( ... \)`) define the output fields.
- `-ic` — ignore case when matching the regular expression (default: case-sensitive).
- `-k` — retain the field being split in the output, appended after the new sub-fields at the split position (default: the original field is discarded).

The matched field must be selected by exactly one of `-f` / `-fn`; omitting both is an error (`Need field specified by -f or -fn flag`). Omitting `-r` is an error (`Required option -r missing`).

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#ALL,SKIP,PASS`, so this command supports the full standard set: output `-o`, CSV separators `-sep`/`-rsep`/`-osep`, `-ibl` (ignore blank input lines), `-smq` (smart-quote output), `-sqf` (force-quote fields), `-hdr` (write a header record), `-ifn` (ignore/drop the field-name header row), and `-skip`/`-pass` row filtering.

## Examples

### Split a code into letters and digits with two capture groups
Input (`sr1.csv`):
```
"abc123","x"
"def456","y"
```
Command:
```
csvfix split_regex -f 1 -r '\([a-z]*\)\([0-9]*\)' sr1.csv
```
Output:
```
"abc","123","x"
"def","456","y"
```

### Keep the original field with -k
Same input. Command:
```
csvfix split_regex -f 1 -k -r '\([a-z]*\)\([0-9]*\)' sr1.csv
```
Output:
```
"abc","123","abc123","x"
"def","456","def456","y"
```

### Select the field by header name and drop the header
Input (`sr3.csv`):
```
code,tag
abc123,x
def456,y
```
Command:
```
csvfix split_regex -fn code -ifn -r '\([a-z]*\)\([0-9]*\)' sr3.csv
```
Output:
```
"abc","123","x"
"def","456","y"
```
(Without `-ifn` the header row is also matched and split like a data row, e.g. `"cod","","tag"`.)

## Notes
- **Capture groups, not split-on-delimiter.** Unlike `split_char` (which splits at a separator), `split_regex` extracts each `\( ... \)` group as a new field. To capture the whole match as one field, wrap the entire pattern in one group.
- **Basic (POSIX BRE) syntax.** Use `\(` `\)` for groups, `\{ \}` for counted repeats, etc. A bare `(...)` is literal. Common classes like `[a-z]`, `[0-9]`, `.`, `*`, `^`, `$` work as in BRE.
- **No match / no groups:** if the pattern does not match, or matches but contains no capture group, the target field is removed and replaced by zero new fields (so the record loses a column) unless `-k` keeps the original. A group that legally matches the empty string yields an empty field `""`.
- **Header handling:** when using `-fn`, the header row is transformed like any other row unless you pass `-ifn` to drop it. `-f` ignores header names entirely.
- Sibling commands: [split_char](split_char.md) splits on a character or alpha/numeric transition; [split_fixed](split_fixed.md) splits at fixed positions/lengths. To recombine fields use `merge`.

## See also
- [split_char](split_char.md)
- [split_fixed](split_fixed.md)
- [find](find.md)
- [edit](edit.md)
