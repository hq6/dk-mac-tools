# dk-mac-tools

This is an arbitrary collection of command line tools designed to be compatible
with [dk](https://github.com/hq6/dk).

## Installation

If you are not an existing `dk` user, [install `dk`](https://github.com/hq6/dk)
and then clone this repository into `$HOME/dkbin`.
```
git clone https://github.com/hq6/dk-mac-tools "$HOME/dkbin"
```

If you are an existing `dk` user and `$HOME/dkbin` already exists,
clone the repo and copy its contents into `$HOME/dkbin`.
```
git clone https://github.com/hq6/dk-mac-tools "$HOME/dkbin"
cp -r -n dkbin/* $HOME/dkbin
```
Note that this copy will not clobber any existing tools, so you may
not get all the tools if you have a tool with an overlapping name.

## Usage

Type `dk` from your home directory or any subdirectory to discover all
available commands.
