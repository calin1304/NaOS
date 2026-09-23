{ pkgs ? import <nixpkgs> {}}:

let binutils = import ./binutils.nix { inherit pkgs; };
    gcc = import ./gcc.nix { inherit pkgs; };
    sysroot = import ./mlibc-headers.nix { inherit pkgs; };

  subprojects = {
    freestnd-c-hdrs = pkgs.fetchFromGitHub {
      owner = "osdev0";
      repo = "freestnd-c-hdrs";
      rev = "d33711241b46ecb8f2ad33927fcefdcb3ac0162e";
      hash = "sha256-gi+ZNmZvzYicRc/NZONFC2P984EXcyp7nUtT6vXaJ68=";
    };
    freestnd-cxx-hdrs = pkgs.fetchFromGitHub {
      owner = "osdev0";
      repo = "freestnd-cxx-hdrs";
      rev = "a6b351e0ab3e74e5789b01fa1447e4cd62373da7";
      hash = "sha256-sDXHMP/xTuL+DtaJgyxl322IWIXXcqRUbtJRMvYUmZY=";
    };
    frigg = pkgs.fetchFromGitHub {
      owner = "managarm";
      repo = "frigg";
      rev = "cf3ac202438a08d45fd2dc54ee6b27624723f8e7";
      hash = "sha256-p5sXHLdAw+le5/XZGM+4x9ZMVR740Mmt0kuzMgp6Ob8=";
    };
    libsmarter = pkgs.fetchFromGitHub {
      owner = "managarm";
      repo = "libsmarter";
      rev = "f7d061bc37d485418344452c7ceb28d5df3ba85d";
      hash = "sha256-K2K2Vya8uOtcVBhogbaS7xN8KKMiWvStHQD7MWF3EWk=";
    };
  };
in
pkgs.stdenv.mkDerivation rec {
    name = "mlibc-${version}";
    src = pkgs.fetchFromGitHub {
        owner = "managarm";
        repo = "mlibc";
        rev = "v7.0.0";
        hash = "sha256-e4YjosGDI2CWkGeih0HG69yPJa+sKAReTQ87lgzBTzg=";
    };
    version = "v7.0.0";
    patches = [ ./mlibc-full.patch ];
    buildInputs = [
        sysroot
    ];
    nativeBuildInputs = [
        pkgs.meson
        pkgs.ninja
        gcc
        binutils
    ];

    postUnpack = ''
        mkdir -p $sourceRoot/subprojects


        ${pkgs.lib.concatStringsSep "\n" (
        pkgs.lib.mapAttrsToList
            (name: src: "cp -r ${src} $sourceRoot/subprojects/${name}")
            subprojects
        )}

        chmod u+w $sourceRoot/subprojects/freestnd-c-hdrs
        cp \
            $sourceRoot/subprojects/packagefiles/freestnd-c-hdrs/meson.build  \
            $sourceRoot/subprojects/freestnd-c-hdrs/meson.build

        chmod u+w $sourceRoot/subprojects/freestnd-cxx-hdrs
        cp \
            $sourceRoot/subprojects/packagefiles/freestnd-cxx-hdrs/meson.build  \
            $sourceRoot/subprojects/freestnd-cxx-hdrs/meson.build
    '';

    dontConfigure = true;

    mesonFlags = [
        "--wrap-mode=nofallback"
        "-Dlinux_kernel_headers=${pkgs.linuxHeaders}/include"
    ];
    buildPhase = ''
        meson setup \
            --cross-file=${./naos.cross-file} \
            --prefix=/usr \
            -Ddefault_library=static \
            -Dno_headers=true \
            build
    '';
    installPhase = ''
        DESTDIR=$out ninja -C build install
    '';
}
