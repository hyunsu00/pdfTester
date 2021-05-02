# pdfboxTester

## CentOS 7 개발 환경

```bash
# centos7 개발툴 설치
# GCC 및 개발 도구 설치
1. yum group install "Development Tools" -y

# java-jdk 설치
2. yum install java-1.8.0-openjdk-devel.x86_64 -y
vi /root/.bashrc
# 맨 뒤에 추가
export JAVA_HOME=/usr/lib/jvm/java-1.8.0
source /root/.bashrc

echo $JAVA_HOME
# 결과확인
/usr/lib/jvm/java-1.8.0

# cmake 설치
3. yum install cmake -y
```

## Ubuntu 18.04 개발 환경

```bash

```
