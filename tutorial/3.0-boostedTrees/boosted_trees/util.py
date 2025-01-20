
def search_ROC(labels, outputs, minTPR):
    threshold = max(outputs); TPR = 0; FPR = -1
    while TPR < minTPR:
        TPR, FPR = calculate_ROC_point(labels, outputs, threshold)
        threshold -= 0.001

    return TPR, FPR, threshold

def calculate_ROC_point(targets, outputs, threshold):
     TP = 0; FP = 0; TN = 0; FN = 0

     for idx in range(len(targets)):
        trueClass = targets[idx] > 0
        predictedClass = outputs[idx] >= threshold

        if trueClass:
            if predictedClass:
                TP += 1
            else:
                FN += 1
        else:
            if not predictedClass:
                TN += 1
            else:
                FP += 1

     TPR = TP / (TP + FN)
     FPR = FP / (FP + TN)
     return TPR, FPR