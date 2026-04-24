import os
import struct

import cv2

print("Path to image file")
path = input()

img = cv2.imread(path, cv2.IMREAD_COLOR)
img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
# print(img.shape)  # (height, width, 3) for rgb, tuple
height = img.shape[0]
width = img.shape[1]
# print(img[0][0])  # row, col

uint32 = [width, height]
path = os.path.splitext(path)[0] + ".bin"
with open(path, "wb") as f:
    for v in uint32:
        f.write(struct.pack("<I", v))
    for i in range(height):
        for j in range(width):
            f.write(struct.pack("B", img[i][j][0]))
            f.write(struct.pack("B", img[i][j][1]))
            f.write(struct.pack("B", img[i][j][2]))
