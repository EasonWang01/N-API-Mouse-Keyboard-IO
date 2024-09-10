# N-API-Mouse-Keyboard-IO

**napi-mouse-keyboard-io** is a native Node.js addon built with **N-API** and **C++ bindings**. It allows you to listen to mouse and keyboard events, such as mouse movements, clicks, and key presses, in a cross-platform way (supports **macOS** and **Windows**).

## Features
- Capture mouse movements (`mousemove`)
- Capture mouse clicks (`mousedown`, `mouseup`)
- Capture mouse scroll events (`scroll`)
- Capture keyboard events (`keydown`)

## Prerequisites

Before building or using this project, make sure you have the following installed:

1. **Node.js** (v18 or higher)
2. **node-gyp** (For building native addons)
3. **Build Tools**:
   - **macOS**: Xcode Command Line Tools
   - **Windows**: Visual Studio with C++ build tools

### Installing Node-Gyp
Ensure **node-gyp** is installed globally:
```bash
npm install -g node-gyp
```

On Windows, you might need to install windows-build-tools:

```
npm install --global --production windows-build-tools
```

# Installation
You can install this addon via npm. It will automatically trigger the build process for your platform.

```
npm install napi-mouse-keyboard-io
```

# Building from Source
To build from source, clone the repository and run the following commands:

Clone the repository:

```
git clone https://github.com/EasonWang01/napi-mouse-keyboard-io.git
cd napi-mouse-keyboard-io
```

# Install dependencies:

```
npm install
```

# Build the project:

```
npm run build
```

Optionally, clean the build files:

```
npm run clean
```