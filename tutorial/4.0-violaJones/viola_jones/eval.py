import math
import cv2
from .base import *
from . import config

class Detection():
    def __init__(self, x, y, w, h, conf):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.conf = conf

    def __repr__(self):
        return f"({self.x}, {self.y}, {self.w}, {self.h}) -> {round(self.conf, 3)}"

def draw_detections(img, detections: List[Detection]):
    for d in detections:        
        cv2.rectangle(img, (d.x, d.y), (d.x + d.w, d.y + d.h), (0, 0, 255), 5)

def detect_objects(cascade: Cascade, gray_im) -> List[Detection]:   
    detections = []
    w = gray_im.shape[1]
    h = gray_im.shape[0]

    s = config.min_size
    while s <= min(w, h):
        step = max(math.floor(config.step_scale * s), 1)

        for r in range(0, h - s, step):
            for c in range(0, w - s, step):
                ww = math.floor(s * cascade.wh_ratio)
                sample = gray_im[r:r+s, c:c+ww]

                result, conf = predict(cascade, sample)
                if result:
                    d = Detection(c, r, int(s * cascade.wh_ratio), s, conf)
                    detections.append(d)

        s = math.floor(s * config.scale_increase)

    return detections