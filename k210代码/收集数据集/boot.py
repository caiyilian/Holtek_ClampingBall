
import sensor, image, lcd, time
import os
print(os.listdir("/sd/."))
lcd.init(freq=15000000)
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
lcd.rotation(0)
sensor.set_windowing((224,224))
sensor.set_hmirror(False)
sensor.set_vflip(False)

sensor.run(1)
sensor.skip_frames(30)

import video
for j in range(4):
    v = video.open("/sd/"+str(j)+".avi", audio = False, record=1, interval=200000, quality=90)

    tim = time.ticks_ms()
    for i in range(500):
        tim = time.ticks_ms()
        img = sensor.snapshot()
        lcd.display(img)
        img_len = v.record(img)
        # print("record",time.ticks_ms() - tim)

    print("record_finish")
    v.record_finish()
    v.__del__()

lcd.clear()
