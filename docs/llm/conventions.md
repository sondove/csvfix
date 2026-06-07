# CSVfix — Conventions & Concepts

Shared reference for the CSVfix CLI. The per-command docs in this directory
link here for the invocation model, the generic options every command shares,
field-list syntax, header-name selection (`-fn`), and pointers to the
expression / regex / fixed-format mini-languages.

## Invocation model

```
csvfix <command> [options] [file ...]
```

- The first argument is always the command name (e.g. `csvfix order -f 2,1 in.csv`).
- **Input:** one or more input files may be listed after the options. If no files
  are given, CSVfix reads from **standard input**. The token `-` used as a
  filename also means standard input.
- **Output:** results are written to **standard output** by default; use `-o file`
  to redirect to a file instead.
- Multiple input files are processed in sequence and concatenated into one
  output stream (commands like `join`, `file_merge` and `diff` treat their
  multiple inputs specially — see those docs).
- Run `csvfix help` for the command list, or `csvfix help <command>` for a
  command's own flags. Flag values that contain spaces or expression/regex
  syntax must be quoted for your shell (single quotes on Linux/macOS, double
  quotes on Windows `cmd.exe`).

## Generic (standard) options

Most commands accept a common set of generic flags in addition to their own.
These are registered for every command in `Command::CheckFlags()`
(`csvfix/src/csved_command.cpp`); the flag spellings are defined in
`csvfix/inc/csved_strings.h`. The following are **always** accepted:

| Flag        | Argument | Description |
|-------------|----------|-------------|
| `-o file`   | file     | Write output to *file* instead of standard output. |
| `-sep s`    | char(s)  | Input CSV field separator (`s` replaces the default comma; for input parsing). |
| `-rsep s`   | char(s)  | As `-sep`, but **retain** the separator on output as well. |
| `-osep s`   | char(s)  | Output field separator. |
| `-ibl`      | —        | Ignore blank input lines. |
| `-smq`      | —        | Smart quoting on output: only quote fields that need it (contain commas, quotes, etc.), instead of quoting every field. No effect on commands that don't emit CSV. |
| `-sqf fields` | list   | Force the listed fields to always be quoted on output (a [field list](#field-lists)). |
| `-ifn`      | —        | Ignore the field-name (header) record: treat the first input row as data rather than column names. |
| `-hdr s`    | string   | Write the string *s* as a header record before the output rows. |
| `-seed n`   | integer  | Seed the random number generator used by the expression-language `random()` function. |

`-skip` and `-pass` are accepted **only** by commands whose help opts into them
(see token table below):

| Flag         | Argument   | Description |
|--------------|------------|-------------|
| `-skip expr` | expression | If *expr* (an [expression](#expression-language)) is true for an input row, discard it — the row is not processed by the command and produces no output. |
| `-pass expr` | expression | If *expr* is true for an input row, output the row **as-is**, bypassing the command's processing. |

### Help group tokens (`#`)

In the source, each command's help text may end with a `#`-delimited section
listing which generic groups it supports. CSVfix uses these tokens to append
the matching generic-flag help. They are a documentation/help mechanism — the
output flags above are wired up for all commands regardless — but they tell you
which generic options a given command meaningfully honours:

- `ALL` — shorthand expanding to `IBL,IFN,SMQ,OFL,SEP`.
- `SEP` — separator group: `-sep`, `-rsep`, `-osep`, **and** `-hdr`.
- `SMQ` — smart-quote group: `-smq` **and** `-sqf`.
- `IBL` — `-ibl` (ignore blank lines).
- `IFN` — `-ifn` (ignore field-name/header record).
- `OFL` — `-o` (output file).
- `SKIP` — `-skip` (also enables registration of `-skip`/`-pass`).
- `PASS` — `-pass`.

## Field lists

Many flags (notably `-f`) take a **comma-separated list** of 1-based field
indexes. Conventions:

- Fields are numbered from **1** (not 0).
- Plain list: `1,2,3,4` — fields 1, 2, 3 and 4.
- **Ranges** use a colon: `1:3` means fields 1, 2, 3.
- Ranges may be **descending**: `3:1` means fields 3, 2, 1 (useful for
  reversing order).
- Lists may mix singletons and ranges: `1,4:6,2`.
- No spaces are allowed before or after the commas or colons.
- Some flags use colon-separated **pairs**, e.g. `1:foo,2:bar` (the meaning of
  the pair depends on the command).
- Where a command requires a field-list parameter positionally but does not
  actually need one, an asterisk `*` is used as a placeholder.

## Selecting fields by header name (`-fn`)

This fork adds **`-fn`** to every command that selects columns with `-f`. It
lets you name columns by their header instead of their position.

- `-fn` takes a comma-separated list of **header names** (e.g. `-fn surname,age`).
  Names are matched against the first input record (the header row),
  **case-insensitively** (`-fn AGE` == `-fn age`).
- `-f` is **numeric-only**: it accepts indexes/ranges and rejects names.
  Use `-fn` for names.
- `-f` and `-fn` are **mutually exclusive** — supply exactly one of them
  (commands that also have `-rf`/`-xf` variants require exactly one of the set).
- When `-fn` is used, the header row **passes through to the output** by default
  (e.g. `exclude -fn age` still emits a header, minus `age`). Add `-ifn` to drop
  the header from the output.
- `-fn` also accepts a plain index where a name would go, so it degrades to
  numeric behaviour if you pass a number.

### Command-specific `-fn` forms

Commands whose `-f` carries extra syntax keep that syntax under `-fn`:

- **`sort -fn name:AN`** — header name plus the per-field sort qualifiers
  (e.g. `:A`/`:D` direction, `:N`/`:S` numeric/string), as for `sort -f`.
- **`rowsort -fn a,b,c`** — name list, same form as its `-f`.
- **`squash -fn key`**, **`erase -fn col`** — name list.
- **`join -fn left:right`** — `left` resolves against the left stream's header,
  `right` against the right stream's header (e.g. `join -fn custid:id`).
- **`inter -fn Lname,Rname`** — names resolved per source stream
  (e.g. `inter -fn Lname,Rcity`).
- **`sql_insert -fn name:dbcol`** — CSV header name mapped to a DB column;
  **`sql_delete -wn key:dbcol`** — likewise for the WHERE-key mapping (`-wn`).
- **`pivot`** — its `-r`, `-c` and `-f` each accept **either** a header name or
  an index.

### Exception

- **`fixed`** (`read_fixed`/`write_fixed`) has **no `-fn`**: fixed-width input
  has no header record to resolve names against.

## Mini-languages (pointers)

Summaries of the languages used by various flags; see the manual
(`docs/index.html`, *Terminology* section) for full detail.

### Expression language

A small expression-based language used by `-skip`/`-pass`, and by the `eval`,
`edit` and related commands. All values are strings (denoted by single or
double quotes) but can be treated as numbers. CSV fields are available as
positional parameters `$1`, `$2`, … (and helper vars for the current line/file/
field-count). Supports arithmetic, comparison (`<`, `>`, `==`, `!=`/`<>`),
boolean logic, and built-in functions (`if()`, `random()`, `match()`, `find()`,
`round()`, `isint()`, string functions, etc.). Always quote expressions on the
command line so the shell does not interpret `$n`.

### Regular expressions

Used by the `find`, `remove`, `erase` and `split_regex` commands (and the
expression-language `match()` function) to match field contents. `find` outputs
matching rows, `remove` outputs non-matching rows (like `grep -v` but
CSV-aware). Multiple `-e` expressions OR together. Case-insensitive and
literal-string variants exist (`-ei`, `-s`, `-si`).

### Fixed-format data

Fixed-format files place each field at a fixed character position/width on a
line (rather than delimiting with separators). `read_fixed` converts
fixed-format input into CSV; `write_fixed` produces fixed-format output from
CSV. Field specs are given as `position:width` (read) or `field:width` (write)
lists, e.g. `write_fixed -f 1:16,2:16,3:1` writes field 1 in 16 columns, field 2
in 16, field 3 in 1. These commands have no header/`-fn` support.
