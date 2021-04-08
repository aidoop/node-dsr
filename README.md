# node-dsr
Doosan robot client module for nodejs

## Limitations
- Only supported on linux and windows.
- Node version > 14.0
- Max instance number < 10

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

