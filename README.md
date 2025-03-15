# Pull
Pull files from the internet.
This program primarily exists to test [TurTLS](https://github.com/lukasvrenner/turtls).

## Usage
To use `pull`, you need an Internet connection.
Run the following command:
```bash
pull <domain_name>
```
Where <domain_name> represents the domain name of the server you're trying to connect to.

In the future, `pull` will ideally take a URL as input instead.

## Dependencies
### Build dependencies
- A C compiler
- [meson](https://github.com/mesonbuild/meson)

### Runtime dependencies
- libc (POSIX-compatible)
- [TurTLS](https://github.com/lukasvrenner/turtls)

## Building
### Debug mode
```bash
meson setup build
meson compile -C build
```

### Release mode
```bash
meson setup build --buildtype release
meson compile -C build
```

## Installing
Follow the build instructions, then run
```bash
meson install -C build
```
This will install `pull` to `/usr/local/bin/`. To install to a different path,
set the `DESTDIR` environment variable or add the `--destdir` flag.

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
