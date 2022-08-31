# 安装编译器和相关依赖
yum install clang

# 下载代码
git clone https://github.com/wmjtyd/quickfix-rs.git

## 下载和编译依赖代码 
git submodule update --init

编译quickfix
cd ./vendor/quickfix-cpp
cmake .
make all

# 编译主工程代码
cd xxx/quickfix-rs

rust使用 
cargo build

cpp使用 
cmake .
make tradeclient-ccapi