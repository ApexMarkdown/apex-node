const { describe, it } = require("node:test");
const assert = require("node:assert/strict");
const { convert } = require("../lib/index.js");

describe("option validation", () => {
  it("rejects unknown option names", () => {
    assert.throws(
      () => convert("# x", { notARealOption: true }),
      (err) => err instanceof TypeError && /Unknown option/.test(err.message)
    );
  });

  it("rejects wrong boolean types", () => {
    assert.throws(
      () => convert("# x", { unsafe: "yes" }),
      (err) => err instanceof TypeError && /unsafe/.test(err.message)
    );
  });

  it("rejects invalid mode strings", () => {
    assert.throws(
      () => convert("# x", { mode: "pandoc" }),
      (err) => err instanceof TypeError && /mode/.test(err.message)
    );
  });

  it("rejects non-string markdown", () => {
    assert.throws(
      () => convert(42),
      (err) => err instanceof TypeError
    );
  });
});
