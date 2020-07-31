let
  sources = import ./nix/sources.nix;
  pkgs = import sources.nixpkgs {};
  toolchain = import ./toolchain.nix;
in
  pkgs.mkShell {
    buildInputs = [
      toolchain.binutils
      toolchain.gcc
    ];
  }
