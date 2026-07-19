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

  it("accepts camelCase mode aliases only as documented strings", () => {
    assert.doesNotThrow(() => convert("# x", { mode: "unified" }));
    assert.doesNotThrow(() => convert("# x", { mode: "commonmark" }));
    assert.doesNotThrow(() => convert("# x", { mode: "quarto" }));
  });

  it("rejects snake_case option names", () => {
    assert.throws(
      () => convert("# x", { enable_tables: true }),
      TypeError
    );
  });

  it("rejects fractional integer options", () => {
    assert.throws(
      () => convert("# x", { terminalWidth: 2.5 }),
      (err) =>
        err instanceof TypeError && /terminalWidth must be a finite integer/.test(err.message)
    );
  });

  it("rejects NaN integer options", () => {
    assert.throws(
      () => convert("# x", { terminalWidth: NaN }),
      (err) =>
        err instanceof TypeError && /terminalWidth must be a finite integer/.test(err.message)
    );
  });

  it("rejects Infinity integer options", () => {
    assert.throws(
      () => convert("# x", { terminalWidth: Infinity }),
      (err) =>
        err instanceof TypeError && /terminalWidth must be a finite integer/.test(err.message)
    );
  });

  it("rejects out-of-range integer options", () => {
    assert.throws(
      () => convert("# x", { terminalWidth: 2147483648 }),
      (err) =>
        err instanceof TypeError &&
        /terminalWidth must be a finite integer within int32 range/.test(err.message)
    );
  });
});
