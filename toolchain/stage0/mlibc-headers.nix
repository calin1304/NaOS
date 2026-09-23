{ pkgs ? import <nixpkgs> {}}:

pkgs.stdenv.mkDerivation rec {
    name = "mlibc-headers-${version}";
    src = pkgs.fetchFromGitHub {
        owner = "managarm";
        repo = "mlibc";
        rev = "v7.0.0";
        hash = "sha256-e4YjosGDI2CWkGeih0HG69yPJa+sKAReTQ87lgzBTzg=";
    };
    version = "v7.0.0";
    patches = [ ./mlibc.patch ];
    nativeBuildInputs = [
        pkgs.meson
        pkgs.ninja
    ];

    dontConfigure = true;
    buildPhase = ''
        meson setup \
            --cross-file=naos.cross-file \
            --prefix=/usr \
            -Dheaders_only=true \
            headers-build
    '';
    installPhase = ''
        DESTDIR=$out ninja -C headers-build install
    '';
}
