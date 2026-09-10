This monorepo contains the code for the client, developer utilities, and
several server-side services for the Cryptic engine that were used for City of
Heroes up until the game was cancelled in 2012. This is a fork of the Ourodev's
repo that was intended for porting to the latest Visual C compiler, but it also
has some minor feature additions.

# Dependencies

The CMake build downloads pinned zlib, Crypto++, and Lua 5.1.5 sources through
CPM. PhysX 2.8.4 remains in [3rdparty/PhysX](3rdparty/PhysX): MapServer requires
its proprietary Win32 SDK and binaries. CMake stages the matching PhysX DLLs
beside MapServer for each configuration. Other vendored packages remain for
components that have not yet migrated to CMake.

Debug staging also supplies the checked Core and Cooking binaries under the
unsuffixed filenames requested internally by the legacy PhysX loader. No
system-wide PhysX installation is needed for the dependency checks.

The presets cache downloaded sources in `out/build/CPM`, shared between
configurations. Set `CPM_SOURCE_CACHE` to use another cache location. The first
configure needs network access; subsequent builds reuse the pinned sources.
Do not edit the cache: dependency build patches belong in `cmake/patches` and
are applied by `CPMAddPackage`.

Runtime dependencies for a complete server installation include:

## SQL Server

https://www.microsoft.com/en-us/sql-server/sql-server-downloads

The character database is stored in SQL Server. Any version of SQL Server seems
to work. LocalDB is a lighter version of SQL Server that is recommended for
local testing and private use. Instructions to install and administrate SQL
Server is outside the scope of this guide.

* From the provided link, download the free SQL Server Express installer
* During installation, choose "Custom"
* Click through the default install wizard options until prompted for features
* On the features page, uncheck everything and then choose "LocalDB"
* Continue with defaults until installation is complete

NOTE: If you're prompted for something about "Azure", just disable that too

## ODBC17

https://learn.microsoft.com/en-us/sql/connect/odbc/download-odbc-driver-for-sql-server?view=sql-server-ver17

The ODBC driver is used by the game server to interface with the SQL Server.
It's basically the bridge between the game and the database. [ODBC 17
x86](https://go.microsoft.com/fwlink/?linkid=2361647) is the only driver that's
supported. Just download and run the installer.

# Building

Install Visual Studio 2026 with the **Desktop development with C++** workload,
including the Windows SDK and CMake tools. The Visual Studio 2026 generator
requires CMake 4.2 or newer. Git and network access are needed for CPM to fetch
build dependencies on the first configure.

From the repository root in a Visual Studio Developer PowerShell or command
prompt, configure once and build either configuration:

```text
cmake --preset vs2026
cmake --build --preset vs2026-debug
cmake --build --preset vs2026-release
```

MapServer is enabled by default for MSVC Win32. To build it specifically:

```text
cmake --build --preset vs2026-debug --target MapServer
cmake --build --preset vs2026-release --target MapServer
```

Executables and PhysX DLLs are in `out/build/vs2026/bin/Debug` or
`out/build/vs2026/bin/Release`. Set `COX_BUILD_MAPSERVER=OFF` to configure only
the existing utilities. MapServer requires MSVC Win32; the MinGW preset keeps
building the utilities with MapServer disabled. Enabling MapServer with an
unsupported compiler or architecture is a configuration error.

CMake is the authoritative build system. The handwritten Visual Studio
projects and solution are historical and still reference removed vendored
dependencies. This CMake build currently provides StructParser, UtilitiesLib,
pig, AILib, and MapServer; it does not yet build Game, DBServer, or BeaconServer.

## Build checks

Configure with `COX_BUILD_UTILITIES_TESTS=ON` and `COX_BUILD_PIG_TESTS=ON`, build,
then run `ctest --test-dir out/build/vs2026 -C Debug --output-on-failure` (or
`-C Release`). Standalone dependency checks exercise zlib, Crypto++, Lua, and
PhysX without game data or a database:

```text
cmake -S cmake/checks -B out/build/dependency-checks -G "Visual Studio 18 2026" -A Win32 -DCPM_SOURCE_CACHE=out/build/CPM
cmake --build out/build/dependency-checks --config Debug --parallel
ctest --test-dir out/build/dependency-checks -C Debug --output-on-failure
```

Repeat the build and tests with `Release` to check the release PhysX runtime.
These checks do not replace starting a map against DBServer with game data.

# Usage

For local testing, at the minimum, the DBServer, one MapServer, and the Game
client need to run together. MapServer and Game won't run without all of the
game data in pigg archive files or in the data folder. The DBServer needs the
minimal [servers.cfg](data/server/db/servers.cfg) config file to start it with
"fake auth" mode that accepts a user without a password.

First start the DBServer
```batch
start DBserver.exe
```

Then, start the MapServer to connect to the DBServer
```batch
start MapServer.exe -db 127.0.0.1 -map_id 1
```

Finally, start the game client to use the localmapserver.
```batch
start Game.exe -db 127.0.0.1 -localmapserver 1 -notimeout 1 -noaudio 1
```

This will enable logging in with a fake account and any password. Creating or
selecting any character will send them straight to Atlas Park (map ID 1).

**NOTE:** The data needed to run the game will be available soon
