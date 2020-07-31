let
  sources = import ./nix/sources.nix;
  pkgs = import sources.nixpkgs {};
  target = "i686-elf";
  i686-elf-binutils =
    pkgs.stdenv.mkDerivation
      { name = "i686-elf-" + pkgs.binutils-unwrapped.name;
        src = pkgs.binutils-unwrapped.src;
        version = pkgs.binutils-unwrapped.version;
        configurePhase = ''
          ./configure --target=${target} --prefix=$out --with-sysroot --disable-nls --disable-werror
        '';
        doCheck = false;
        enableParallelBuilding = true;
      };
  i686-elf-gcc =
    pkgs.stdenv.mkDerivation
      { name = "i686-elf-" + pkgs.gcc-unwrapped.name;
        src = pkgs.gcc-unwrapped.src;
        version = pkgs.gcc-unwrapped.version;
        configurePhase = ''
          export PATH=${i686-elf-binutils}/bin:$PATH
          cd ..
          mkdir gcc_build
          cd gcc_build
          ../$sourceRoot/configure --target=${target} --prefix=$out --disable-nls --enable-languages=c --without-headers --disable-multilib
          '';
        buildPhase = ''
          export PATH=${i686-elf-binutils}/bin:$PATH
          make -j16 all-gcc
          make -j16 all-target-libgcc
          '';
        installPhase = ''
          make -j16 install-gcc
          make -j16 install-target-libgcc
          '';
        doCheck = false;
        enableParallelBuilding = true;
        hardeningDisable = [ "format" "pie" ];
        dontFixup = true;
        buildInputs = [
          i686-elf-binutils
          pkgs.mpfr.dev
          pkgs.gmp.dev
          pkgs.libmpc
        ];
      };
in
  { binutils = i686-elf-binutils;
    gcc = i686-elf-gcc;
  }
