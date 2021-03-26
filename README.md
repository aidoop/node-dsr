# node-dsr
Doosan robot client module for nodejs

## Dependencies
### Build and install POCO
```bash
wget https://pocoproject.org/releases/poco-1.10.1/poco-1.10.1-all.tar.gz
tar xvfz poco-1.10.1-all.tar.gz
ls poco-1.10.1-all
mkdir cmake-build
cd cmake-build/
cmake ..
cmake --build . --config Release
sudo make install
```
### NPM
```json
  "dependencies": {
    "bindings": "^1.5.0",
    "node-addon-api": "^3.1.0"
  }
```
