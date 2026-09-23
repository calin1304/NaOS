
{ pkgs ? import <nixpkgs> {}}:

let
    headers = import ../stage0/mlibc-headers.nix { inherit pkgs; };
    mlibc = import ../stage0/mlibc.nix { inherit pkgs; };
in 
pkgs.symlinkJoin {
    name = "naos-sysroot";
    paths = [
        headers
        mlibc
    ];
}
