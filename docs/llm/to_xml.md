# csvfix to_xml

> convert CSV to XML

## Synopsis
```
csvfix to_xml [options] [file ...]
```

## Description
Reads CSV input (from the named files or standard input) and writes XML to standard output (or to the file given by `-o`). It works in two modes. With no specification file it emits a simple XHTML `<table>`, mapping each CSV record to a `<tr>` and each field to a `<td>`. With `-xf file` it reads an XML *specification file* and produces customised, tree-structured XML, grouping input rows into nested tags. Field values placed into attributes and text elements have XML special characters escaped to entities (e.g. `&` becomes `&amp;`), unless a `cdata` element is used.

## Options
- `-xf file` — XML specification file describing the output tree. If omitted, an XHTML `<table>` is produced instead. See "Specification file" below.
- `-in ind` — indentation to use. `ind` is either a non-negative integer (number of spaces per level) or the literal word `tabs` (indent using one tab per level). (default: `4` spaces)
- `-et` — always output explicit end tags. Without it, a tag that has no child tags is written as a self-closing empty element `<tag />`; with it the tag is written as `<tag></tag>`. (default: off; affects spec-file mode only — table mode always uses explicit `</td>`/`</tr>` tags)

Positional arguments are input CSV files; if none are given, input is read from standard input.

## Specification file
The `-xf` file is a line-oriented, indentation-structured description of the output XML. Lines whose first non-whitespace character is `#`, and blank lines, are ignored. Indentation (leading whitespace) determines parent/child nesting: a more-deeply-indented line is a child of the line above it. Exactly one root tag is required (it must be at indent 0). Each non-comment line is one of:

- `tag NAME [group F[,F...]] [attrib NAME F] [attrib NAME F]...` — emit an element named `NAME`. `group` lists 1-based field numbers; consecutive input rows sharing the same values in those fields are grouped under one instance of this tag (input need not be sorted, but rows with equal group values that are not adjacent produce separate elements). With no `group`, the tag uses the first row of the parent's grouping. Each `attrib NAME F` adds an attribute `NAME` whose value comes from 1-based field `F`.
- `text F` — emit the (entity-escaped) value of 1-based field `F` as the text content of the enclosing tag. Text elements cannot have child tags.
- `cdata F` — like `text` but wraps field `F` in a `<![CDATA[ ... ]]>` section and does not escape it.

## Common options
Also accepts the standard options — see [conventions](conventions.md). From the source generic-flag token `#IBL,IFN,OFL,SKIP`, the applicable groups are: `-o file` output redirection, `-ibl` ignore blank input lines, `-ifn` ignore (drop) the field-name header record, and `-skip t` skip rows where expression `t` is true (skip filtering applies in spec-file mode). Standard CSV separator options (`-sep`/`-osep` etc.) also apply to input parsing.

## Examples

### 1. Default XHTML table
Input `tx.csv`:
```
Name,Age,City
Alice,30,London
Bob,25,Paris
```
Command:
```
csvfix to_xml tx.csv
```
Output:
```
<table>
    <tr>
        <td>Name</td><td>Age</td><td>City</td>
    </tr>
    <tr>
        <td>Alice</td><td>30</td><td>London</td>
    </tr>
    <tr>
        <td>Bob</td><td>25</td><td>Paris</td>
    </tr>
</table>
```
Adding `-ifn` drops the header row so it is not turned into a `<tr>`:
```
csvfix to_xml -ifn tx.csv
```
```
<table>
    <tr>
        <td>Alice</td><td>30</td><td>London</td>
    </tr>
    <tr>
        <td>Bob</td><td>25</td><td>Paris</td>
    </tr>
</table>
```

### 2. Tree-structured XML with a specification file
Input `books.csv`:
```
Dickens,Charles,Bleak House,Esther Sumerson,Drippy heroine
Dickens,Charles,Great Expectations,Pip,Deluded ex-blacksmith
Austen,Jane,Emma,Emma Woodhouse,Smug Surrey goddess
```
Specification file `books.xsp` (note the lines are indented with tabs):
```
# create XML describing some fictional characters
tag characters
	tag author group 1,2 attrib forename 2 attrib surname 1
		tag book group 3 attrib title 3
			tag character group 4
				tag name
					text 4
				tag description
					text 5
```
Command:
```
csvfix to_xml -xf books.xsp books.csv
```
Output:
```
<characters>
    <author forename="Charles" surname="Dickens">
        <book title="Bleak House">
            <character>
                <name>
                    Esther Sumerson
                </name>
                <description>
                    Drippy heroine
                </description>
            </character>
        </book>
        <book title="Great Expectations">
            <character>
                <name>
                    Pip
                </name>
                <description>
                    Deluded ex-blacksmith
                </description>
            </character>
        </book>
    </author>
    <author forename="Jane" surname="Austen">
        <book title="Emma">
            <character>
                <name>
                    Emma Woodhouse
                </name>
                <description>
                    Smug Surrey goddess
                </description>
            </character>
        </book>
    </author>
</characters>
```

## Notes
- Field references in the spec file (`group`, `attrib`, `text`, `cdata`) are 1-based.
- Grouping is based on *adjacency*: rows are grouped only while consecutive group-key values match. To collect scattered rows under one element, sort the input first (e.g. with `sort`).
- Out-of-range field indexes produce empty attribute/text values rather than an error.
- `-et` only changes empty-element rendering in spec-file mode; tags with text or child tags always get explicit end tags. The XHTML table mode is unaffected by `-et`.
- The command cannot combine multiple input fields into one value; preprocess with `merge`, `edit`, etc. to shape the data first.
- The XHTML table mode emits only `<table>`/`<tr>`/`<td>` markup — no surrounding HTML document, headers, or `<th>`.
- [from_xml](from_xml.md) performs the reverse conversion (XML to CSV); the XML produced by `to_xml` can be read back by `from_xml`.

## See also
- [from_xml](from_xml.md)
- [ascii_table](ascii_table.md)
- [merge](merge.md)
- [edit](edit.md)
- [order](order.md)
