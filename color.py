
import cv2
import numpy as np

from data216 import decimal_to_high_low_bytes, decimal_to_byte_hex,create_data_packet,send_data, create_buffer
import serial
import time
import serial.tools.list_ports
from main import aruco_det

# def template_match(input_image):
#     # input_image = cv2.imread('input.jpg')
#     x,y = 0,0
#
#     template_image = cv2.imread('13.png')
#     c1 = cv2.imread('12.png')
#     h, w = input_image.shape[0:2]
#     c1 = cv2.resize(c1,(w,h))
#     c1[input_image>125] = 255
#     c1[input_image<126] = 0
#
#     cv2.imshow('c1',c1)
#     if input_image is None or template_image is None:
#         print("无法加载输入图像或模板图像")
#         exit()
#
#     # 获取模板的高度和宽度
#     template_height, template_width = template_image.shape[:2]
#
#     # 执行模板匹配
#     result = cv2.matchTemplate(c1, template_image, cv2.TM_CCOEFF_NORMED)
#
#     # 设置阈值
#     threshold = 0.5
#     y_coords, x_coords = np.where(result >= threshold)
#     # print(y_coords)
#     # 在检测到的区域上绘制矩形
#     for (x, y) in zip(x_coords, y_coords):
#         pass
#         # cv2.rectangle(input_image, (x, y), (x + template_width, y + template_height), (0, 255, 0), 2)
#         # cv2.imshow('Detected Template', input_image)
#         # height_high,height_low = decimal_to_high_low_bytes(y + template_height/2)
#         # width_high, width_low = decimal_to_high_low_bytes(x + template_width/2)
#
#
#     if (x==0):
#         return 0,0
#     else:
#         return int(x + template_width/2),int(y + template_height/2)
#
#     # 显示结果


def green(frame):
    cX, cY, color = 0,0,0
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    lower_red1 = np.array([0, 100, 100])
    upper_red1 = np.array([10, 255, 255])
    lower_red2 = np.array([160, 100, 100])
    upper_red2 = np.array([180, 255, 255])

    # 绿色范围
    lower_green = np.array([40, 40, 40])
    upper_green = np.array([80, 255, 255])

    # 蓝色范围
    lower_blue = np.array([100, 150, 0])
    upper_blue = np.array([140, 255, 255])

    # 创建掩码
    mask_red1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask_red2 = cv2.inRange(hsv, lower_red2, upper_red2)
    mask_red = mask_red1 | mask_red2
    mask_green = cv2.inRange(hsv, lower_green, upper_green)
    mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)

    # 进行形态学操作以去除噪声
    kernel = np.ones((5, 5), np.uint8)
    mask_red = cv2.morphologyEx(mask_red, cv2.MORPH_CLOSE, kernel)
    mask_green = cv2.morphologyEx(mask_green, cv2.MORPH_CLOSE, kernel)
    mask_blue = cv2.morphologyEx(mask_blue, cv2.MORPH_CLOSE, kernel)

    # 查找每种颜色的轮廓
    list_re = []
    masks = {'0': mask_red, '1': mask_green, '2': mask_blue}
    colors = {'red': (0, 0, 255), 'green': (0, 255, 0), 'blue': (255, 0, 0)}
    for color, mask in masks.items():
        contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        for contour in contours:
            if cv2.contourArea(contour) > 500 and cv2.contourArea(contour) < 2000:  # 过滤小面积轮廓
                x, y ,w,h = cv2.boundingRect(contour)
                if w/h < 1.4 and w/h > 0.6:                     # chang  kuan  bi
                    M = cv2.moments(contour)
                    if M['m00'] != 0:
                        cX = int(M['m10'] / M['m00'])
                        cY = int(M['m01'] / M['m00'])
                        # tmp = (color,cX,cY)
                        list_re.append([cX,cY,color])
                        # 在中心点绘制一个圆点
                        cv2.circle(frame, (cX, cY), 7, colors['red'], -1)
                    

    # 显示结果

    if list_re:
        return list_re
    else:
        list_re.append([cX,cY,color])
        return list_re

def main():

    cap = cv2.VideoCapture(0)

    if not cap.isOpened():
        print("无法打开摄像头")

        exit()

    port = '/dev/ttyS0'  # 替换为你的串口号
    baudrate = 9600  # 波特率
    timeout = 0.01  # 超时时间（秒）

    # 打开串口
    ser = serial.Serial(port, baudrate, timeout=timeout)
    print(f"串口 {port} 已打开，波特率: {baudrate}")
    count = 0
    re_aruco1 = [(0,0),(0,0),(0,0),(0,0)]
    while True:
        # 捕获一帧
        ret, frame = cap.read()
        # frame = cv2.resize(frame,(240,160))
        # frame = cv2.imread('1.jpg')
        if not ret:
            print("无法获取帧")
            break
        re_aruco = aruco_det(frame)
        if len(re_aruco) == 4:
            re_aruco1 = re_aruco
        if len(re_aruco) < 4:
            re_aruco = re_aruco1
        x1,y1 = re_aruco[1]
        w = (abs(re_aruco[1][0] - re_aruco[0][0]) + abs(re_aruco[3][0] - re_aruco[2][0])) // 2
        h = (abs(re_aruco[0][1] - re_aruco[2][1]) + abs(re_aruco[3][1] - re_aruco[1][1])) // 2
        cv2.rectangle(frame,(x1,y1),(x1+w,y1+h),(0,0,0),2)
        # print(re_aruco)
        # print(x1,y1)
        re_color = green(frame)
        # cv2.circle(frame, (x1,y1), 7, (0,0,255), -1)
        cv2.imshow('Original Frame', frame)
        if count % 600 == 0:
        # if True:
            print('count:',count, re_aruco)
            for i in range(len(re_color)):

                x2,y2,color = re_color[i]
                y3 = y2 - y1     #y1xiao  x1da   y2xiao  x2da
                x3 = x2 - x1
                # print('xy',y2,y1,y3,x2,x1,x3)

                data = create_buffer(w,h,color,x3,y3)
                # data = create_buffer(146, 83, 0, 78, 42)
                if ser.is_open:
                    print('w,h:', w, h, color, x3, y3)
                    ser.write(data)

                    # ser.read()   jie shou
                    # print('data',data)
        count += 1
        # send_data(data)
        cv2.imwrite('2.png',frame)
        # 按 'q' 键退出
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

# 释放摄像头
#     cap.release()
#     cv2.destroyAllWindows()



if __name__ == '__main__':
    main()