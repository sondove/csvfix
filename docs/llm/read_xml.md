# csvfix read_xml

> convert XML table to CSV

## Synopsis
```
csvfix read_xml [options] [file ...]
```

## Description
`read_xml` parses each input stream as an XML document, locates the first `<table>` element (searched depth-first from the document root), and converts that HTML-style table into CSV. Each `<tr>` child of the table becomes one CSV row, and each `<td>` child of a row becomes one field whose value is the concatenated text content of that cell. All other tags (including `<th>`, `<thead>`, attributes, and nested non-text elements) are ignored. Output is written to standard output (or to `-o file`).

> AVAILABILITY: This command was REMOVED from CSVfix and is NOT present in this build. Running `csvfix help read_xml` returns `no such command: read_xml`. Its source still exists (`csvfix/src/csved_xml.cpp`) but is not compiled — the project Makefile builds `csved_fromxml.o`, not `csved_xml.o`. Use [`from_xml`](from_xml.md) instead, which the manual describes as "a much improved facility over the original read_xml command, which was removed a while back." The information below is reconstructed from source and is illustrative only; it cannot be run against this binary.

## Options
The built command registered no command-specific flags (the only flag in the source, a `-c`/columns option, is commented out). It accepted only:

- positional `[file ...]` — one or more XML input files. With no file arguments it reads XML from standard input. Each input stream is parsed independently and its first `<table>` is emitted.

## Common options
Also accepts the standard options — see [conventions](conventions.md). In practice only the output option `-o file` is meaningful, since the input is XML rather than CSV (the usual CSV-input options such as `-sep`/`-rsep`, `-ibl`, `-ifn`, `-skip`/`-pass` do not apply to XML parsing). Output quoting follows CSVfix defaults.

## Examples

> The following examples are ILLUSTRATIVE ONLY and were NOT verified against this binary, because `read_xml` is not in the build. They describe the behaviour of the original (removed) command as implemented in `csvfix/src/csved_xml.cpp`.

Input XML (`books.xml`):
```xml
<table>
  <tr><td>title</td><td>author</td></tr>
  <tr><td>Dune</td><td>Herbert</td></tr>
  <tr><td>Neuromancer</td><td>Gibson</td></tr>
</table>
```
Command:
```
csvfix read_xml books.xml
```
Expected output:
```
title,author
Dune,Herbert
Neuromancer,Gibson
```

Input XML with surrounding markup — the first `<table>` is found anywhere in the tree, and `<th>` cells are ignored (only `<td>` is emitted):
```xml
<html><body>
  <h1>Report</h1>
  <table>
    <tr><th>id</th><th>name</th></tr>
    <tr><td>1</td><td>Alice</td></tr>
    <tr><td>2</td><td>Bob</td></tr>
  </table>
</body></html>
```
Command:
```
csvfix read_xml report.xml
```
Expected output:
```
1,Alice
2,Bob
```

## Notes
- NOT IN THIS BUILD. `csvfix help read_xml` errors with `no such command`. Reach for [`from_xml`](from_xml.md) for any XML-to-CSV work.
- Only the FIRST `<table>` in the document is converted; additional tables are ignored.
- Tag matching is case-sensitive for `table`, `tr`, and `td`. Rows built from `<th>` header cells are emitted as empty rows (no `<td>` children) rather than as a header line, so HTML header rows are effectively dropped.
- A `<td>` cell's value is the concatenation of its direct text nodes only; markup inside a cell (e.g. `<b>`, `<a>`) contributes no text in the original implementation.
- Malformed XML, or a document containing no `<table>`, causes the command to abort with an error naming the input file (and, for parse errors, the line number).
- `from_xml` differs substantially: it is path-driven (`-re` selects the record tag, `@`-separated paths), emits parent/child/attribute data (toggled by `-np`/`-nc`/`-na`), and is not limited to HTML `<table>`/`<tr>`/`<td>` structure.

## See also
- [from_xml](from_xml.md) — the supported replacement; converts arbitrary XML to CSV
- [order](order.md) — reorder/select fields after conversion
