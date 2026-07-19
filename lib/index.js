const { loadNative } = require("./load-native.js");
const native = loadNative();

module.exports = {
  convert: native.convert,
  version: native.version
};
