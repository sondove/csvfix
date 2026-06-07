# csvfix sql_delete

> generate SQL DELETE statements

## Synopsis
```
csvfix sql_delete [options] [file ...]
```

## Description
Reads CSV input and emits one SQL `DELETE` statement per input row to standard output. The selected fields (via `-w`/`-wn`) form the statement's `WHERE` clause, matching `colname = value` pairs joined with `AND`. String values are single-quoted and embedded apostrophes are doubled (SQL escaping, e.g. `O'Brien` becomes `'O''Brien'`). A table name is required via `-t`. There is no field-set clause (unlike `sql_update`); only a `WHERE` clause is generated.

## Options
- `-w fields` — fields to use to build the `WHERE` clause, as a comma-separated list of `index:colname` pairs where `index` is the 1-based CSV field number and `colname` is the database column it maps to (e.g. `-w 1:fname,2:sname`). Required (or use `-wn`).
- `-wn fields` — as `-w`, but the CSV field is identified by header name instead of index, as `name:colname` pairs (e.g. `-wn fname:first_name`). Matched against the first input row. Required (or use `-w`).
- `-t table` — name of the table to delete from. Required.
- `-s sep` — statement separator written after each `DELETE` (default `;`). Accepts escape sequences (e.g. `-s ' GO\n'`).
- `-nq fields` — comma-separated 1-based field indexes whose values are emitted unquoted (e.g. for numeric columns).
- `-qn` — force quoting of `NULL` values.
- `-en` — convert empty CSV fields to SQL `NULL` (rendered as `IS NULL` in the `WHERE` clause).

## Common options
Also accepts the standard options — see [conventions](conventions.md). Applicable groups (from source token `#IBN,SEP,OFL,IFN,SKIP`): CSV separators (`-sep`, `-rsep`, `-osep`), output file (`-o`), header record (`-hdr`), ignore-field-name header (`-ifn`), and row filtering (`-skip`).

## Examples

### 1. WHERE clause from field indexes
Input (`names.csv`):
```
fname,sname
Charles,Dickens
Flann,O'Brien
```
Command:
```
csvfix sql_delete -t mailing -w 1:fname,2:sname names.csv
```
Output:
```
DELETE FROM mailing WHERE fname = 'fname' AND sname = 'sname'
;
DELETE FROM mailing WHERE fname = 'Charles' AND sname = 'Dickens'
;
DELETE FROM mailing WHERE fname = 'Flann' AND sname = 'O''Brien'
;
```
Note: with numeric `-w`, the header row is treated as data. Add `-ifn` to skip it.

### 2. Field-by-name with header skipped, unquoted and NULL columns
Input (`ids.csv`):
```
id,sname
10,
20,Austen
```
Command:
```
csvfix sql_delete -t mailing -w 1:id,2:sname -nq 1 -en -s ' GO\n' -ifn ids.csv
```
Output:
```
DELETE FROM mailing WHERE id = 10 AND sname IS NULL GO
DELETE FROM mailing WHERE id = 20 AND sname = 'Austen' GO
```
Here `-nq 1` leaves the `id` column unquoted, `-en` turns the empty `sname` into `IS NULL`, `-s ' GO\n'` replaces the default `;` separator, and `-ifn` drops the header row.

## Notes
- `-t` is mandatory; omitting it is an error. At least one of `-w`/`-wn` is required.
- `-w` (index) and `-wn` (name) are alternative ways to specify the same `WHERE` fields; use one form.
- The header row is emitted as a `DELETE` statement unless you pass `-ifn` (when using `-w`) — there is no automatic header detection. With `-wn`, you still need `-ifn` to suppress the header data row.
- Empty fields are quoted as `''` unless `-en` is given (then `IS NULL`). Use `-qn` to force `NULL` literals to be quoted.
- Output goes to stdout unless redirected with `-o`.
- This command only generates SQL text; it does not connect to or modify any database. Pipe the output to your SQL client to execute it.

## See also
- [sql_insert](sql_insert.md)
- [sql_update](sql_update.md)
