{ stdenv, autoPatchelfHook }:

stdenv.mkDerivation rec {
  pname = "i386-elf";
  version = "7.5.0";
  src = ./vendor + "/${pname}-${version}-Linux-x86_64.tar.xz";

  phases = [ "unpackPhase" "installPhase" "fixupPhase" ];
  nativeBuildInputs = [ autoPatchelfHook ];
  installPhase = ''
    mkdir $out
    cp -r * $out/
  '';
}
