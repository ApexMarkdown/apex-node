const { describe, it } = require("node:test");
const assert = require("node:assert/strict");
const { convert } = require("../lib/index.js");

describe("option behavior", () => {
  it("disables header ids when generateHeaderIds is false", () => {
    const withIds = convert("# Hello", { mode: "gfm", generateHeaderIds: true });
    const withoutIds = convert("# Hello", { mode: "gfm", generateHeaderIds: false });
    assert.match(withIds, /id=/);
    assert.doesNotMatch(withoutIds, /id=/);
  });

  it("applies criticMode accept", () => {
    const html = convert("{++added++}", {
      mode: "unified",
      enableCriticMarkup: true,
      criticMode: "accept"
    });
    assert.match(html, /added/);
    assert.doesNotMatch(html, /ins/);
  });

  it("accepts stylesheetPaths array without throwing", () => {
    const html = convert("# Hi", {
      standalone: true,
      documentTitle: "Doc",
      stylesheetPaths: []
    });
    assert.match(html, /<html/i);
  });
});
