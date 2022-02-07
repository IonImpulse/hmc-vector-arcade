Incantations
---
Workflow for making this project:
```bash
# Install cargo-pio and PlatformIO (run once per machine)
cargo install cargo-pio
cargo pio installpio
# Create the new project
cargo pio new --board sparkfun_thing_plus_v --target riscv32imac-unknown-none-elf vector-generator-processor
```

Workflow for working with this project:
```bash
# Check for errors/warnings/lints in the build
cargo check
cargo clippy
# Build the project
cargo pio build
# Build the project in release mode
cargo pio build --release
# Deploy the project to a device
cargo pio exec -- run --target upload
# Monitor the execution of the program via gdb:
cargo pio exec -- debug --interface=gdb -x .pioinit
```
Note that you should be able to build and deploy using PlatformIO and not using
cargo-pio at all, if you prefer. See [the documentation for
cargo-pio](https://github.com/ivmarkov/embuild/tree/master/cargo-pio) for help.
