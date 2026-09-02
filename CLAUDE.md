# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

prufus creates a bootable USB with a Windows installer from GNU/Linux (GPT/UEFI only, no TPM-bypass hacks). GPL-3.0.

## Build

```sh
make            # top-level: runs `make -C source_code`, produces ./prufus
make clean      # `make -C source_code clean` + remove ./prufus
make install    # installs prufus, scripts/, and the desktop icon system-wide (needs root; uses $SUDO_USER for the Desktop icon)
make uninstall
```

`source_code/Makefile` is the real build (`prufus: main.c $(OBJS)`, one `cc` invocation per translation unit, no incremental dependency tracking beyond the object file existing — `make clean` before rebuilding after touching a header).

There are no tests and no linter configured in this repo.

### Build dependencies

Beyond the packages in README.md (`build-essential`, `libx11-dev`, `libgl1-mesa-dev`), the GUI links against two sibling libraries that are **not part of this repo** and must already be built and installed to `/usr/local/{lib,include}`:
- `pway` — the Wayland window/event/SHM helper (source at `/root/pway` in this environment).
- `pfonts` — TTF text rendering, with a CPU-only backend (`pfonts_cpu`) used here so no GL context is required for drawing (source at `/root/pfonts`).

Also needs `pixman-1` dev headers (`pkg-config --cflags/--libs pixman-1` is baked into the Makefile) and Wayland/xkbcommon dev headers (`-lwayland-client -lwayland-egl -lxkbcommon`).

For clangd/editor completion:
```sh
cd source_code && ./generate_compile_commands.sh
```
then edit the generated `compile_commands.json`, replacing the `"directory": "."` placeholder with the absolute path to `source_code/`.

## Architecture

**Two independent, unconnected frontends live in this repo; only one is actually built.**

- **`main.c` + `source_code/user_interface/*`** — the frontend built by `source_code/Makefile` (`OBJS` list). A single Wayland window created via `pway`, rendered entirely on the CPU: no GL/X11 anywhere in this path despite `-lGL -lX11` still being linked (those stay linked only because the static `pway`/`pfonts` archives keep their unused GL/EGL backends as alternatives to the CPU paths this binary actually calls). There is no separate window for the file-picker — `pway` manages exactly one surface, so `select_window.c`'s `can_draw_select_window` flag just swaps what gets drawn into that one window each frame (home screen vs. file browser) rather than opening a second OS window.
  - Per-frame draw loop (`main.c`): grab a buffer with `pway_shm_get_buffer()`, point both `pfonts_cpu_set_target()` and `cpu_image_set_target()` (`cpu_image.c`) at it, draw with `draw.c`'s `draw_text`/`draw_button_outline`/`draw_button_plane` (thin wrappers over `pfonts_cpu_draw_glyph`/`pfonts_cpu_draw_rect`) and `cpu_image_draw()` (a hand-rolled pixman blit — `pfonts_cpu` has no image API, used only for the directory-icon PNG), then `pway_shm_commit()`.
  - Input has no event thread: `pway->click` / `pway->click_release` callbacks (wired in `window.c`, implemented in `input.c`) update `mouse_click_x/y`, `check_buttons_collision`, and `mouse_wheel_up/down`, which `button.c`'s `check_button_clicked()` (AABB hit test against `Button.aabb`) and `select_window.c`'s scroll handling read on the next frame.
  - This frontend does not yet drive the actual USB-creation flow — `start_button` in `main.c` has no `execute` callback assigned.
- **`main_gtk.c` + `prufus.h`** — a GTK4 frontend, **not referenced by `source_code/Makefile`'s `OBJS`, so it is not currently compiled**. This is the one that implements the real flow: device dropdown, ISO file picker (`GtkFileDialog`), a confirmation `GtkAlertDialog`, spawning the USB-creation script, and polling `/tmp/prufus/status` for progress (see `stage_t` in `prufus.h`).

**Where the real work happens, regardless of frontend:** shell scripts under `source_code/scripts/`, invoked by absolute paths hardcoded in `scripts_names.h` (`/usr/bin/prufus.sh`, `/usr/libexec/prufus/graphics_sudo`, `/usr/libexec/prufus/get_usb_disks.sh` — these paths only exist after `make install`).
- `prufus.sh <iso_path> <device>` does the actual work: `fdisk` to create a GPT table + EFI/NTFS partitions, `mkfs.fat`/`mkfs.ntfs`, mounts the ISO and USB, `rsync`s everything except `install.wim`, then `wimlib-imagex split`s `install.wim` across the EFI/FAT32 partition's 4GB file-size limit. It writes single-byte stage codes (`stage_t` values) to `/tmp/prufus/status` as it goes, which the GTK frontend tails to update its status label.
- `simulate.sh` mirrors the same stage sequence with `sleep`s instead of real disk operations, for testing the status-polling UI without touching a real disk.
- `graphics_sudo` (→ `pkexec`) and `prufus_sudo`/`prufus_launcher` are the privilege-escalation entry points, since disk formatting needs root but the GUI shouldn't run as root.

**`device.c`** enumerates USB disks directly from `/sys/block` (skips non-`removable` devices, confirms the resolved sysfs path contains `/usb` before treating something as a USB disk) — used independently of scripts for the CPU-rendered frontend's device list; the GTK frontend instead shells out to `get_usb_disks.sh` (a `lsblk` one-liner).
