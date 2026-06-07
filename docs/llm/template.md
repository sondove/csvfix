# csvfix template

> output via template

## Synopsis
```
csvfix template [options] [file ...]
```

## Description
Reads CSV input and formats each record through a textual template, writing the result (which is NOT CSV) to standard output. The template is supplied either as a file (`-tf`) or as an inline string (`-ts`); exactly one of the two is required. Placeholders of the form `{N}` (1-based field index) are replaced by the corresponding field value; placeholders of the form `{@ expr }` evaluate an expression in the CSVfix expression language against the record. With `-fn`, the output of each input record is written to its own file whose name is itself produced from a template, instead of being concatenated to one stream.

## Options
- `-tf file` — read the template text from `file`. Mutually exclusive with `-ts`. (Note: the binary's `help template` text misprints this as `-ft`; the working flag is `-tf`.)
- `-ts str` — use `str` directly as the template (instead of a file). Mutually exclusive with `-tf`. C-style escapes such as `\n` and `\t` are interpreted; literal braces must be backslash-escaped (`\{`, `\}`).
- `-fn ftpl` — write each input record to a separate output file. `ftpl` is itself a template (e.g. `out/{1}.txt`) expanded per record to produce the file name; the record's templated text becomes that file's contents. Overwrites existing files. When omitted, all output goes to one stream (stdout or `-o`).

Template placeholder syntax (applies to `-tf`, `-ts`, and `-fn` strings):
- `{N}` — replaced by field N (1-based). If field N does not exist in the record it is replaced by the empty string. `{0}` or non-integer indexes are errors.
- `{@ expr }` — evaluate `expr` in the expression language; record fields are available as positional params `$1`, `$2`, ... (e.g. `{@ $1 + $2 }`).
- `\{`, `\}` — literal braces. `\n`, `\t` — newline/tab. Any other `\x` yields the literal `x`.

## Common options
Also accepts the standard options — see [conventions](conventions.md). Applicable groups (from the source token `#IBL,SEP,IFN,OFL,SKIP`): `-ibl` (ignore blank input lines), CSV separators `-sep`/`-rsep`/`-osep`, `-ifn` (ignore field-name header record), output `-o`/`-hdr`, and row filtering `-skip`. Note there is no `-f`/`-fn` field-by-name selection here: in this command `-fn` means the output file-name template.

## Examples

Input `names.csv`:
```
Charles,Dickens,M
Jane,Austen,F
Herman,Melville,M
```

Template file `names.tpl`:
```
Name: {1} {2}
Sex: {3}
```

Command (template from a file):
```
csvfix template -tf names.tpl names.csv
```
Output:
```
Name: Charles Dickens
Sex: M
Name: Jane Austen
Sex: F
Name: Herman Melville
Sex: M
```

Command (inline template string, with `\n`):
```
csvfix template -ts '{2}, {1} ({3})\n' names.csv
```
Output:
```
Dickens, Charles (M)
Austen, Jane (F)
Melville, Herman (M)
```

Command (expression placeholder). Input `nums.csv`:
```
10,20
3,4
```
```
csvfix template -ts '{1} + {2} = {@ $1 + $2}\n' nums.csv
```
Output:
```
10 + 20 = 30
3 + 4 = 7
```

Command (one output file per record via `-fn`):
```
csvfix template -tf names.tpl -fn 'out/{1}.txt' names.csv
```
This writes `out/Charles.txt`, `out/Jane.txt`, `out/Herman.txt` (no stdout output). Contents of `out/Jane.txt`:
```
Name: Jane Austen
Sex: F
```

## Notes
- Exactly one of `-tf` or `-ts` must be given; supplying both is an error, supplying neither is an error. An empty template (empty file or empty string) is rejected.
- The output is plain text, not CSV — quoting/escaping is whatever you put in the template.
- A template file is read line by line and each line gets a trailing `\n` appended, so file-based templates always end their per-record block with a newline. Inline `-ts` templates do not get an implicit trailing newline; add `\n` yourself.
- Out-of-range `{N}` placeholders silently produce empty text rather than an error; only `{0}`, negative, or non-integer/non-`@` placeholders raise an error, as does an unterminated `{`.
- With `-fn`, the file-name template is expanded per record using the same placeholder rules; the target directory must already exist (the command opens the file but does not create directories). Existing files are overwritten. The output is one file per record, so `-o` and stdout are not used.
- `-ifn` drops the first input row (treated as field-name header) so it is not formatted as a data record.

## See also
[printf](printf.md), [order](order.md), [put](put.md), [echo](echo.md)
