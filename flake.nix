{
  description = "cshot";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "cshot";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
            clang
            wayland-scanner
            pkg-config
          ];

          buildInputs = with pkgs; [
            wayland
            wayland-protocols
            cairo
            libxkbcommon
            expat
          ];

          cmakeFlags = [
            "-DCMAKE_C_COMPILER=clang"
            "-DCMAKE_CXX_COMPILER=clang++"
          ];
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ self.packages.${system}.default ];

          shellHook = ''
            echo "  cshot devenv"
            echo "  aliases set up:"
            echo "    configure  - run CMake configuration"
            echo "    build      - self-explainatory ig"

            configure() {
              mkdir -p build
              cd build
              cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=1
              cd ..
            }

            build() {
              configure
              cmake --build build
            }
          '';
        };
      }
    );
}
