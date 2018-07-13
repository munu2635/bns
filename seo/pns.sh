# 쉘프로그램 실습
# 명령어로 실행하기.
#!/bin/sh
read state <<< https://192.168.50.15::8085/?action=stream
while :
do
  if [ state != "실행되지 않을 때 값" ] then
    echo "start yolo"
    cd ~/yolov3/darknet
    ./darknet detector demo data/coco.data cfg/yolov3.cfg yolov3.weights http://192.168.0.80:8080/video?dummy=param.mjpg -i 0
    echo "start openCV"
    cd ~/yolov3/darknet/src/dns
    ./test
done
