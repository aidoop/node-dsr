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
wget https://pocoproject.org/releases/poco-1.8.0/poco-1.8.0.tar.gz
tar xvfz poco-1.8.0.tar.gz
cd poco-1.8.0
mkdir cmake-build
cd cmake-build/
cmake ..
cmake --build . --config Release
sudo make install
```

## API
...

