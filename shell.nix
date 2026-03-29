{ pkgs ? import <nixpkgs> { } }:

let toolchain = pkgs.callPackage (import ./compiler.nix) { };
in pkgs.mkShell {
  buildInputs = [ pkgs.qemu toolchain pkgs.cppcheck pkgs.indent ];
}
