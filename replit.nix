{ pkgs }: {
	deps = [
		pkgs.emscripten
  pkgs.dotnet-sdk
    pkgs.omnisharp-roslyn
	];
}