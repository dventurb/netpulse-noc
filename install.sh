#!/bin/bash

# Debian / Ubuntu
install_with_apt() {
  sudo apt update
  sudo apt install -y \
    build-essential \
    pkg-config \
    libgtk-4-dev \
    libsodium-dev
}

# Red Hat / CentOS
install_with_dnf() {
  sudo dnf install -y \
    gcc \
    make \
    pkgconf-pkg-config \
    gtk4-devel \
    libsodium-devel
}

# Arch Linux
install_with_pacman() {
  sudo pacman -Sy --noconfirm \
    base-devel \
    pkgconf \
    gtk4 \
    libsodium
}

# MacOS (Brew)
install_with_homebrew() {
  brew install \
    pkg-config \
    gtk4 \
    libsodium
}

# Windows (MSYS2)
install_with_msys2() {
  pacman -Sy --noconfirm \
      mingw-w64-ucrt-x86_64-gcc \
      mingw-w64-ucrt-x86_64-gtk4 \
      mingw-w64-ucrt-x86_64-libsodium \
      make \
      pkgconf \
      git
}

echo "Detecting OS..."

# Windows (MSYS2)
if [[ -n "$MSYSTEM" ]]; then
    install_with_msys2

# macOS
elif [[ "$(uname)" == "Darwin" ]]; then
    install_with_homebrew

# Arch Linux
elif [ -f /etc/arch-release ]; then
    install_with_pacman

# Debian / Ubuntu
elif [ -f /etc/debian_version ]; then
    install_with_apt

# Fedora / RHEL
elif [ -f /etc/redhat-release ] || [ -f /etc/centos-release ]; then
    install_with_dnf

else
  echo "Your OS is not supported currently."
  echo "You need to manualy install those packages: gcc, make, pkg-config, libgtk-4-dev/gtk4-devel/gtk4, libsodium-dev/libsodium-devel/libsodium"
  exit 1
fi

make

echo "INSTALLATION FINISHED"
