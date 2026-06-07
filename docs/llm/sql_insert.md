# csvfix sql_insert

> generate SQL INSERT statements

## Synopsis
```
csvfix sql_insert [options] [file ...]
```

## Description
Reads CSV input (from the named files or standard input) and writes a SQL `INSERT INTO` statement for each input row to standard output. You map CSV fields to SQL columns with `-f`/`-fn`; only the mapped fields are output and column names are mandatory. String values are wrapped in single quotes and embedded apostrophes are SQL-escaped automatically (e.g. `O'Brien` becomes `'O''Brien'`). Each statement is followed by a separator (default newline + `;` + newline). The generated SQL is text only — it is not executed against any database; pipe or save it and run it with your own SQL client.

## Options
- `-t table` — name of the table to insert into. Required.
- `-f fields` — comma-separated list of `index:colname` pairs selecting the CSV fields and naming their target SQL columns; `index` is 1-based. Column names are required (a bare `index` without `:colname` is rejected). Fields not listed are excluded. Mutually exclusive with `-fn`.
- `-fn fields` — as `-f`, but the field part is a CSV header name (`name:dbcol`) matched against the first input row. Mutually exclusive with `-f`.
- `-s sep` — statement separator appended after each INSERT (default: `\n;\n`, i.e. a newline, a semicolon, then a newline). Escape sequences like `\n` are interpreted.
- `-nq fields` — comma-separated list of (1-based) positions, among the selected output columns, that should NOT be single-quoted (e.g. dates/numbers the database cannot implicitly convert from a quoted string).
- `-qn` — force quoting of the special value `NULL` (by default a `NULL` field value, in any case, is emitted unquoted).
- `-en` — convert empty CSV fields to `NULL`.

Positional arguments are zero or more input files; with none, input is read from standard input.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row. `-f` and `-fn` are mutually exclusive, and supplying both is an error. For sql_insert each spec is `field:dbcol`, so the named form looks like `-fn name:dbcol` (e.g. `-fn name:emp_name,hired:hire_date`), where the left side is a CSV header name and the right side is the SQL column name. The header row is read and treated like data (it would produce its own INSERT) unless you also pass `-ifn` to drop it.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Generic groups in effect (`#IBN,SEP,OFL,IFN,SKIP`): `-ibl` ignore blank input lines; CSV separators `-sep`/`-rsep`/`-osep`; output `-o file`; `-ifn` ignore (drop) the field-name header record; and `-skip`/`-pass` row filtering. There is no `-hdr` for this command.

## Examples

Basic insert mapping field 1 to `fname` and field 2 to `sname`:

Input (`names.csv`):
```
Charles,Dickens
Jane,Austen
Flann,O'Brien
```
Command:
```
csvfix sql_insert -t people -f 1:fname,2:sname names.csv
```
Output:
```
INSERT INTO people ( fname, sname ) VALUES( 'Charles', 'Dickens')
;
INSERT INTO people ( fname, sname ) VALUES( 'Jane', 'Austen')
;
INSERT INTO people ( fname, sname ) VALUES( 'Flann', 'O''Brien')
;
```

Selecting by header name, dropping the header (`-ifn`), leaving the 2nd output column unquoted (`-nq 2`), and turning empty fields into NULL (`-en`):

Input (`emp.csv`):
```
id,name,hired,salary
1,Alice,2020-01-15,
2,Bob,2021-06-30,50000
```
Command:
```
csvfix sql_insert -t emp -fn name:emp_name,hired:hire_date,salary:pay -nq 2 -en -ifn emp.csv
```
Output:
```
INSERT INTO emp ( emp_name, hire_date, pay ) VALUES( 'Alice', 2020-01-15, NULL)
;
INSERT INTO emp ( emp_name, hire_date, pay ) VALUES( 'Bob', 2021-06-30, '50000')
;
```

## Notes
- Column names are mandatory: every entry in `-f`/`-fn` must include a `:colname`, otherwise the command errors with "Need column names". This is intentional, to make the generated SQL safer.
- All referenced fields must exist in each input row; a missing field is a fatal error ("Required field N missing from input"), unlike most csvfix commands which tolerate short rows.
- `-nq` positions are 1-based and count the SELECTED output columns (in the order given to `-f`/`-fn`), not the original CSV field indexes.
- A field whose value is the literal `NULL` (any case) is emitted unquoted by default; use `-qn` to quote it. `-en` turns empty fields into that `NULL` token.
- The default separator embeds a leading newline, so each statement and its trailing `;` appear on separate lines. Override with `-s` (e.g. `-s '\n;\nCOMMIT\n;\n'`).
- Sibling commands `sql_update` and `sql_delete` share the same flags plus `-w`/`-wn` for building the WHERE clause.

## See also
- [sql_update](sql_update.md)
- [sql_delete](sql_delete.md)
- [order](order.md)
