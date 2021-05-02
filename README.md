# pdfTester

## pdfTester 도커 실행

### centos7

```bash
docker run -d --privileged -v ~/share_docker:/share_docker --name "centos7-pdfTester" centos:7 init
```

## pdfTestr 복사 및 압축풀기

### centos7

```bash
docker exec -it centos7-pdfTester bash
mkdir pdfTester && cd pdfTester
# 패키지가 /share_docker 디렉토리에 있다고 가정
cp /share_docker/pdfTester-1.0.0-Linux.tar.gz .
tar -xzvf pdfTester-1.0.0-Linux.tar.gz
cd pdfTester-1.0.0-Linux
# 변환 결과 폴더 생성(굳이 필요 없음)
mkdir pdfboxTester/result
mkdir pdfiumTester/result
```

## pdfiumTester 종속 패키지 설치

### centos7(yum)

```bash
# 패키지 확인시
ldd ./pdfiumTester

# libpng 설치
1. yum install libpng -y
# intel tbb 설치
2. yum install tbb -y
```

### ubuntu18.04(apt)

```bash
# 패키지 확인시
ldd ./pdfiumTester

# libpng 설치
1. apt install libpng16-16 -y
# intel tbb 설치
2. apt install libtbb-dev -y
```

## pdfboxTester 종속 패키지 설치

### centos7(yum)

```bash
# 패키지 확인시
ldd ./pdfboxTester

# java jre 설치
1. yum install java-1.8.0-openjdk -y

vi /root/.bashrc
# 맨 뒤에 추가
export JAVA_HOME=/usr/lib/jvm
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JAVA_HOME/jre/lib/amd64:$JAVA_HOME/jre/lib/amd64/server

source /root/.bashrc

echo $JAVA_HOME
# 결과확인
/usr/lib/jvm

echo $LD_LIBRARY_PATH
# 결과 확인
:/usr/lib/jvm/jre/lib/amd64:/usr/lib/jvm/jre/lib/amd64/server
```

### ubuntu18.04(apt)

```bash
# 패키지 확인시
ldd ./pdfboxTester

# vim 에디터 설치
apt install vim -y

# java jre 설치
1. apt install openjdk-8-jre -y

vi /root/.bashrc
# 맨 뒤에 추가
export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JAVA_HOME/jre/lib/amd64:$JAVA_HOME/jre/lib/amd64/server

source /root/.bashrc

echo $JAVA_HOME
# 결과확인
/usr/lib/jvm/java-8-openjdk-amd64

echo $LD_LIBRARY_PATH
# 결과 확인
:/usr/lib/jvm/java-8-openjdk-amd64/jre/lib/amd64:/usr/lib/jvm/java-8-openjdk-amd64/jre/lib/amd64/server
```

# pdfiumTester CLI

```bash
# 기본
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t png > ./pdfium.sample01.png.txt
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t txt > ./pdfium.sample01.txt.txt
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t png > ./pdfium.sample02.png.txt
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t txt > ./pdfium.sample02.txt.txt
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t png > ./pdfium.sample03.png.txt
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t txt > ./pdfium.sample03.txt.txt

# ppl
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t png --ppl > ./pdfium.sample01.png.ppl.txt
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t txt --ppl > ./pdfium.sample01.txt.ppl.txt
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t png --ppl > ./pdfium.sample02.png.ppl.txt
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t txt --ppl > ./pdfium.sample02.txt.ppl.txt
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t png --ppl > ./pdfium.sample03.png.ppl.txt
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t txt --ppl > ./pdfium.sample03.txt.ppl.txt

# 메모리
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t png --memory > ./pdfium.sample01.png.memory.txt
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t txt --memory > ./pdfium.sample01.txt.memory.txt
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t png --memory > ./pdfium.sample02.png.memory.txt
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t txt --memory > ./pdfium.sample02.txt.memory.txt
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t png --memory > ./pdfium.sample03.png.memory.txt
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t txt --memory > ./pdfium.sample03.txt.memory.txt

# ppl, memory
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t png --ppl --memory > ./pdfium.sample01.png.ppl.memory.txt
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t txt --ppl --memory > ./pdfium.sample01.txt.ppl.memory.txt
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t png --ppl --memory > ./pdfium.sample02.png.ppl.memory.txt
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t txt --ppl --memory > ./pdfium.sample02.txt.ppl.memory.txt
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t png --ppl --memory > ./pdfium.sample03.png.ppl.memory.txt
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t txt --ppl --memory > ./pdfium.sample03.txt.ppl.memory.txt
```

# pdfboxTester CLI

```bash
./pdfboxTester -s ./samples/sample01.pdf -r ./result/ -t png > ./pdfbox.sample01.png.txt
./pdfboxTester -s ./samples/sample01.pdf -r ./result/ -t txt > ./pdfbox.sample01.txt.txt
./pdfboxTester -s ./samples/sample02.pdf -r ./result/ -t png > ./pdfbox.sample02.png.txt
./pdfboxTester -s ./samples/sample02.pdf -r ./result/ -t txt > ./pdfbox.sample02.txt.txt
./pdfboxTester -s ./samples/sample03.pdf -r ./result/ -t png > ./pdfbox.sample03.png.txt
./pdfboxTester -s ./samples/sample03.pdf -r ./result/ -t txt > ./pdfbox.sample03.txt.txt
```
