{ pkgs ? import <nixpkgs> {}}:

let binutils = ((import ./binutils.nix) { inherit pkgs; });
    sysroot = import ./sysroot.nix { inherit pkgs; };
in 
pkgs.stdenv.mkDerivation {
    name = "i386-unknown-naos-gcc";
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
    configureFlags = [
        "--target=i386-unknown-naos"
        "--with-arch=i686"
        "--with-sysroot=${sysroot}"
        "--enable-languages=c,c++"
        "--disable-werror"
        "--disable-multilib"
        "--enable-initfini-array"
        "--with-as=${binutils}/bin/i386-unknown-naos-as"
        "--with-ld=${binutils}/bin/i386-unknown-naos-ld"
    ];
    configureScript = "../configure";
    NIX_CFLAGS_COMPILE = "-Wno-error=format-security";

    enableParallelBuilding = true;
    buildPhase = ''
        make -j16 all-gcc all-target-libgcc
    '';

    installPhase = ''
        make install-gcc install-target-libgcc
    '';

    # preBuild = ''
    #     mkdir -p ld/ldscripts
    # '';
    doCheck = false;
}
