from machine import UART
from fpioa_manager import fm
from board import board_info
import sensor, image, lcd, time
import KPU as kpu
import gc, sys
import ustruct
import video
from Maix import GPIO
from machine import Timer
def sd_check():
    import os
    try:
        os.listdir("/sd/.")
    except Exception as e:
        return False
    return True
print(sd_check())
fm.register(10,fm.fpioa.UART2_TX)
fm.register(11,fm.fpioa.UART2_RX)
fm.register(9, fm.fpioa.GPIO0)
fm.register(8, fm.fpioa.GPIO1)
uart_A = UART(UART.UART2, 115200, 8, 0, 0, timeout=1000, read_buf_len=4096)#串口
trig = GPIO(GPIO.GPIO0, GPIO.OUT)#超声波的trig发射引脚
echo = GPIO(GPIO.GPIO1, GPIO.IN)#超声波的echo接受引脚
CsbDistance = 50 #超声波数据
def on_timer(timer):
    trig.value(1)
    time.sleep_us(20)
    trig.value(0)
    t = time.ticks_us()
    while True:
        if echo.value()==1:
            break
        if time.ticks_us()-t>3000:
            return
    t = time.ticks_us()
    while True:
        if echo.value()==0:
            break
        if time.ticks_us()-t>3000:
            return
    t = time.ticks_us()-t
    global CsbDistance
    CsbDistance = int(t*0.017)
tim = Timer(Timer.TIMER0, Timer.CHANNEL0, mode=Timer.MODE_PERIODIC,unit=Timer.UNIT_MS, period=100, callback=on_timer,start=True)
#发送坐标数据给合泰单片机
def sendData(target_id, objects):
    #获取现在想要搜寻的目标（小球0还是装球箱1），得到距离屏幕中心最近的那个目标，如果屏幕中没有任何目标，这个列表长度为零
    targetList = [element for element in list(objects) if element.classid()==target_id]
    #如果这个列表长度不为零，那就表示屏幕中有这个目标，那就开始发送数据
    if targetList:
        nearest_list = sorted(targetList, key=lambda x:abs(x.rect()[0]+x.rect()[2]//2-120))[0].rect()
        Data = ustruct.pack('BBBBBBB',255, nearest_list[0],nearest_list[1],nearest_list[2],nearest_list[3],CsbDistance, target_id)

    else:
        Data = ustruct.pack('BBBBBBB',255, 225, 225, 225, 225,CsbDistance,target_id) #没有目标就发送这个
    uart_A.write(Data)
    #延时一会儿，不要发太快，不然合泰单片机的main函数会没办法执行
    time.sleep_ms(35)
#读取合泰单片机发回来的数据
def readData(readStr, find_flag):
    readStr = uart_A.read()#串口收集数据
    if readStr!= None:
        readStr = ustruct.unpack('B',readStr)[0]
        if (readStr==0) or (readStr==1):
            return readStr
    return find_flag
def lcd_show_except(e):
    import uio
    err_str = uio.StringIO()
    sys.print_exception(e, err_str)
    err_str = err_str.getvalue()
    img = image.Image(size=(224,224))
    img.draw_string(0, 10, err_str, scale=1, color=(0xff,0x00,0x00))
    lcd.display(img)
def main(anchors, labels = None, model_addr="/sd/m.kmodel",sensor_window=(224, 224), lcd_rotation=0, sensor_hmirror=False, sensor_vflip=False):
    sensor.reset()
    sensor.set_pixformat(sensor.RGB565)
    sensor.set_framesize(sensor.QVGA)
    sensor.set_windowing(sensor_window)
    sensor.set_hmirror(sensor_hmirror)
    sensor.set_vflip(sensor_vflip)
    sensor.run(1)
    lcd.init(type=1)
    lcd.rotation(lcd_rotation)
    lcd.clear(lcd.WHITE)
    if not labels:
        with open('labels.txt','r') as f:
            exec(f.read())
    try:
        img = image.Image("startup.jpg")
        lcd.display(img)
    except Exception:
        img = image.Image(size=(320, 240))
        img.draw_string(90, 110, "loading model...", color=(255, 255, 255), scale=2)
        lcd.display(img)

    task = kpu.load(model_addr)
    kpu.init_yolo2(task, 0.5, 0.3, 5, anchors) # threshold:[0,1], nms_value: [0, 1]
    try:
        find_flag=0 #初始化的时候，默认寻找小球
        while True:
            img = sensor.snapshot()
            t = time.ticks_ms()
            objects = kpu.run_yolo2(task, img)
            t = time.ticks_ms() - t
            #获取合泰单片机发过来的串口数据
            find_flag = readData(uart_A.read(),find_flag)

            if objects:

                #发送坐标数据给合泰单片机
                sendData(target_id=find_flag,objects=objects)

                #画出矩形框
                for obj in objects:
                    print("坐标为：",obj)
                    pos = obj.rect()
                    img.draw_rectangle(pos)
                    img.draw_string(pos[0], pos[1], "%s : %.2f" %(labels[obj.classid()], obj.value()), scale=2, color=(255, 0, 0))

            else:
                uart_A.write(ustruct.pack('BBBBBBB',255, 225, 225, 225, 225, CsbDistance, find_flag)) #没有目标
            #标出FPS
            img.draw_string(0, 200, "targetId:%d" %(find_flag), scale=2, color=(255, 0, 0)) #xy极限224
            lcd.display(img)
    except Exception as e:
        raise e
    finally:
        kpu.deinit(task)


if __name__ == "__main__":
    try:
        labels = ['ball', "flag"]
        anchors = [1, 1.2, 2, 3, 4, 3, 6, 4, 5, 6.5]
        main(anchors = anchors, labels=labels, model_addr="/sd/m.kmodel")
    except Exception as e:
        sys.print_exception(e)
        lcd_show_except(e)
    finally:
        gc.collect()
