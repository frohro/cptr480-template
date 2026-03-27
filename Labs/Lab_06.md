# CPTR 480 — Lab 6
## UAC2 + CDC Composite Device — Part 1: Enumeration
**Date:** Tuesday, May 5, 2026 | 2:00–5:00 p.m.
**Due:** All deliverables committed to your GitHub repo by **2 p.m., Tuesday May 12**

---

### Learning Objectives
By the end of this lab you will be able to:
- Construct a correct UAC2 + CDC composite USB descriptor set for a capture-only device
- Explain the purpose of every entity in the USB Audio Class 2 control hierarchy (Clock Source, Input Terminal, Output Terminal, Feature Unit)
- Enumerate the composite device and verify all descriptors with `lsusb -v`
- Continue sending LO tune commands over CDC while UAC2 is present (no audio streaming yet)

---

### What You Need
- 2026 board with YD-RP2040
- Pico Debug Stack
- CDC firmware from Lab 5 as your starting point
- TinyUSB (Pico SDK)
- USB-C data cable (data-capable)
- Linux host with `usbutils` (`sudo apt install usbutils` if missing)
- Your Assignment 5 descriptor notes — you need them today

> **This is the hardest descriptor problem in the course.** Allow the full lab period. Read Assignment 5 before showing up.

---

### Overview

Labs 6 and 7 are a two-week climb:
- **Lab 6 (this lab):** Get the composite UAC2 + CDC device to enumerate correctly. No audio samples move yet. The deliverable is `lsusb -v` output showing every audio descriptor correctly parsed, plus a CDC tune command that still works.
- **Lab 7 (next week):** Wire the PCM1808 DMA pipeline into the UAC2 isochronous stream. Quisk waterfall goes live.

The hardest part is always the descriptors. A single wrong `bLength`, wrong interface number, or wrong entity ID causes silent enumeration failure or a kernel driver error. Take it field by field.

---

## Part 1 — Plan the Composite Interface Layout (~20 min)

Before touching code, draw (on paper or in your report) the complete interface layout. Every interface needs a number assigned now; changing them later breaks everything.

Suggested layout:

| Interface | Class | Subclass | Protocol | Purpose |
|---|---|---|---|---|
| 0 | 0x02 CDC Control | 0x02 ACM | 0x00 | CDC management |
| 1 | 0x0A CDC Data | — | — | CDC bulk data |
| 2 | 0x01 Audio | 0x01 Control | 0x20 UAC2 | Audio Control |
| 3 | 0x01 Audio | 0x02 Streaming | 0x20 UAC2 | Audio Streaming (alt 0) |
| 3 | 0x01 Audio | 0x02 Streaming | 0x20 UAC2 | Audio Streaming (alt 1, active) |

Two IADs (Interface Association Descriptors) are required:
- IAD 1: `bFirstInterface=0`, `bInterfaceCount=2` (CDC)
- IAD 2: `bFirstInterface=2`, `bInterfaceCount=2` (Audio)

Commit your interface table to `lab6/descriptor_plan.txt` before writing any C.

---

## Part 2 — Update `tusb_config.h` (~15 min)

Starting from your Lab 5 `tusb_config.h`, add audio support:

```c
#define CFG_TUD_AUDIO           1
// REQUIRED: total byte length of the audio descriptor block (IAD through last EP).
// TinyUSB's audio_device.h produces a hard #error if this macro is absent.
// See usb_descriptors.c for the full 125-byte breakdown.
#define CFG_TUD_AUDIO_FUNC_1_DESC_LEN       125
// Number of audio functions (we have one: stereo capture)
#define CFG_TUD_AUDIO_FUNC_1_N_AS_INT   1   // one streaming interface
// TX = device transmits to host (capture / USB IN endpoint)
#define CFG_TUD_AUDIO_FUNC_1_N_TX       1
#define CFG_TUD_AUDIO_FUNC_1_N_RX       0   // no playback
// Control EP buffer
#define CFG_TUD_AUDIO_FUNC_1_CTRL_BUF_SZ   64
// REQUIRED: must be set explicitly — defaults to 0, which disables the IN endpoint.
#define CFG_TUD_AUDIO_ENABLE_EP_IN          1
// Audio TX FIFO size (bytes).
// Nominal: 48 samples × 2 ch × 4 bytes = 384. Double-buffer: 768.
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX   392   // 49 samples worst-case
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ  768
```

Keep `CFG_TUD_CDC 1` and your CDC settings from Lab 5.

---

## Part 2b — Update CDC Endpoint Addresses in `usb_descriptors.c`

Before writing a single audio descriptor, update the CDC endpoint addresses you carried forward from Lab 5. Audio **claims EP1** (address `0x81` IN for data, `0x01` OUT for feedback). If you leave your Lab 5 CDC endpoints unchanged they will collide with the audio endpoints and either CDC or audio will fail silently.

| Role | Lab 5 address | Lab 6 address |
|---|---|---|
| CDC notification (IN, interrupt) | `0x81` | `0x83` |
| CDC data OUT (bulk) | `0x02` | `0x04` |
| CDC data IN (bulk) | `0x82` | `0x84` |

Update the `EPNUM_CDC_*` defines (or equivalent raw endpoint address bytes) in your `usb_descriptors.c` before proceeding.

---

## Part 3 — Write the Audio Descriptor Block (~75 min)

This is the core of Lab 6. Each sub-section is a discrete descriptor; write and verify them in order.

### 3a — Audio Control Interface and Header

```c
// Audio Control Interface descriptor
0x09, 0x04,             // bLength, bDescriptorType (INTERFACE)
0x02,                   // bInterfaceNumber = 2
0x00,                   // bAlternateSetting
0x00,                   // bNumEndpoints (Audio Control has no endpoints)
0x01,                   // bInterfaceClass (AUDIO)
0x01,                   // bInterfaceSubClass (AUDIO_CONTROL)
0x20,                   // bInterfaceProtocol (UAC2)
0x00,                   // iInterface

// Class-specific AC Interface Header (UAC2, Section 4.7.2)
0x09, 0x24,             // bLength=9, bDescriptorType=CS_INTERFACE
0x01,                   // bDescriptorSubtype=HEADER
0x00, 0x02,             // bcdADC = 0x0200 (UAC2)
0x08,                   // bCategory = 0x08 (I/O box — generic)
U16_TO_U8S_LE(AUDIO_CTRL_TOTAL_LEN), // wTotalLength — sum of ALL class-specific AC descs
0x00,                   // bmControls
```

> `AUDIO_CTRL_TOTAL_LEN` = sum of Header + Clock Source + Input Terminal + Output Terminal descriptors. Calculate it after writing them all, then come back and fill it in.

### 3b — Clock Source Entity (ID = 1)

```c
// Clock Source (UAC2, Section 4.7.2.1)
0x08, 0x24,             // bLength=8, bDescriptorType=CS_INTERFACE
0x0A,                   // bDescriptorSubtype=CLOCK_SOURCE
0x01,                   // bClockID = 1
0x01,                   // bmAttributes: internal fixed clock
0x07,                   // bmControls: frequency read/write, validity read
0x00,                   // bAssocTerminal
0x00,                   // iClockSource
```

### 3c — Input Terminal (ID = 2 — represents the PCM1808 ADC)

```c
// Input Terminal (UAC2, Section 4.7.2.4)
0x11, 0x24,             // bLength=17, bDescriptorType=CS_INTERFACE
0x02,                   // bDescriptorSubtype=INPUT_TERMINAL
0x02,                   // bTerminalID = 2
U16_TO_U8S_LE(0x0201), // wTerminalType = 0x0201 (Microphone / line in)
0x00,                   // bAssocTerminal
0x01,                   // bCSourceID = 1 (our Clock Source)
0x02,                   // bNrChannels = 2 (stereo I/Q)
U32_TO_U8S_LE(0x00000003), // bmChannelConfig: L + R
0x00,                   // iChannelNames
U16_TO_U8S_LE(0x0000), // bmControls
0x00,                   // iTerminal
```

### 3d — Output Terminal (ID = 3 — represents the USB pipe to the host)

```c
// Output Terminal (UAC2, Section 4.7.2.5)
0x0C, 0x24,             // bLength=12, bDescriptorType=CS_INTERFACE
0x03,                   // bDescriptorSubtype=OUTPUT_TERMINAL
0x03,                   // bTerminalID = 3
U16_TO_U8S_LE(0x0101), // wTerminalType = 0x0101 (USB streaming)
0x00,                   // bAssocTerminal
0x02,                   // bSourceID = 2 (Input Terminal)
0x01,                   // bCSourceID = 1 (Clock Source)
U16_TO_U8S_LE(0x0000), // bmControls
0x00,                   // iTerminal
```

### 3e — Audio Streaming Interface (alt 0: zero-bandwidth)

```c
0x09, 0x04,             // bLength, bDescriptorType (INTERFACE)
0x03,                   // bInterfaceNumber = 3
0x00,                   // bAlternateSetting = 0 (zero bandwidth — host selects on open)
0x00,                   // bNumEndpoints = 0
0x01, 0x02, 0x20,       // Audio / Streaming / UAC2
0x00,                   // iInterface
```

### 3f — Audio Streaming Interface (alt 1: active)

```c
0x09, 0x04,             // INTERFACE
0x03,                   // bInterfaceNumber = 3
0x01,                   // bAlternateSetting = 1
0x02,                   // bNumEndpoints = 2 (data IN + feedback OUT)
0x01, 0x02, 0x20,       // Audio / Streaming / UAC2
0x00,

// Class-specific AS Interface (UAC2, Section 4.9.2)
0x10, 0x24,             // bLength=16, CS_INTERFACE
0x01,                   // bDescriptorSubtype=AS_GENERAL
0x03,                   // bTerminalLink = 3 (Output Terminal)
0x00,                   // bmControls
0x01,                   // bFormatType = FORMAT_TYPE_I
U32_TO_U8S_LE(0x00000001), // bmFormats = PCM
0x02,                   // bNrChannels = 2
U32_TO_U8S_LE(0x00000003), // bmChannelConfig: L + R
0x00,                   // iChannelNames

// Type I Format Descriptor (UAC2, Section 2.3.1.6)
0x06, 0x24,             // bLength=6, CS_INTERFACE
0x02,                   // bDescriptorSubtype=FORMAT_TYPE
0x01,                   // bFormatType=FORMAT_TYPE_I
0x04,                   // bSubslotSize = 4 (32-bit container)
0x18,                   // bBitResolution = 24 (significant bits)

// Isochronous Data Endpoint (IN, device->host)
// wMaxPacketSize = 392 bytes (49 samples × 2 ch × 4 bytes, worst case)
0x07, 0x05,             // bLength, bDescriptorType (ENDPOINT)
0x81,                   // bEndpointAddress = EP1 IN
0x05,                   // bmAttributes = isochronous, asynchronous
U16_TO_U8S_LE(392),     // wMaxPacketSize
0x01,                   // bInterval = 1 (every frame = 1 ms for Full Speed)

// Class-specific AS Isochronous Endpoint (UAC2, Section 4.10.1.2)
0x08, 0x25,             // bLength=8, CS_ENDPOINT
0x01,                   // bDescriptorSubtype=EP_GENERAL
0x00,                   // bmAttributes
0x00,                   // bmControls
0x00,                   // bLockDelayUnits
U16_TO_U8S_LE(0x0000), // wLockDelay

// Feedback Endpoint (OUT, host->device) — for adaptive sync
0x07, 0x05,             // bLength, ENDPOINT
0x01,                   // bEndpointAddress = EP1 OUT
0x11,                   // bmAttributes = isochronous, feedback
U16_TO_U8S_LE(3),       // wMaxPacketSize = 3 (Full Speed 10.14 feedback)
0x05,                   // bInterval (2^(5-1) = 16 frames between feedback packets)
```

> **Why 32-bit containers (`bSubslotSize=4`) instead of 24-bit (`bSubslotSize=3`)?** Three-byte subframes are technically correct per the UAC2 spec but are rejected by many Windows and Linux kernel drivers. Sending the 24-bit sample zero-padded in the LSB of a 32-bit word is universally compatible.

> **Copilot use:** Copilot can help compute `AUDIO_CTRL_TOTAL_LEN` and fill in the configuration descriptor `wTotalLength`. Always verify by hand: add up every `bLength` field in the configuration and check against the value Copilot generates.

---

## Part 4 — Enumeration Verification (~30 min)

Flash and run. On the host:

```bash
lsusb
```
Both CDC and audio interfaces should appear.

```bash
lsusb -v -d <vendorID>:<productID> 2>/dev/null | tee lab6/lsusb_output.txt
```

Work through the output and confirm:
- Two IADs present (one for CDC, one for Audio)
- AudioControl interface: Clock Source, Input Terminal (type 0x0201), Output Terminal (type 0x0101)
- AudioStreaming interface: two altsettings, alt 1 has two endpoints
- `bSubslotSize = 4`, `bBitResolution = 24`
- Feedback endpoint present (type isochronous, usage feedback)

Check kernel messages for errors:
```bash
dmesg | tail -20
```
The UAC2 driver (`snd-usb-audio`) should bind to the audio interfaces. Even with no audio data flowing yet, it should say something like `USB Audio Class 2.0 device` and create an ALSA card.

```bash
arecord -l
```
Your device should appear as a capture device. That means enumeration succeeded.

> **Deliverable:** `lab6/lsusb_output.txt` and `lab6/dmesg_output.txt` showing clean enumeration.

---

## Part 5 — CDC Tune Still Works (~15 min)

The CDC interface from Lab 5 must still function alongside UAC2:

```bash
minicom -D /dev/ttyACM0 -b 115200
```
Send `F7100000` → confirm Si5351a tunes (logic analyzer or oscilloscope on CLK0).

If `ttyACM0` is gone (wrong interface number ordering or missing IAD), debug the CDC IAD and interface numbers before moving on. A common mistake: IAD `bFirstInterface` pointing at the wrong interface number after adding audio interfaces.

> **Deliverable:** `lab6/tune_demo.txt` showing CDC tune command still works with UAC2 present.

---

## Part 6 — Troubleshooting Guide (reference)

| Symptom | Likely cause |
|---|---|
| Build error: `CFG_TUD_AUDIO_FUNC_1_DESC_LEN` not defined | This macro is required by `audio_device.h` and produces a hard `#error` if absent. Add it to `tusb_config.h` |
| Audio endpoint does not appear / `arecord -l` shows no card | `CFG_TUD_AUDIO_ENABLE_EP_IN` is missing — it defaults to 0, disabling the IN endpoint entirely; add `#define CFG_TUD_AUDIO_ENABLE_EP_IN 1` |
| `lsusb` shows device but `arecord -l` shows nothing | UAC2 driver failed to bind — check `dmesg`; often a wrong `bcdADC` or bad `wTotalLength` |
| `dmesg` shows `cannot get freq at ep 0x81` | Feedback endpoint descriptor wrong (`bmAttributes` or `bInterval`) |
| CDC disappeared after adding UAC2 | CDC endpoints collide with audio EP1: update CDC addresses (notification `0x81→0x83`, bulk `0x02/0x82→0x04/0x84`), or check IAD `bFirstInterface`/`bInterfaceCount` |
| `lsusb -v` truncates at configuration descriptor | `wTotalLength` is too small |
| Kernel oops / USB reset loop | `bLength` of one descriptor is wrong, causing the parser to lose sync |

Document any issues you hit and how you resolved them in `lab6/lab6_report.md`.

---

### Commit Checklist

By 2:00 p.m., Tuesday May 12:

- [ ] `lab6/CMakeLists.txt`
- [ ] `lab6/tusb_config.h`
- [ ] `lab6/usb_descriptors.c` — UAC2 + CDC composite descriptors
- [ ] `lab6/main.c` — TinyUSB task loop (CDC parser from Lab 5 carried forward; no audio callback yet)
- [ ] `lab6/descriptor_plan.txt` — interface layout table from Part 1
- [ ] `lab6/lsusb_output.txt` — full descriptor dump showing UAC2 audio interfaces
- [ ] `lab6/dmesg_output.txt` — kernel messages showing `snd-usb-audio` binding
- [ ] `lab6/tune_demo.txt` — CDC tune command working alongside UAC2
- [ ] `lab6/lab6_report.md` — enumeration problems hit, how resolved, and lessons learned
