# CSVfix LLM Reference

This folder is a machine-oriented reference for the **CSVfix** command-line tool,
intended for use by LLMs and other automated agents. There is one Markdown file per
CSVfix command, plus [conventions.md](conventions.md) which documents the options,
flags, and behaviours shared across all commands.

## Invocation model

CSVfix processes CSV data through a single named command per invocation:

```
csvfix <command> [options] [files...]
```

Input is read from the named files (or standard input if none are given), the command
transforms it, and the result is written to standard output. Commands can therefore be
chained with shell pipes to build a processing pipeline. Most commands accept the shared
options described in [conventions.md](conventions.md) — including field-selection syntax
and the `-fn` feature for referring to fields by header name rather than position.

See [conventions.md](conventions.md) for the full set of common options and the `-fn`
header-name feature before reading the individual command pages.

## Commands by category

### Field / column operations

- [edit](edit.md): edit fields
- [erase](erase.md): erase fields using regular expressions
- [escape](escape.md): escape special characters
- [eval](eval.md): expression evaluation
- [exclude](exclude.md): exclude fields from output
- [map](map.md): map betwen CSV field values
- [merge](merge.md): merge fields
- [order](order.md): change field order
- [pad](pad.md): pad CSV records to fixed number of fields
- [printf](printf.md): printf-style formatting
- [put](put.md): put literal or env variable into CSV output
- [template](template.md): output via template
- [truncate](truncate.md): truncate CSV records

### Row filtering & selection

- [find](find.md): find rows matching regular expression or range
- [head](head.md): list first CSV records
- [remove](remove.md): remove rows matching regular expression or range
- [tail](tail.md): list last CSV records
- [unique](unique.md): filter duplicate CSV records

### Sorting & ordering

- [rowsort](rowsort.md): in-row sort of CSV fields
- [sequence](sequence.md): add sequence numbers
- [shuffle](shuffle.md): randomly shuffle rows or fields
- [sort](sort.md): sort CSV input on one or more fields

### Reshaping (flatten / pivot / squash / transpose)

- [block](block.md): perform actions on blocks of CSV records
- [flatten](flatten.md): flatten to single row
- [pivot](pivot.md): simple pivot tables
- [squash](squash.md): squash duplicate rows to single row
- [unflatten](unflatten.md): convert single row to multiple rows

### Case & text editing

- [lower](lower.md): convert to lowercase
- [mixed](mixed.md): convert to mixed case
- [rmnew](rmnew.md): remove embedded newlines
- [trim](trim.md): trim leading/trailing spaces
- [upper](upper.md): convert to uppercase

### Numbers, money & dates

- [date_format](date_format.md): perform date formatting
- [date_iso](date_iso.md): convert dates to ISO format
- [money](money.md): format fields as money/currency values
- [number](number.md): convert formatted numeric fields to ordinary numeric
- [timestamp](timestamp.md): add timestamp to CSV data

### Format conversion (DSV / fixed / XML / SQL / multi-line)

- [from_xml](from_xml.md): convert from XML to CSV
- [read_dsv](read_dsv.md): convert DSV (delimiter separated variables) data to CSV
- [read_fixed](read_fixed.md): convert fixed format data to CSV
- [read_multi](read_multi.md): read multi-line data
- [read_xml](read_xml.md): convert XML table to CSV
- [split_char](split_char.md): split using regular expression
- [split_fixed](split_fixed.md): split input using fixed sizes
- [split_regex](split_regex.md): split at character or character type transition
- [sql_delete](sql_delete.md): generate SQL DELETE statements
- [sql_insert](sql_insert.md): generate SQL INSERT statements
- [sql_update](sql_update.md): generate SQL UPDATE statements
- [to_xml](to_xml.md): convert CSV to XML
- [write_dsv](write_dsv.md): convert CSV to DSV format
- [write_fixed](write_fixed.md): convert CSV to fixed format
- [write_multi](write_multi.md): convert CSV to multi-line records

### Inspection & validation

- [ascii_table](ascii_table.md): produce ascii table
- [check](check.md): check CSV record format conforms to CSV standard
- [diff](diff.md): compare two CSV files
- [echo](echo.md): echo input to output
- [file_info](file_info.md): add file information to output
- [stat](stat.md): produce record/field stats for CSV files
- [summary](summary.md): summarise CSV data
- [validate](validate.md): validate CSV data files

### Joining & merging

- [file_merge](file_merge.md): merge multiple sorted CSV files
- [file_split](file_split.md): split into files
- [inter](inter.md): interleave fields from two CSV sources
- [join](join.md): perform relational join

### Generation & misc

- [call](call.md): call function in DLL
- [exec](exec.md): execute external command
- [odbc_get](odbc_get.md): get data from ODBC database
