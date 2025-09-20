# Pull
Pull files from the internet.

## Dependencies
### Build dependencies
- A C compiler
- Make

### Runtime dependencies
- libc
- [turtls](https://github.com/lukasvrenner/turtls)

## Building
Make sure `turtls` is in a directory parallel to this, e.g. `../turtls`.
If not, `Makefile` will have to be modified to find the library.

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
