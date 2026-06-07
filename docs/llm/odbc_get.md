# csvfix odbc_get

> get data from ODBC database

## Synopsis
```
csvfix odbc_get [options] [file ...]
```

## Description
Connects to a database through an ODBC driver, executes a SQL query (or extracts a whole table), and writes the result set to standard output as CSV. The command does **not** read its input streams — any `[file ...]` arguments are ignored as data sources; the rows come entirely from the database. NULL column values are rendered as a configurable string, and optionally the SQL column names can be emitted as a leading CSV field-name header record.

This command is **only available in the Windows build of CSVfix**, because it depends on the Windows ODBC subsystem. It is not present in this (non-Windows) build; running it here yields `no such command: odbc_get`.

## Options
- `-cs cstr` — ODBC connection string (driver-specific; fields separated by `;`, e.g. `DRIVER=...;SERVER=...;DATABASE=...;UID=...;PWD=...`). Mutually exclusive with `-dir`; exactly one of `-cs`/`-dir` is required. Must not be empty.
- `-dir dirname` — Use the Microsoft Text ODBC driver with `dirname` as the default directory, so SQL can be run against `.csv` files in that directory (each file needs a field-name header row). Synthesises a connection string internally. The directory must exist and be non-empty. Mutually exclusive with `-cs`.
- `-sql stmt` — SQL statement to execute to extract data. If `stmt` begins with `@`, the remainder is treated as the path of a file containing the SQL. No check is made that the statement is a `SELECT`, so any SQL can be run. Mutually exclusive with `-tbl`; exactly one of `-sql`/`-tbl` is required. Must not be empty.
- `-tbl table` — Extract all rows from the named table or view; shorthand for `SELECT * FROM [table]`. Mutually exclusive with `-sql`.
- `-ns null` — String used to represent NULL column values (the help text states the default is the empty string; the source defaults the value to `NULL` when the flag is absent).
- `-h` — Output the SQL column names as a leading CSV field-name header record. Takes no argument.

This command takes no positional data files (input streams are not read).

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#SMQ,OFL`, so only output (`-o`, `-osep`/output formatting via the OFL group) and smart-quoting (`-smq`) apply. Row-filtering, header, and input-side options (`-skip`, `-pass`, `-ibl`, `-ifn`, `-hdr`) do not apply, since this command produces rows rather than consuming an input stream.

## Examples

> The following examples are **illustrative** and taken from the CSVfix manual. `odbc_get` is Windows-only and is not present in this build, so its output could not be re-run and verified here.

Extract the `jobs` table from a Microsoft SQL Server `pubs` database on host `dbs`:

```
csvfix odbc_get -cs "server=dbs;driver=sql server;database=pubs;uid=sa;pwd=" -tbl jobs
```

Output:

```
"1","New Hire - Job not specified","10","10"
"2","Chief Executive Officer","200","250"
"3","Business Operations Manager","175","225"
"4","Chief Financial Officer","175","250"
"5","Publisher","150","250"
...
```

Use the `-dir` text driver to run SQL against the CSV files in `tests/data` (querying `army.csv`):

```
csvfix odbc_get -dir tests/data -sql 'select rank, name from army.csv order by rank'
```

Output:

```
"maj","black"
"maj","smith"
"pvt","pink"
"pvt","white"
"sgt","jones"
```

## Notes
- Platform: Windows-only. Not built on this platform; `csvfix help odbc_get` returns `no such command`.
- Exactly one of `-cs` or `-dir`, and exactly one of `-sql` or `-tbl`, must be supplied. Supplying both members of either pair, or neither, is an error.
- `-tbl` wraps the table name in square brackets (`SELECT * FROM [table]`), which handles names containing spaces.
- `-sql @file` reads the SQL statement from a file.
- NULL representation: the help text documents an empty-string default for `-ns`, but the source assigns `NULL` as the fallback when `-ns` is omitted. Pass `-ns ""` explicitly if you need empty strings.
- With `-dir`, queried files must have a `.csv` extension and an initial field-name record.
- Because no statement-type checking is done, `-sql` can execute destructive SQL (UPDATE/DELETE/DROP); use with care.
- Sibling SQL commands write to a database rather than read from it: see `sql_insert`, `sql_update`, `sql_delete`.

## See also
- [sql_insert](sql_insert.md)
- [sql_update](sql_update.md)
- [sql_delete](sql_delete.md)
- [order](order.md)
- [conventions](conventions.md)
