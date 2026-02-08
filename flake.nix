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
        packages.default = pkgs.clangStdenv.mkDerivation {
          pname = "cshot";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            cmake
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
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ self.packages.${system}.default ];

          nativeBuildInputs = [ pkgs.clang-tools ];

          shellHook = ''
            echo "  cshot devenv"
            echo "  aliases set up:"
            echo "    configure  - run CMake configuration"
            echo "    build      - self-explainatory ig"

            configure() {
              mkdir -p build
              cd build
              cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
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
