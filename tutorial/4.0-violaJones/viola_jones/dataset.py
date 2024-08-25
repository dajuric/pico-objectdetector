import cv2
import random
import os
from glob import glob
from . import config

class RoiRandomJitterTransformer():
    def __init__(self, wTranslatePerc = 0.07, hTranslatePerc = 0.07, whScalePerc = 0.07):
        self.wTranslatePerc = wTranslatePerc
        self.hTranslatePerc = hTranslatePerc
        self.whScalePerc = whScalePerc

    def __call__(self, roi):
        cx, cy, w, h = roi
        cx += int((random.random() * 2 - 1) * w * self.wTranslatePerc)
        cy += int((random.random() * 2 - 1) * h * self.hTranslatePerc)

        whRatio = w / h
        h += int((random.random() * 2 - 1) * h * self.whScalePerc)
        w = int(h * whRatio) #keep the width height ratio

        return [cx, cy, w, h]


class LabeledDataset():
    def __init__(self, img_folder, wh_ratio):
        self.img_folder = img_folder
        self.wh_ratio = wh_ratio
        self.roi_ims, self.rois = self._get_data(img_folder, wh_ratio)

    def _get_data(self, img_folder, wh_ratio):
        rois = []
        roi_ims = []
        
        img_files = []
        for img_ext in [".jpg", ".jpeg", ".bmp", ".png"]:
            img_files.extend(glob(f"{img_folder}/**/{img_ext}", recursive=True))
        
        for i, img_file in enumerate(img_files):
            base_name, _ = os.path.splitext(img_file)
            txt_file = base_name + ".txt"
            
            rois[i] = []
            if os.path.exists(txt_file):
                rois[i] = self._parse_labels(txt_file, wh_ratio)
                
            roi_ims.extend([img_file] * len(rois[i]))
            
        return roi_ims, rois

    def _parse_labels_file(self, txt_file, wh_ratio):
        rois = []
        with open(txt_file, "r") as f: rows = f.readlines()
        
        for row in rows:
            items = list(map(float, row.split(",")))
            if len(items) != 4:
                raise Exception(f"Invalid row in {txt_file}.")
            
            _, cx, cy, w, h = items
            w = int(wh_ratio * h)
            rois.append((cx, cy, w, h))
            
        return rois
            
    def _get_impatch(self, im, roi):
        imH, imW = im.shape[:2]
        cx, cy, w, h = roi
         
        x = int((cx - w/2) * imW)
        y = int((cy - h/2) * imH)
        w = int(w * imW)
        h = int(h * imH)
        
        if x < 0 or y < 0 or (x + w) >= imW or (y + h) >= imH:
            raise Exception("Patch coordinates out of image bounds.")
        
        return im[y:y+h, x:x+w]


class PositiveDataset(LabeledDataset):
    def __init__(self, img_folder, wh_ratio, roiT = None):
        super().__init__(img_folder, wh_ratio)
        self.roiT = roiT

    def __getitem__(self, idx):
        if idx >= len(self):
            raise StopIteration

        im = cv2.imread(self.roi_ims[idx])
        im = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)

        cx, cy, w, h = self.rois[idx]
        if self.roiT:
            cx, cy, w, h = self.roiT(self.rois[idx])

        im_patch = self._get_impatch(im, (cx, cy, w, h))
        return im_patch

    def __len__(self):
        return len(self.roi_ims)

    
class NegativeDataset(LabeledDataset):
    def __init__(self, img_folder, wh_ratio, minH = config.min_size):
        super().__init__(img_folder, wh_ratio)
        self.minH = minH

    def __getitem__(self, _):
        imIdx = random.randrange(0, len(self.roi_ims))
        im = cv2.imread(self.roi_ims[imIdx])
        im = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)   

        imROI = self.rois[imIdx]  
        randROI = None

        while True:
            randROI = self._get_random_ROI(im.shape, self.minH, self.wh_ratio)
            if self._calc_IOU(imROI, randROI) < 0.5:
                break
            
        im_patch = self._get_impatch(im, randROI)
        return im_patch     

    def __len__(self):
        return 2**31 - 1 #infinity

    def _get_random_ROI(self, im_shape, minH, wh_ratio):
        imH, imW = im_shape[:2]

        h = random.randrange(minH, imH)
        w = int(wh_ratio * h)

        xC = random.randrange(w//2, imW - w//2)
        yC = random.randrange(h//2, imH - h//2)

        return [xC / imW, yC / imH, w / imW, h / imH]

    def _calc_IOU(self, roiA, roiB):
        xCa, yCa, wa, ha = roiA
        xCb, yCb, wb, hb = roiB

        overW = max(0, min(xCa + wa//2, xCb + wb//2) - max(xCa - wa//2, xCb - wb//2))
        overH = max(0, min(yCa + ha//2, yCb + hb//2) - max(yCa - ha//2, yCb - hb//2))

        iou = overW * overH / (wa * ha + wb * hb - overW * overH)
        return iou