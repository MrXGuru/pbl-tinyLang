<div align="center">
  <h1>⚙️ TinyLang Web Compiler</h1>
  <p><i>A beautiful, interactive, web-based LL(1) compiler and grammar analysis tool for TinyLang.</i></p>
  
  [![Build CI](https://github.com/MrXGuru/pbl-tinyLang/actions/workflows/build.yml/badge.svg)](https://github.com/MrXGuru/pbl-tinyLang/actions/workflows/build.yml)
  [![Live Demo](https://img.shields.io/badge/Live_Demo-pbl--tinylang.onrender.com-success?style=for-the-badge&logo=render)](https://pbl-tinylang.onrender.com/)
  [![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
  [![Python](https://img.shields.io/badge/Python-3.8+-3776AB?logo=python&logoColor=white)](https://www.python.org/)
  [![C Compiler](https://img.shields.io/badge/C_Backend-GCC-00599C?logo=c&logoColor=white)](https://gcc.gnu.org/)
</div>

<br>

Welcome to the **TinyLang Web Compiler**! This project is a fully-featured interactive compiler visualization tool. It consists of an incredibly fast **C-based compiler backend** and a beautiful **Python Flask frontend** to interactively step through the compilation pipeline.

## ✨ Features

- 🔍 **Lexical Analysis:** Breaks down source code into precise tokens (Keywords, Identifiers, Operators, Literals).
- 📐 **LL(1) Grammar Lab:** Interactively analyze formal grammars, compute FIRST & FOLLOW sets, build parse tables, and detect left recursion and conflicts.
- 🔄 **Predictive Parsing:** Stack-based LL(1) predictive parser with step-by-step trace visualization.
- 🌳 **Abstract Syntax Tree (AST):** Interactive, graphical drag-and-drop tree visualization of your parsed code.
- 🗂️ **Symbol Table:** Tracks variables, their types, scopes, and reference counts securely.
- ⚡ **Static Code Analysis:** Estimates Big-O time complexity (e.g. $O(n^2)$) and detects potential performance hotspots.
- 🚀 **Code Optimization:** Automatically applies constant folding, dead code elimination, and loop-invariant code motion.

---

## 🏗️ Architecture

This project is built using a hybrid architecture for maximum performance and portability:

1. **The Core Compiler (`/compiler`)**: Written entirely in **C** for blazing fast lexical analysis, parsing, and optimization.
2. **The Web API (`app.py`)**: Written in **Python (Flask)**. It acts as a bridge, executing the C binary as a subprocess and formatting the JSON output.
3. **The Interactive UI (`/templates`)**: Pure HTML/CSS/JS (Vanilla) for a lightweight, beautiful glassmorphism-styled dashboard.

---

## 🚀 Getting Started Locally

### Prerequisites
- **C Compiler** (`gcc` or `clang`)
- **Python 3.8+**
- **Docker** (Optional, for containerized builds)

### Option A: Standard Build
1. **Compile the C Backend**
   ```bash
   # On Mac/Linux
   make

   # On Windows
   build.bat
   ```
2. **Install Python Dependencies**
   ```bash
   pip install -r requirements.txt
   ```
3. **Run the Server**
   ```bash
   python app.py
   ```
   Navigate to `http://localhost:5000` in your browser.

### Option B: Docker Build (Recommended)
You can run the entire stack inside an isolated Docker container:
```bash
docker build -t tinylang-web .
docker run -p 5000:5000 tinylang-web
```

---

## ☁️ Deployment

This project is fully containerized and production-ready. 

**Deploying to Render or Railway:**
1. Connect your GitHub repository to your PaaS provider.
2. The platform will automatically detect the `Dockerfile`.
3. It will install GCC, compile the C code, install Python, and start the `gunicorn` production server automatically!

---

## 🤝 Contributing
Contributions are always welcome! Whether it's adding new grammar rules, improving the AST visualization, or optimizing the C backend:
1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License
Distributed under the MIT License. See `LICENSE` for more information.
