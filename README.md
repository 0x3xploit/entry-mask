# EntryMask

**EntryMask** is a lightweight 64-bit ELF packer that applies simple XOR-based encryption to the `.text` section of ELF binaries and prepends a small stub that decrypts and jumps to the original entry point.

---

## 🔐 Features

- Packs **64-bit ELF** binaries compiled with `gcc` or `g++`
- XOR-based encoding of the `.text` section
- Custom decryption stub executes before jumping to OEP
- Automatically updates ELF entry point to stub
- Makes the `.text` segment writable at runtime
- Clean output with color-coded information
- Minimal, fast, and dependency-free implementation in C++

---

## 🧠 How It Works

1. **Reads the input ELF binary** and locates the `.text` section.
2. **Applies XOR encoding** to `.text` using a user-specified key.
3. **Builds a stub** that:
   - Decrypts the `.text` section
   - Jumps to the original entry point
4. **Inserts the stub** into a new aligned memory segment.
5. **Modifies the ELF header** to point the new entry to the stub.
6. Writes the final packed ELF binary.

---

## 🛠️ Usage

```bash
./entrymask <input_binary> -o <output_binary> -k <xor_key>

