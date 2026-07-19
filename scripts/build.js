const { spawnSync } = require("node:child_process");
const path = require("node:path");

const root = path.join(__dirname, "..");
const result = spawnSync(
  "npx",
  ["cmake-js", "compile", "--directory", "binding"],
  { cwd: root, stdio: "inherit", shell: process.platform === "win32" }
);
process.exit(result.status ?? 1);
