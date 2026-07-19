const { spawnSync } = require("node:child_process");
const path = require("node:path");

const root = path.join(__dirname, "..");
const result = spawnSync(
  "npx",
  [
    "cmake-js",
    "compile",
    "--directory",
    "binding",
    // The vendored cmark-gfm declares cmake_minimum_required(VERSION 3.0),
    // which CMake 4.x refuses to configure without this override.
    "--CDCMAKE_POLICY_VERSION_MINIMUM=3.5",
  ],
  { cwd: root, stdio: "inherit", shell: process.platform === "win32" }
);
process.exit(result.status ?? 1);
