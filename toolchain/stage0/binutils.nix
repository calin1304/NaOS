{ pkgs ? import <nixpkgs> { } }:

pkgs.stdenv.mkDerivation {
  name = "i386-unknown-naos-binutils";
  src = pkgs.fetchFromGitHub {
    owner = "gnutools";
    repo = "binutils-gdb";
    rev = "binutils-2_47";
    hash = "sha256-F8LFBk9eT3kAIpCCOrm/lnvAArZXb6HyZ/0XttLkL7k=";
  };
  version = "2.47";
  patches = [ ./binutils.patch ];
  nativeBuildInputs =
    [ pkgs.automake pkgs.autoconf269 pkgs.bison pkgs.flex pkgs.texinfo ];

  dontUpdateAutotoolsGnuConfigScripts = true;
  preConfigure = ''
    mkdir build
    cd build
  '';
  configureFlags = [
    "--target=i386-unknown-naos"
    "--with-sysroot"
    "--disable-nls"
    "--disable-werror"
    "--enable-default-execstack=no"
    "--disable-gdb"
    "--disable-doc"
  ];
  configureScript = "../configure";

  enableParallelBuilding = true;
  preBuild = ''
    mkdir -p ld/ldscripts
  '';
  doCheck = false;
}
