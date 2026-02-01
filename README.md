# Dynaserve CLI

Command-line interface for Dynaserve cloud infrastructure management.

## Installation

### macOS - Apple Silicon (arm64)
```bash
curl -L -o dynaserve https://github.com/YOUR_USERNAME/dynaserve-cli/releases/latest/download/dynaserve-darwin-arm64
chmod +x dynaserve && sudo mv dynaserve /usr/local/bin/
```

### macOS - Intel (x86_64)
```bash
curl -L -o dynaserve https://github.com/YOUR_USERNAME/dynaserve-cli/releases/latest/download/dynaserve-darwin-x86_64
chmod +x dynaserve && sudo mv dynaserve /usr/local/bin/
```

### Linux (x86_64)
```bash
curl -L -o dynaserve https://github.com/YOUR_USERNAME/dynaserve-cli/releases/latest/download/dynaserve-linux-x86_64
chmod +x dynaserve && sudo mv dynaserve /usr/local/bin/
```

### Linux (aarch64)
```bash
curl -L -o dynaserve https://github.com/YOUR_USERNAME/dynaserve-cli/releases/latest/download/dynaserve-linux-aarch64
chmod +x dynaserve && sudo mv dynaserve /usr/local/bin/
```

## Usage

```bash
dynaserve help              # Show available commands
dynaserve --version         # Show CLI version
```

## Development

### Build from source
```bash
make
```

### Clean build
```bash
make clean && make
```

## Release

To create a new release, commit with the version number as the message:
```bash
git commit -m "1.0.0"
git push origin Release
```

This will automatically build binaries for all platforms and create a GitHub release.

## License

© 2020 - 2026 Dynaserve & Lachy Schumacher.....