
.
├── Makefile
├── include/        # Header files
├── server/         # Server source code
│   ├── server.c
│   ├── common.c
│   └── \*.dat (generated at runtime)
├── client/         # Client source code
│   └── client.c
└── init/           # Initialization utility
└── init\_data.c

````

---

## ⚙️ Build Instructions

Make sure you have `gcc` installed. Then, from the project root:

```bash
make
````

This will build the following executables:

* `server/server`
* `client/client`
* `init/init_data`

### Cleaning up

To remove compiled files and generated data:

```bash
make clean
```

---

## 🚀 Usage

1. **Initialize Data**

   ```bash
   ./init/init_data
   ```

2. **Start the Server**

   ```bash
   ./server/server
   ```

3. **Run the Client**

   ```bash
   ./client/client
   ```

---

## 🛠 Dependencies

* GCC
* pthreads (POSIX threads library)

---

## 📜 License

Specify your license here (e.g., MIT, GPL, etc.).

```

---

Do you want me to also generate a **`LICENSE` file (MIT by default)** so your GitHub repo looks more complete?
```
