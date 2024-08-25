from viola_jones import *
from ttictoc import tic, toc
import cv2

if __name__ == "__main__":
    #train() #to train cascade.bin from 'dataset/' folder (NOTE: it is not used since it would be extremnly slow in Python for real purpose)
    
    c = read_cascade("hands.bin")
    im = cv2.imread("test.jpg")
    gray_im = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)

    tic()
    detections = detect_objects(c, gray_im)
    print("Elapsed ms: " + str(round(toc() * 1000)))

    draw_detections(im, detections)
    cv2.imshow("Image", im)

    cv2.waitKey()