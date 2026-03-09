/* eslint-disable no-undef */
/* eslint-disable @typescript-eslint/no-require-imports */
const NCC = require("@vercel/ncc");
const {
  mkdirSync,
  existsSync,
  writeFileSync,
  rmSync,
  copyFileSync,
} = require("fs");
const path = require("path");

const outDir = "./build/src"; // Desired output directory

(async () => {
  const distDir = path.resolve(__dirname, outDir);
  if (existsSync(distDir)) {
    rmSync(distDir, { recursive: true, force: true });
  }

  // eslint-disable-next-line @typescript-eslint/no-unused-vars
  const { code, map, assets } = await NCC(
    path.join(__dirname, "/src/main.ts"),
    {
      // provide a custom cache path or disable caching
      cache: false,
      // externals to leave as requires of the build
      // externals: ["better_sqlite3", "mathengine3"],
      // directory outside of which never to emit assets
      filterAssetBase: process.cwd(), // default
      minify: false, // default
      sourceMap: false, // default
      assetBuilds: false, // default
      sourceMapBasePrefix: "../", // default treats sources as output-relative
      // when outputting a sourcemap, automatically include
      // source-map-support in the output file (increases output by 32kB).
      sourceMapRegister: false, // default
      watch: false, // default
      license: "", // default does not generate a license file
      target: "es2015", // default
      v8cache: false, // default
      quiet: false, // default
      debugLog: false, // default
    }
  );

  // Ensure the output directory exists
  if (!existsSync(outDir)) {
    mkdirSync(outDir, { recursive: true });
  }

  // Write the compiled code
  writeFileSync(path.join(outDir, "server.js"), code, "utf-8");

  // Write the source map (optional)
  if (map) {
    writeFileSync(path.join(outDir, "server.js.map"), map, "utf-8");
  }

  // // Write additional assets (like required files)
  // for (const [assetPath, asset] of Object.entries(assets)) {
  //   const assetFilePath = path.join(outDir, assetPath);
  //   mkdirSync(path.dirname(assetFilePath), { recursive: true });
  //   writeFileSync(assetFilePath, asset.source);
  // }

  // Copy Cheat.json
  copyFileSync(
    path.resolve(__dirname, "./src/cheats/cheat.json"),
    path.resolve(__dirname, "./build/src/cheat.json")
  );

  console.log(`Compiled files saved to ${outDir}`);
})();
