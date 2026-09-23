{ pkgs ? import <nixpkgs> { } }:

let
  gcc = import ./stage1/gcc.nix { inherit pkgs; };
  binutils = import ./stage1/binutils.nix { inherit pkgs; };
in pkgs.stdenvNoCC.mkDerivation rec {
  name = "lua-${version}";
  src = pkgs.fetchFromGitHub {
    owner = "lua";
    repo = "lua";
    rev = "v5.5.1";
    hash = "sha256-Sml/XrEMBC6uxLmq7EW84YSIcK52qLIngCNQNvzax5E=";
  };
  version = "5.5.1";

  nativeBuildInputs = [ gcc binutils ];

  preBuild = ''
    sed -i 's/CC= gcc/CC= i386-unknown-naos-gcc/' makefile
    sed -i 's/AR= ar/AR= i386-unknown-naos-ar/' makefile
    sed -i 's/RANLIB= ranlib/RANLIB= i386-unknown-naos-ranlib/' makefile
  '';

  installPhase = ''
    mkdir -p $out/{bin,lib}
    cp -v lua $out/bin/
    cp -v liblua.a $out/lib/
  '';

  enableParallelBuilding = true;
  dontFixup = true;
}
