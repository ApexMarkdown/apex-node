const fs = require("node:fs");
const path = require("node:path");
const os = require("node:os");

function platformPackageName() {
  const platform = process.platform;
  const arch = process.arch;
  if (platform === "darwin" && arch === "arm64") return "@apexmarkdown/apex-darwin-arm64";
  if (platform === "darwin" && arch === "x64") return "@apexmarkdown/apex-darwin-x64";
  if (platform === "linux" && arch === "arm64") return "@apexmarkdown/apex-linux-arm64";
  if (platform === "linux" && arch === "x64") return "@apexmarkdown/apex-linux-x64";
  return null;
}

function loadNative() {
  const candidates = [];
  const pkg = platformPackageName();
  if (pkg) {
    try {
      const resolved = require.resolve(`${pkg}/apex.node`);
      candidates.push(resolved);
    } catch (_) {
      // optional dependency may be absent during local development
    }
  }

  candidates.push(
    path.join(__dirname, "..", "build", "Release", "apex_node.node"),
    path.join(__dirname, "..", "binding", "build", "Release", "apex_node.node")
  );

  for (const candidate of candidates) {
    if (fs.existsSync(candidate)) {
      return require(candidate);
    }
  }

  const detected = `${process.platform}-${process.arch}`;
  throw new Error(
    `Apex native addon is unavailable for ${detected}. ` +
      `Install a supported optional dependency or run npm run build.`
  );
}

module.exports = { loadNative, platformPackageName };
