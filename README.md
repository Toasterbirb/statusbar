# statusbar

*statusbar* is an asynchronous status setter for the [dwm](https://dwm.suckless.org/) window manager. It spins up a separate thread for each "module" and thus if one command happens to take ages to finish, it doesn't affect the rest of the status bar. This also allows for blocking module scripts that will wait until something happens (think of `mpc idle` for example).

## Configuration
An example configuration is provided in the [statusbar.json](./statusbar.json) file. The configuration file should be placed to `~/.config/statusbar.json`, but the program can also be given a custom configuration path with the `--config` argument.

The module are stored into an array and configured with different variables. The `name` variable specifies the module and is always required. In the table below are the common variables that can be specified for any module.

| Variable | Optional | Type   | Function |
|----------|----------|--------|----------|
| name     | No       | string | Specifies the module |
| icon     | Yes      | string | Icon or text to display before the module output |
| interval | Yes      | int    | How many seconds to wait between module updates (default: 1s) |

### Available modules

#### text
Print out static text that never changes.

| Variable | Optional | Type   | Function |
|----------|----------|--------|----------|
| text     | No       | string | Text to display |

#### shell
Execute a shell command and display the output. The command output should all fit into a singular line.

| Variable | Optional | Type   | Function |
|----------|----------|--------|----------|
| cmd      | No       | string | Shell command to execute |

#### memory
Show the current memory/swap usage of the system.

| Variable | Optional | Type | Function |
|----------|----------|------|----------|
| decimals | Yes      | int  | How many decimal points of accuracy to display |
| swap     | Yes      | bool | Should swap be included in the total |


## Building
Build the project with g++ by running `make`. To speed up the build, you can try using the -j flag.
```sh
make -j$(nproc)
```

## Installation
To install statusbar to /usr/local/bin, run the following
```sh
make install
```
You can customize the installation prefix with the PREFIX variable like so
```sh
make PREFIX=/usr install
```

## Uninstall
```sh
make uninstall
```
