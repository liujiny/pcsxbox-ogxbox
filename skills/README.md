# Codex skills for this repository

`pcsxbox-ogxbox/` is a [Codex skill](https://github.com/openai/codex) — a reusable set
of instructions the agent loads automatically when a task matches its description.

It covers rebuilding and debugging this project: the legacy XDK toolchain and the
four-core build (1.4 / 1.5 / Reloaded / 1.6), which directory belongs to which core,
why the emulator chain-loads companion XBEs, the black-screen root cause and the
experiments already ruled out, and the frame-time A/B procedure.

## Install

```bash
cp -r skills/pcsxbox-ogxbox ~/.codex/skills/
```

Then ask for the work in plain language, or invoke it explicitly with
`$pcsxbox-ogxbox`.

Build all four cores and assemble a drop-in test folder (requires the legacy XDK
and a WSL/Linux host driving the Windows binaries):

```bash
NAME=PCSXBox_build bash ~/.codex/skills/pcsxbox-ogxbox/scripts/build_all_cores.sh
```
