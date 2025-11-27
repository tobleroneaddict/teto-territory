{
  description = "video game shitpost with kasane teto";
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-stable";
  };
  outputs = {self,nixpkgs}: {
    packages = nixpkgs.lib.genAttrs ["x86_64-linux"] (system:
      let pkgs = import nixpkgs {inherit system;};
      in {
        teto-territory = pkgs.stdenv.mkDerivation {
          pname = "teto-territory";
          version = "1.0";
          src = ./.;
          nativeBuildInputs = with pkgs; [ gcc ];
          buildInputs = with pkgs; [sdl3 sdl3-image tinyxml2];
          buildPhase = ''
            g++ -O2 -Wall -lSDL3 -lSDL3_image -ltinyxml2 *.cpp -o teto-territory
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp teto-territory $out/bin/
          '';
          meta = with pkgs.lib; {
            # description = "video game shitpost with kasane teto";
            description = self.description;
            license = licenses.gpl3;
            platforms = platforms.unix;
          };
        };
      }
    );
    defaultPackage = {
      aarch64-darwin = self.packages.aarch64-darwin.teto-territory;
      x86_64-linux = self.packages.x86_64-linux.teto-territory;
    };
    defaultApp = {
      forAllSystems = nixpkgs.lib.mapAttrs' (system: pkg: {
        inherit system;
        defaultApp = {
          type = "app";
          program = "${pkg.teto-territory}/bin/teto-territory";
        };
      }) self.packages;
    };
  };
}
