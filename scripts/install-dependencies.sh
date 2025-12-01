sudo apt-get install -y build-essential git make \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libfribidi-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev libthai-dev \
libpipewire-0.3-dev libwayland-dev libdecor-0-dev liburing-dev

wget https://download.savannah.gnu.org/releases/freetype/freetype-2.14.1.tar.xz

tar -xf freetype-2.14.1.tar.xz
cd freetype-2.14.1

sh autogen.sh
./configure # this will install in /usr/local. See --help for moving target
make
sudo make install

cd ..

wget https://github.com/libsdl-org/SDL/releases/download/release-3.2.26/SDL3-3.2.26.zip
wget https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.zip

# 1. setup
mkdir SDL3_setup && cd SDL3_setup
mv ../SDL*.zip .
mkdir build

# Repeat step 2 and 3 for each packages you want to install

# 2. prepare for the desired package
unzip SDL3-3.2.26.zip
# optional step
rm -rf ./build/* # clean folder for next installation

# 3. build and install
cd build
cmake ../SDL3-3.2.26/CMakeLists.txt -B .
make
sudo make install

cd ..

unzip SDL3_ttf-3.2.2.zip
# optional step
rm -rf ./build/* # clean folder for next installation

# 3. build and install
cd build
cmake ../SDL3_ttf-3.2.2/CMakeLists.txt -B .
make
sudo make install
