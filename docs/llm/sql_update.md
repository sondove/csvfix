# csvfix sql_update

> generate SQL UPDATE statements

## Synopsis
```
csvfix sql_update [options] [file ...]
```

## Description
Reads CSV input (from the named files or standard input) and writes one SQL `UPDATE` statement per input row to standard output. The fields named by `-f`/`-fn` become the `SET` clause (column = value), and the fields named by `-w`/`-wn` become the `WHERE` clause (column = value joined with `AND`). Both `-t` and `-w`/`-wn` are required: the command refuses to generate an `UPDATE` without a `WHERE` clause for safety. Text values are single-quoted with embedded apostrophes doubled (`O'Brien` → `'O''Brien'`); numeric values are quoted unless excluded with `-nq`.

## Options
- `-t table` — name of the table to update. Required.
- `-f fields` — comma-separated `index:colname` pairs (1-based field index) used to build the `SET` clause. Mutually exclusive with `-fn`. Required (must specify SET columns).
- `-fn fields` — as `-f`, but the CSV field is selected by header name: `name:colname`. Mutually exclusive with `-f`.
- `-w fields` — comma-separated `index:colname` pairs (1-based field index) used to build the `WHERE` clause. Mutually exclusive with `-wn`. Required.
- `-wn fields` — as `-w`, but the CSV field is selected by header name: `name:colname`. Mutually exclusive with `-w`.
- `-s sep` — statement separator written after each statement (default: `;`).
- `-nq fields` — comma-separated 1-based field indexes whose values are written unquoted (e.g. numeric columns).
- `-qn` — force quoting of NULL values.
- `-en` — convert empty CSV fields to SQL `NULL`.

## Selecting fields by name
`-f`/`-w` take 1-based numeric field indexes in `index:colname` form (the CSV field index on the left, the SQL column name on the right). `-fn`/`-wn` take the same form but the left side is a header name matched case-insensitively against the first input row: `-fn price:price`, `-wn id:product_id`. `-f` and `-fn` are mutually exclusive, as are `-w` and `-wn`. When using the named forms, pass `-ifn` so the header row is not itself emitted as an UPDATE statement.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#IBN,SEP,OFL,IFN,SKIP`: ignore-blank-lines (`-ibl`), CSV separators (`-sep`/`-rsep`/`-osep`), output file (`-o`), ignore field-name header (`-ifn`), header record (`-hdr`), and row filtering (`-skip`).

## Examples

### 1. SET from one field, WHERE from two

Input `/tmp/people.csv`:
```
Charles,Dickens
Jane,Austen
Flann,O'Brien
```
The third field needed for the SET clause is added with `pad`, then piped in:
```
csvfix pad -n 3 -p Y /tmp/people.csv | \
csvfix sql_update -t mailing -f 3:need_mail -w 1:fname,2:sname
```
Output:
```
UPDATE mailing SET need_mail = 'Y' WHERE fname = 'Charles' AND sname = 'Dickens'
;
UPDATE mailing SET need_mail = 'Y' WHERE fname = 'Jane' AND sname = 'Austen'
;
UPDATE mailing SET need_mail = 'Y' WHERE fname = 'Flann' AND sname = 'O''Brien'
;
```
Note the apostrophe in `O'Brien` is doubled automatically.

### 2. Field-by-name selection with `-fn`/`-wn`, unquoted numeric column

Input `/tmp/prices.csv` (has a header row):
```
id,price,instock
101,9.99,1
102,14.50,0
```
Command (`-ifn` drops the header so it is not turned into a statement; `-nq 3` leaves field 3 unquoted):
```
csvfix sql_update -t product -fn price:price,instock:in_stock -wn id:product_id -nq 3 -ifn /tmp/prices.csv
```
Output:
```
UPDATE product SET price = '9.99', in_stock = 1 WHERE product_id = '101'
;
UPDATE product SET price = '14.50', in_stock = 0 WHERE product_id = '102'
;
```

## Notes
- A `WHERE` clause is mandatory: omitting `-w`/`-wn` errors out. This is deliberate to avoid generating an UPDATE that touches every row.
- `-t` and a SET specification (`-f`/`-fn`) are also required; missing them produces an error.
- Every referenced field index/name must exist in each input row, otherwise the command errors (e.g. `ERROR: Required field 3 missing from input`).
- `-nq` takes field indexes (the CSV field position), not SQL column names.
- `-en` converts empty input fields to the literal `NULL`; combine with `-qn` to quote those NULLs instead.
- When using named (`-fn`/`-wn`) selection, the first input row is treated as the header; pass `-ifn` so it is consumed as the name map rather than emitted as a statement.

## See also
- [sql_insert](sql_insert.md)
- [sql_delete](sql_delete.md)
- [pad](pad.md)
