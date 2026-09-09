# StructParser

StructParser reads its complete source file list from standard input, one
unquoted path per line, until EOF. LF and CRLF are accepted; empty lines are
ignored. Relative paths resolve against the current working directory.
Repeated paths are scanned once. Only `.c` and `.h` files are scanned,
case-insensitively, with the existing AutoGen and other parser exclusions
applied.

Pass target metadata as separate `Name=value` command-line arguments. Quote
the entire argument when its value contains spaces. Names are case-insensitive.
Semicolon-separated assignments within an argument are also accepted.

| Argument | Meaning |
| --- | --- |
| `TargetName` | Target name used for generated files and symbols. |
| `SrcDir` | Existing target source directory; contains generated `AutoGen/`. |
| `CommonDir` | Existing shared source directory; contains shared `AutoGen/`. |
| `OutDir` | Target output directory. |
| `IntDir` | Intermediate directory for the clean-build marker. |
| `Configuration` | Build configuration, such as `Debug` or `Release`. |
| `ProjectDir` | Existing directory to start searching for `StructParserVars.txt`. |
| `IsExecutable` | `true` for an executable; `false` for a library. |

All arguments are required and nonempty. Directory arguments can be absolute
or relative to the working directory. The parser updates generated files and
its saved dependency list; unchanged source membership and timestamps allow
an incremental quick exit. An empty input stream supplies an empty source list.
The parser returns zero on success and nonzero on failure.

For example, from a Windows command prompt:

```bat
StructParser.exe TargetName=Example SrcDir=src CommonDir=Common ^
OutDir=bin IntDir=obj Configuration=Debug ProjectDir=. ^
IsExecutable=true < sources.txt
```
