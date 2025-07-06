{
  description = "NVIDIA HPC SDK development environment";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          config.allowUnfree = true;  # Required for NVIDIA proprietary software
        };
      in {
        devShells.default = pkgs.mkShell {
          name = "nvhpc-env";
          buildInputs = [ self.packages.${system}.nvhpc ];

          # Environment variables for SDK usage
          shellHook = ''
            export NVARCH=$(uname -s)_$(uname -m)
            export NVCOMPILERS=${self.packages.${system}.nvhpc}
            export PATH=$NVCOMPILERS/$NVARCH/25.5/compilers/bin:$PATH
            export MANPATH=$NVCOMPILERS/$NVARCH/25.5/compilers/man:$MANPATH
          '';
        };

        packages.nvhpc = pkgs.stdenv.mkDerivation rec {
          pname = "nvhpc";
          version = "25.5";
          src = pkgs.requireFile {
            name = "nvhpc_2025_255_Linux_x86_64_cuda_multi.tar.gz";
            sha256 = "YOUR_PREFETCHED_HASH";  # Use `nix-prefetch-url <URL>`
            message = "Download from: https://developer.nvidia.com/hpc-sdk-downloads";
          };

          nativeBuildInputs = [ pkgs.autoPatchelfHook pkgs.gnumake pkgs.gcc ];
          sourceRoot = ".";

          installPhase = ''
            runHook preInstall
            # Silent install to avoid prompts
            export NVHPC_SILENT=true
            export NVHPC_INSTALL_DIR=$out
            export NVHPC_INSTALL_TYPE="single"  # Use "network" for multi-node setups
            ./install
            runHook postInstall
          '';
        };
      }
    );
}
