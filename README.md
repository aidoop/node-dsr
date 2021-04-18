# node-dsr
Doosan robot client module for nodejs

## Doosan Robot Library(libDRFL)
Library Link: https://github.com/doosan-robotics/doosan-robot2/raw/master/common2/lib/foxy/x86_64/libDRFL.a

## Module Limitations
- Supported on linux(64bit)
- Node version > 14.0
- Max instance number < 10 by doosan library limitations

## DSR Library Limitations & Known issues
- openConnection function results in application hangs when some connection exceptions happened.
- need some opearation like homming in teaching pendant.

## Prerequsites
### Linux
#### Build and install POCO
```bash 
wget https://github.com/pocoproject/poco/archive/refs/tags/poco-1.8.0-release.tar.gz 
tar xvfz poco-1.8.0-release.tar.gz
cd poco-poco-1.8.0-release
mkdir cmake-build
cd cmake-build/
cmake ..
cmake --build . --config Release
sudo make install
```

## API
...

