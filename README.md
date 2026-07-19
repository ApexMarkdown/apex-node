# @apexmarkdown/apex

Node.js bindings for [Apex](https://github.com/ApexMarkdown/apex).

## Install

```bash
npm install @apexmarkdown/apex
```

## Usage

```js
const { convert, version } = require("@apexmarkdown/apex");

const html = convert("# Hello", { mode: "gfm", generateHeaderIds: true });
console.log(html);
console.log(version);
```

## Supported platforms

- macOS arm64 / x64
- Linux arm64 / x64
- Node.js 20+

No system `cmark-gfm` or `libyaml` install is required; both are bundled and
statically linked via the vendored `vendor/apex` submodule.

## Options

Options use camelCase keys matching Apex's serializable `apex_options`
fields (e.g. `mode`, `generateHeaderIds`, `hardbreaks`). Unknown option keys
throw a `TypeError`.

## Local development

```bash
git submodule update --init --recursive
npm install
npm run build
npm test
```

To produce a platform-specific prebuilt package (used by CI and the publish
workflow):

```bash
npm run package:prebuild
```

This writes `prebuilds/@apexmarkdown/apex-<platform>-<arch>/`, ready to be
published as its own npm package.

## License

MIT
