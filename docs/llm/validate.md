# csvfix validate

> validate CSV data files

## Synopsis
```
csvfix validate -vf rulesfile [options] [file ...]
```

## Description
`validate` checks CSV records against a set of business-rule validations
defined in an external rules file (specified with `-vf`). It reads CSV from the
named files (or standard input) and, by default, writes a human-readable error
report (filename, line number, offending record, and per-field messages) for
every record that fails a rule. **The output of report mode is not CSV.** It
does not check basic CSV syntax — the input must already be syntactically valid
CSV; the rules express business constraints (required fields, value sets,
numeric ranges, lookups against another file, dates, etc.). The `-om` flag can
switch output to emit the passing or failing CSV records instead of a report.

## Options
- `-vf file` — **required.** File containing the validation rules (see "Rules file" below).
- `-om mode` — output mode; one of `report`, `pass`, or `fail` (default: `report`).
  - `report` — writes a non-CSV error report for each failing record.
  - `pass` — writes (as CSV) the records that pass all rules.
  - `fail` — writes (as CSV) the records that fail at least one rule.
- `-ec` — if set, return exit code `2` to the OS when any validation failure is detected; without it the exit code is `0` regardless of failures. (Takes no argument. Present in the binary but not shown in `help validate`.)

This command takes zero or more positional input files; with none it reads standard input.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Applicable
groups (from the help token `#IFN,SEP,OFL,IBL,SKIP`): `-ifn` ignore field-name
header record; CSV separators `-sep`/`-rsep`/`-osep`; output `-o` and `-hdr`;
`-ibl` ignore blank input lines; and `-skip` row filtering.

## Rules file
A text file where lines starting with `#` are comments and blank lines are
ignored. Each rule line has space-separated fields: a rule name, a
comma-separated list of 1-based field numbers to apply it to (or `*` as a
placeholder for rules that do not need one), then any rule-specific parameters.
Available rules:

- `required <fields>` — the listed fields must exist (may be empty). Other rules do not require the field to exist, so use `required` to enforce presence.
- `notempty <fields>` — the listed fields must not be only whitespace.
- `fields * min:max` — row must have between `min` and `max` fields (use equal values for an exact count); no field list needed.
- `length <fields> min:max` — field length must be within `min`..`max`.
- `numeric <fields> [lo:hi ...]` — field must be numeric; optional one-or-more inclusive ranges (e.g. `numeric 1 1:1000 -1:-1`).
- `values <fields> v1 v2 ...` — field must equal one of the listed values (values may be bare, single-, or double-quoted).
- `notvalues <fields> v1 v2 ...` — field must NOT equal any of the listed values.
- `lookup * a:x,b:y lookupfile.csv` — look up current-file fields in another CSV; each pair is `currentfield:lookupfield`.
- `date <fields> 'mask' [from:to]` — field must be a valid date in the given mask (see [date_iso](date_iso.md)); optional ISO date range `YYYY-M-D:YYYY-M-D`.

Multiple rules may fire on the same record, producing multiple report lines.

## Examples

### Example 1 — report mode (default)
Rules file `val_names.txt`:
```
# val_names.txt
required    1,2,3
notempty    1,2,3
values      3       "M" "F"
```
Input `names_data.csv`:
```
John,Smith,M
Jane,,F
Mary,Jones,F
Flann,O'Brien,X
George,Elliot
```
Command:
```
csvfix validate -vf val_names.txt names_data.csv
```
Output (not CSV):
```
names_data.csv (2): Jane,,F
    field: 2 - field is empty
names_data.csv (4): Flann,O'Brien,X
    field: 3 - "X" is invalid value
names_data.csv (5): George,Elliot
    field: 3 - required field missing
```
Exit code is `0` here. Adding `-ec` produces the same output but exit code `2`.

### Example 2 — pass and fail modes (output is CSV)
Same rules and input as Example 1.

```
csvfix validate -om pass -vf val_names.txt names_data.csv
```
```
"John","Smith","M"
"Mary","Jones","F"
```

```
csvfix validate -om fail -vf val_names.txt names_data.csv
```
```
"Jane","","F"
"Flann","O'Brien","X"
"George","Elliot"
```

### Example 3 — numeric range and length rules
Rules file `val_num.txt`:
```
# numeric range and length
numeric  1  1:1000
length   2  2:5
```
Input `nums.csv`:
```
500,abc
2000,toolongvalue
-3,xy
```
Command:
```
csvfix validate -vf val_num.txt nums.csv
```
Output:
```
nums.csv (2): 2000,toolongvalue
    field: 1 - "2000" failed range check
    field: 2 - "toolongvalue" is too long
nums.csv (3): -3,xy
    field: 1 - "-3" failed range check
```

## Notes
- `-vf` is mandatory; running without it is an error ("Need validation file specified by -vf flag").
- An invalid `-om` value (anything other than `report`/`pass`/`fail`) is a fatal error.
- Default exit code is `0` even when records fail; use `-ec` to get exit code `2` on any failure (useful in scripts/CI).
- `report` mode output is plain text, not CSV — do not pipe it into other csvfix commands expecting CSV. `pass`/`fail` modes do emit CSV.
- Field numbers in the rules file are 1-based. This command selects fields only by number (no `-fn` name support).
- It validates business rules, not CSV well-formedness; malformed CSV input is a separate concern.
- Use `-ifn` to skip a field-name header row so it is not validated as data.
- The sibling [check](check.md) command applies a single expression-based test across records; `remove` can strip bad values flagged here.

## See also
- [check](check.md)
- [remove](remove.md)
- [date_iso](date_iso.md)
- [find](find.md)
- [conventions](conventions.md)
