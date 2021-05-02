# pdfTester

## centos7_pdfTester 도커 실행

```bash
docker run -d --privileged -v ~/share_docker:/share_docker --name "centos7-pdfTester" centos:7 init
```

## pdfTestr 복사 및 압축풀기

```bash
docker exec -it centos7-pdfTester bash
mkdir pdfTester && cd pdfTester
# 패키지가 /share_docker 디렉토리에 있다고 가정
cp /share_docker/pdfTester-1.0.0-Linux.tar.gz .
tar -xzvf pdfTester-1.0.0-Linux.tar.gz
cd pdfTester-1.0.0-Linux
```

## pdfiumTester 종속 패키지 설치

```bash
# libpng 설치
1. yum install libpng -y
# intel tbb 설치
2. yum install tbb -y
```

## pdfboxTester 종속 패키지 설치

```bash
# java jre 설치
yum install java-1.8.0-openjdk -y

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

# pdfiumTester CLI

```bash
# 기본
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t png
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t txt
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t png
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t txt
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t png
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t txt

# ppl
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t png --ppl
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t txt --ppl
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t png --ppl
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t txt --ppl
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t png --ppl
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t txt --ppl

# 메모리
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t png --memory
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t txt --memory
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t png --memory
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t txt --memory
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t png --memory
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t txt --memory

# ppl, memory
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t png --ppl --memory
./pdfiumTester -s ./samples/sample01.pdf -r ./result/ -t txt --ppl --memory
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t png --ppl --memory
./pdfiumTester -s ./samples/sample02.pdf -r ./result/ -t txt --ppl --memory
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t png --ppl --memory
./pdfiumTester -s ./samples/sample03.pdf -r ./result/ -t txt --ppl --memory
```

# pdfboxTester CLI

```bash
./pdfboxTester -s ./samples/sample01.pdf -r ./result/ -t png
./pdfboxTester -s ./samples/sample01.pdf -r ./result/ -t txt
./pdfboxTester -s ./samples/sample02.pdf -r ./result/ -t png
./pdfboxTester -s ./samples/sample02.pdf -r ./result/ -t txt
./pdfboxTester -s ./samples/sample03.pdf -r ./result/ -t png
./pdfboxTester -s ./samples/sample03.pdf -r ./result/ -t txt
```
