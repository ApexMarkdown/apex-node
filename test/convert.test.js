const { describe, it } = require("node:test");
const assert = require("node:assert/strict");
const { convert, version } = require("../lib/index.js");

describe("convert", () => {
  it("renders a heading by default", () => {
    const html = convert("# Hello");
    assert.match(html, /<h1[^>]*>Hello<\/h1>/);
  });

  it("honors gfm mode for strikethrough", () => {
    const html = convert("~~x~~", { mode: "gfm" });
    assert.match(html, /<del>x<\/del>/);
  });

  it("returns empty string for empty input", () => {
    assert.equal(convert(""), "");
  });

  it("preserves utf8", () => {
    const html = convert("café 漢字");
    assert.match(html, /café 漢字/);
  });
});

describe("version", () => {
  it("exposes a non-empty version string", () => {
    assert.equal(typeof version, "string");
    assert.notEqual(version.length, 0);
  });
});
