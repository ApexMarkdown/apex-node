const fs = require("node:fs");
const path = require("node:path");
const { platformPackageName } = require("../lib/load-native.js");

const root = path.join(__dirname, "..");
const pkg = require("../package.json");
const name = platformPackageName();
if (!name) {
  console.error(`Unsupported platform: ${process.platform}-${process.arch}`);
  process.exit(1);
}

const addonCandidates = [
  path.join(root, "build", "Release", "apex_node.node"),
  path.join(root, "binding", "build", "Release", "apex_node.node")
];
const addon = addonCandidates.find((p) => fs.existsSync(p));
if (!addon) {
  console.error("Built addon not found; run npm run build first");
  process.exit(1);
}

const outDir = path.join(root, "prebuilds", name);
fs.rmSync(outDir, { recursive: true, force: true });
fs.mkdirSync(outDir, { recursive: true });

const os = process.platform;
const cpu = process.arch;
const rendered = fs
  .readFileSync(path.join(root, "platforms", "package.template.json"), "utf8")
  .replaceAll("PLATFORM_ARCH", `${os === "darwin" ? "darwin" : "linux"}-${cpu}`)
  .replaceAll("PACKAGE_VERSION", pkg.version)
  .replaceAll("\"OS\"", `"${os}"`)
  .replaceAll("\"CPU\"", `"${cpu}"`);

fs.writeFileSync(path.join(outDir, "package.json"), rendered);
fs.copyFileSync(addon, path.join(outDir, "apex.node"));
fs.writeFileSync(
  path.join(outDir, "README.md"),
  `# ${name}\n\nNative Apex addon for ${os}-${cpu}.\n`
);
console.log(`Wrote ${outDir}`);
