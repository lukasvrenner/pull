# Pull
Pull files from the internet.

## Usage
```bash
pull google.com
```

## Dependencies
### Build dependencies
- A C compiler
- Make

### Runtime dependencies
- libc
- [turtls](https://github.com/lukasvrenner/turtls)

## Building
By default, TurTLS's root directory must be in the directory `$HOME/projects/turtls`.
To change this, set the variable TURTLS:
```bash
make TURTLS=<path_to_turtls>
```
The library `libturtls.so` must be in TURTLS/target/debug or TURTLS/target/release.
The TurTLS header files must be in TURTLS/include.

### Debug mode
```bash
make
```
The executable will be generated in `./build`.

### Release mode
```bash
make release
```
The executable will be generated in `./buildrel`.

## License
Copyright 2024 Lukas Renner

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

See LICENSE for details
