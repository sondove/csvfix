# csvfix from_xml

> convert from XML to CSV

## Synopsis
```
csvfix from_xml [options] [file ...]
```

## Description
`from_xml` converts XML data into CSV records. It does not aim to convert arbitrary XML into arbitrary CSV (that would need a Turing-complete language); instead it walks the XML tree and, for each element whose path matches a record path you give with `-re`, emits one CSV record. By default a record contains the data of the matched element's ancestor (parent) tags, then the matched element itself, then its descendant (child) tags, including attribute values and text content. Input is read from the named XML file(s), or from standard input if no file is given (or the file is `-`); CSV is written to standard output. There is no control over field order within a record — pipe the output through `order` if you need to reorder.

## Options
- `-re path` — **required.** Comma-separated list of tag paths that mark the start of a new CSV record. A path is one or more tag names joined by `@` (e.g. `character` or `author@book@character`). A path matches an element when it is a suffix of that element's full path at a tag boundary, so a bare tag name like `character` matches anywhere it occurs. Use a longer path fragment to disambiguate when the same tag name appears in different places. May not be empty.
- `-ex path` — Comma-separated list of tag paths to exclude from output (same suffix-matching rules as `-re`). Matching elements (and their data) are dropped from the record.
- `-np` — Do not output data from parent (ancestor) tags.
- `-nc` — Do not output data from child (descendant) tags of the matched element.
- `-na` — Do not output attribute data.
- `-ip` — Insert the full `@`-separated path of the matched tag as the first CSV field of each record.
- `-ml sep` — Separator string placed between lines of multi-line text within a single CSV field (default: a single space). Escape sequences in `sep` are interpreted.

Positional arguments are one or more XML input files. With no files, input is read from standard input.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Only the smart-quote group (`-smq` use smart quotes on output, `-sqf fields` force-quote named fields) and `-o file` (write output to a file instead of stdout) apply. This command does **not** accept the CSV-input options (`-sep`/`-rsep`/`-osep`, `-ibl`, `-ifn`, `-hdr`) or the `-skip`/`-pass` row filters, since its input is XML rather than CSV.

## Examples

### Example 1 — basic record extraction
Input `books.xml`:
```xml
<characters>
    <author forename="Charles" surname="Dickens">
        <book title="Bleak House">
            <character>
                <name>Esther Sumerson</name>
                <description>Drippy heroine</description>
            </character>
            <character>
                <name>Inspector Bucket</name>
                <description>Prototype detective</description>
            </character>
        </book>
        <book title="Great Expectations">
            <character>
                <name>Pip</name>
                <description>Deluded ex-blacksmith</description>
            </character>
        </book>
    </author>
    <author forename="Jane" surname="Austen">
        <book title="Emma">
            <character>
                <name>Emma Woodhouse</name>
                <description>Smug Surrey goddess</description>
            </character>
        </book>
    </author>
</characters>
```
Command:
```
csvfix from_xml -re 'character' books.xml
```
Output:
```
"Charles","Dickens","Bleak House","Esther Sumerson","Drippy heroine"
"Charles","Dickens","Bleak House","Inspector Bucket","Prototype detective"
"Charles","Dickens","Great Expectations","Pip","Deluded ex-blacksmith"
"Jane","Austen","Emma","Emma Woodhouse","Smug Surrey goddess"
```
Each record carries the matched `character`'s ancestor data first (author attributes `forename`/`surname`, then the book `title` attribute), then the character's own `name` and `description` text.

### Example 2 — simple table, two columns per row
Input `simple.xml`:
```xml
<table>
  <tr>
    <td>Lucky</td><td>7</td>
  </tr>
  <tr>
    <td>Beast</td><td>666</td>
  </tr>
</table>
```
Command:
```
csvfix from_xml -re 'tr' simple.xml
```
Output:
```
"Lucky","7"
"Beast","666"
```

### Example 3 — drop parent and attribute data
Using `books.xml` from Example 1:
```
csvfix from_xml -re 'character' -np -na books.xml
```
Output:
```
"Esther Sumerson","Drippy heroine"
"Inspector Bucket","Prototype detective"
"Pip","Deluded ex-blacksmith"
"Emma Woodhouse","Smug Surrey goddess"
```

### Example 4 — record path field and exclusion
Using `books.xml` from Example 1:
```
csvfix from_xml -re 'character' -ip -ex 'description' books.xml
```
Output:
```
"characters@author@book@character","Charles","Dickens","Bleak House","Esther Sumerson"
"characters@author@book@character","Charles","Dickens","Bleak House","Inspector Bucket"
"characters@author@book@character","Charles","Dickens","Great Expectations","Pip"
"characters@author@book@character","Jane","Austen","Emma","Emma Woodhouse"
```
`-ip` prepends the matched tag's full path; `-ex 'description'` removes the `description` field.

## Notes
- `-re` is the only required flag; running without it is an error. The value may not be an empty string.
- Path matching (for both `-re` and `-ex`) is suffix-based at tag boundaries: `character` matches `characters@author@book@character`, and a path fragment such as `author@book@character` narrows the match when a tag name is otherwise ambiguous.
- Field order is fixed by document order (parents, then the matched element, then children) and cannot be configured here — pipe through [order](order.md) to rearrange.
- Empty leaf elements (no attributes, no child elements, no non-empty text) contribute an empty field rather than being skipped.
- Multi-line text inside one element is joined with the `-ml` separator (default single space); the parser does not concatenate multi-line strings itself.
- XML entities (e.g. `&amp;`) are decoded in the output values.
- Parse errors abort with a message giving the file, line, and reason.
- This is the inverse of [to_xml](to_xml.md); the `to_xml` output round-trips through `from_xml` for typical data.

## See also
- [to_xml](to_xml.md)
- [order](order.md)
