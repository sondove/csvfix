# csvfix call

> call function in DLL

## Synopsis
```
csvfix call [options] [file ...]
```

## Description
`call` invokes a function exported from a Windows DLL once per input CSV row, passing selected fields to the function and appending any fields the function returns to the row before writing it out. It is conceptually similar to [`exec`](exec.md) but, because it loads the DLL in-process rather than spawning an external program, it can be much faster. The function's return value controls per-row behaviour: `0` means write the row (with any returned fields appended), a negative value means silently drop the row from output, and a positive value aborts with an error.

**Availability:** This command is Windows-only and is marked experimental — its interface may change. It is implemented in `csvfix/src/csved_call.cpp` using `LoadLibrary`/`GetProcAddress` and `windows.h`, so it is not compiled into non-Windows builds. The binary at `/Users/sondove/git/csvfix/csvfix/bin/csvfix` reports `no such command: call`, so the examples below are illustrative (taken from the source/manual) and were not executed.

## Options
- `-fnc name` — name of the exported function to call (required).
- `-dll name` — path/filename of the DLL containing the function (required).
- `-f fields` — comma-separated 1-based numeric field indexes to pass to the function. Default: all fields are passed.
- `-fn names` — fields to pass to the function, selected by header name (see below). Mutually exclusive with `-f`.
- `-bs size` — size in kilobytes of the buffer used to receive returned fields from the DLL. E.g. `-bs 8` is an 8 KB buffer. Default: 4 (4 KB); values smaller than 4 KB are rejected.

## Selecting fields by name
`-f` takes 1-based numeric field indexes; `-fn` takes header names matched case-insensitively against the first input row, e.g. `-fn name,price`. `-f` and `-fn` are mutually exclusive. When `-fn` is used the header row is treated as the field-name source and is processed/passed like data unless you also pass `-ifn` to drop it from output.

## Common options
Also accepts the standard options — see [conventions](conventions.md). The source generic-flag token is `#ALL,SKIP,PASS`, so the full common set applies: output `-o`, CSV separators `-sep`/`-rsep`/`-osep`, `-ibl` (ignore blank lines), `-smq` (smart-quote), `-ifn` (ignore field-name header), `-hdr` (header record), and the `-skip`/`-pass` row filters (filtered rows are written through unchanged without calling the function).

## Examples

These examples are illustrative (this build does not include `call`). They use the sample DLL in `csvfix/call-dll/`, whose `MyFunc` wraps each passed field in braces and returns the wrapped fields, which `call` then appends to the row.

Input (`csvfix/call-dll/input.csv`):
```
foo,bar
1,2,3
```

Call `MyFunc` in `mydll.dll`, passing all fields, with an 8 KB return buffer (from `csvfix/call-dll/calltest`):
```
csvfix call -fnc MyFunc -dll mydll.dll -bs 8 input.csv
```
Illustrative output (each input field returned wrapped in braces and appended after the originals):
```
foo,bar,{foo},{bar}
1,2,3,{1},{2},{3}
```

Pass only selected columns to the function using 1-based indexes:
```
csvfix call -fnc MyFunc -dll mydll.dll -f 1 input.csv
```
Illustrative output (only the first field is passed; one wrapped field is appended):
```
foo,bar,{foo}
1,2,3,{1}
```

## Notes
- **Function ABI.** The exported function must have the signature `int Func(int infc, const char * in, int * outfc, char * out, int bufsize)`. Input fields arrive in `in` as `infc` consecutive null-terminated strings; returned fields must be written to `out` as `outfc` consecutive null-terminated strings, and they are appended to the existing row. See the worked sample `csvfix/call-dll/dllmain.cpp` (exported with `extern "C" __declspec(dllexport)`).
- **Return-value semantics.** `0` → write row with returned fields appended; `< 0` → skip the row on output (no error); `> 0` → CSVfix aborts with `<func> returned error code <n>`. If the function returns non-zero, no fields are appended.
- **Buffer size.** `-bs` is in kilobytes (internally multiplied by 1024). Minimum effective size is 4 KB; a non-integer value or a value below the minimum is an error. Make the buffer large enough to hold all returned fields including their null terminators, or behaviour is undefined.
- **Field selection default.** With neither `-f` nor `-fn`, every field of each row is passed to the function.
- **Help text vs. manual.** The bundled HTML manual lists only `-f`; the current source also registers `-fn` (`FLAG_FNAMES = "-fn"`), documented above.
- Related: [`exec`](exec.md) (run an external command per row) and [`eval`](eval.md) (expression-based field computation) achieve similar per-row transformation without a DLL.

## See also
[exec](exec.md), [eval](eval.md), [conventions](conventions.md)
