{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation rec {
  pname = "acronym";
  version = "1.0.1";

  src = ./.;

  nativeBuildInputs = [ pkgs.pkg-config pkgs.meson pkgs.ninja pkgs.pcre ];

  buildPhase = ''
    meson setup build $src --prefix=$out
    meson compile -C build
  '';

  installPhase = ''
    meson install -C build
  '';
}
