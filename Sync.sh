#!/bin/bash

set -e  # exit on error

echo "==> Installing base packages..."
sudo pacman -S --needed git base-devel 7zip mpv bluez btop libreoffice cmus inkscape ufw

echo "==> Building suckless tools..."

cd dwm
make clean install
cd ..

cd slstatus
make clean install
cd ..

cd dmenu
make clean install
cd ..

echo "==> Installing yay (AUR helper)..."
git clone https://aur.archlinux.org/yay.git
cd yay
makepkg -si --noconfirm
cd ..
rm -rf yay

echo "==> Installing Librewolf..."
yay -S --noconfirm librewolf-bin

echo "==> Enabling firewall..."
sudo ufw enable

echo "==> Moving .Xresources to home..."
mv -v .Xresources "$HOME/.Xresources"

echo "==> Extracting Zip.7z..."
7z x Zip.7z

echo "==> Moving .librewolf config..."
if [ -d ".librewolf" ]; then
    mv -v .librewolf "$HOME/"
else
    echo "Warning: .librewolf folder not found after extraction"
fi

echo "==> Done!"