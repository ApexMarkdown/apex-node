const { describe, it } = require("node:test");
const assert = require("node:assert/strict");
const { platformPackageName } = require("../lib/load-native.js");

describe("platformPackageName", () => {
  it("returns a scoped package for the current host when supported", () => {
    const name = platformPackageName();
    if (process.platform === "darwin" || process.platform === "linux") {
      if (process.arch === "arm64" || process.arch === "x64") {
        assert.match(name, /^@apexmarkdown\/apex-(darwin|linux)-(arm64|x64)$/);
        return;
      }
    }
    assert.equal(name, null);
  });
});
