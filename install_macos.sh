#!/usr/bin/env bash
# Installs Io from this repository on macOS by ensuring dependencies,
# building the project, and copying artifacts into the chosen prefix.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_TYPE="${BUILD_TYPE:-Release}"
BUILD_DIR="${BUILD_DIR:-$REPO_ROOT/build/macos-$BUILD_TYPE}"
INSTALL_PREFIX="${INSTALL_PREFIX:-/usr/local}"

info() { printf '\033[0;34m==> %s\033[0m\n' "$1"; }
warn() { printf '\033[1;33mWarning:\033[0m %s\n' "$1" >&2; }
die() { printf '\033[0;31mError:\033[0m %s\n' "$1" >&2; exit 1; }

require_macos() {
    [[ "$(uname -s)" == "Darwin" ]] || die "This installer only targets macOS.";
}

require_clt() {
    if ! xcode-select -p >/dev/null 2>&1; then
        warn "Xcode Command Line Tools not found. Run 'xcode-select --install' then re-run this script."
        exit 1
    fi
}

ensure_homebrew() {
    if command -v brew >/dev/null 2>&1; then
        return
    fi
    warn "Homebrew not found. Installing it now."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    if command -v /opt/homebrew/bin/brew >/dev/null 2>&1; then
        eval "$(/opt/homebrew/bin/brew shellenv)"
    elif command -v /usr/local/bin/brew >/dev/null 2>&1; then
        eval "$(/usr/local/bin/brew shellenv)"
    else
        die "Homebrew installation did not add brew to PATH."
    fi
}

install_dependencies() {
    local packages=(cmake pkg-config)
    for pkg in "${packages[@]}"; do
        if brew list "$pkg" >/dev/null 2>&1; then
            info "$pkg already installed"
        else
            info "Installing $pkg"
            brew install "$pkg"
        fi
    done
}

update_submodules() {
    info "Updating git submodules"
    git -C "$REPO_ROOT" submodule update --init --recursive
}

configure() {
    info "Configuring build directory $BUILD_DIR"
    cmake -S "$REPO_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
}

build() {
    info "Building Io (type: $BUILD_TYPE)"
    cmake --build "$BUILD_DIR" --parallel
}

install_io() {
    info "Installing Io into $INSTALL_PREFIX"
    local needs_sudo=0 parent
    if [ -d "$INSTALL_PREFIX" ]; then
        [[ -w "$INSTALL_PREFIX" ]] || needs_sudo=1
    else
        parent="$(dirname "$INSTALL_PREFIX")"
        [[ -w "$parent" ]] || needs_sudo=1
    fi

    if (( needs_sudo )); then
        sudo cmake --install "$BUILD_DIR" --prefix "$INSTALL_PREFIX"
    else
        cmake --install "$BUILD_DIR" --prefix "$INSTALL_PREFIX"
    fi

    info "Io installed. Add $INSTALL_PREFIX/bin to PATH if needed."
}

main() {
    require_macos
    require_clt
    ensure_homebrew
    install_dependencies
    update_submodules
    configure
    build
    install_io
}

main "$@"
