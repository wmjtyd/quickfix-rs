# 安装编译器和相关依赖

## centos

`yum install clang`

## ubuntu

```bash
sudo apt install clang
sudo apt install libssl-dev
sudo apt-get install zlib1g-dev
```

# 下载代码
```bash
git clone https://github.com/wmjtyd/quickfix-rs.git
```

## 下载和编译依赖代码 
```bash
# 需要提前更新和初始化子模块
git submodule update --init

# 编译quickfix
cd ./vendor/quickfix-cpp
cmake .
make all
```

# 编译主工程代码
```bash
cd <your_path>/quickfix-rs

# rust使用 
cargo build


# cpp使用 
cmake .
make tradeclient-ccapi
```

# 如何运行

## 测试网环境变量设置(若不使用测试网，则跳过这步)

> 如果需要连接测试网进行调试，设置以下cmake变量

币安测试网baseurl

在 CMakeList.txt 加入 basurl 的定义：
```
add_compile_definitions(CCAPI_BINANCE_URL_REST_BASE="https://testnet.binance.vision/api")
```
重新编译程序
```
cmake .
make tradeclient-ccapi
```

#### 生成测试网api-key
在 https://testnet.binance.vision/ 网站上注册测试账号和生成 api-key


# 设置api-key
```
export BINANCE_API_KEY=xxx
export BINANCE_API_SECRET=xxx
```



### 运行程序
```bash
./cxx/tradeclient-ccapi/tradeclient-ccapi -e binance -b BTCUSDT --side 1 -q 0.123 -p 0.321 --stop_price 0.22 -t 1 -o 4
```

## 测试网资料
### 币安官方测试使用文档
https://testnet.binance.vision/

How can I use the Spot Test Network?

Step 1: Log in on this website, and generate an API Key.
 
Step 2: Follow the official documentation of the Spot API, replacing the URLs of the endpoints with the following values:

|Spot API URL|	Spot Test Network URL|
|----|----|
|https://api.binance.com/api|	https://testnet.binance.vision/api
|wss://stream.binance.com:9443/ws|	wss://testnet.binance.vision/ws|
|wss://stream.binance.com:9443/stream|	wss://testnet.binance.vision/stream|

### 测试网faq链接
https://www.binance.com/en/support/faq/ab78f9a1b8824cf0a106b4229c76496d

### 测试网界面
https://testnet.binancefuture.com/en/futures/BTCUSDT
