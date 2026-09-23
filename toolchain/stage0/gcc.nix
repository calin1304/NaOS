{ pkgs ? import <nixpkgs> { } }:

let binutils = import ./binutils.nix { inherit pkgs; };
    sysroot = import ./mlibc-headers.nix { inherit pkgs; };
in pkgs.stdenv.mkDerivation {
  name = "i386-unknown-naos-gcc_freestanding";
  src = pkgs.fetchFromGitHub {
    owner = "gcc-mirror";
    repo = "gcc";
    rev = "releases/gcc-16.2.0";
    hash = "sha256-ltSkLmQg8nUAUlorrXoT/6jWiJa/78O3daOB93Ztp0U=";
  };
  version = "16.2.0";
  patches = [ ./gcc.patch ];
  nativeBuildInputs = [
    pkgs.automake
    pkgs.autoconf269
    pkgs.bison
    pkgs.flex
    pkgs.gmp
    pkgs.mpfr
    pkgs.libmpc
    
    binutils
  ];

  dontUpdateAutotoolsGnuConfigScripts = true;
  preConfigure = ''
    cd libstdc++-v3
    ${pkgs.autoconf269}/bin/autoreconf
    cd ..

    mkdir build
    cd build
  '';

  configureScript = "../configure";
  NIX_CFLAGS_COMPILE = "-Wno-error=format-security";
  configureFlags = [
    "--target=i386-unknown-naos"
    "--with-arch=i686"
    "--with-sysroot=${sysroot}"
    "--without-headers"
    "--enable-languages=c,c++"
    "--enable-initfini-array"
    "--disable-werror"
    "--disable-multilib"
    "--disable-hosted-libstdcxx"
    "--enable-wchar_t"
    "--with-as=${binutils}/bin/i386-unknown-naos-as"
    "--with-ld=${binutils}/bin/i386-unknown-naos-ld"
  ];

  buildPhase = ''
    make -j16 all-gcc all-target-libgcc
  '';

  enableParallelBuilding = true;

  installPhase = ''
    make install-gcc install-target-libgcc
  '';

  doCheck = false;
}
