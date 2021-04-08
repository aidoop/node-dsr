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
wget https://pocoproject.org/releases/poco-1.10.1/poco-1.10.1.tar.gz
tar xvfz poco-1.10.1-all.tar.gz
cd poco-1.10.1-all
mkdir cmake-build
cd cmake-build/
cmake ..
cmake --build . --config Release
sudo make install
```

## API
...

