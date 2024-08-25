from .base import *
from .dataset import *
from .serializer import *
from . import config
import os

def sample_positives(cascade, samples, count):
    positiveSamples = []
    confidences = []

    i = 0; nPicked = 0
    for sample in samples:
        result, conf = predict(cascade, sample)
        if result:
            positiveSamples.append(sample)
            confidences.append(conf)
            nPicked += 1

        i += 1
        if nPicked >= count:
            break

    hitRate = nPicked / i
    return positiveSamples, confidences, hitRate

def append_stage(cascade, positives, negatives, minTPR):
    tpSamples, tpConfs, tprHitRatio = sample_positives(cascade, positives, len(positives))
    print(f"Sampled positives: {len(tpSamples)}/{len(positives)}. Hit rate: {tprHitRatio}")

    nFPsTpPick = 2 * len(positives) - len(tpSamples)
    fpSamples, fpConfs, fprHitRatio = sample_positives(cascade, negatives, nFPsTpPick)
    print(f"Sampled negatives: {len(fpSamples)}/{len(negatives)}. Hit rate: {fprHitRatio}")

    if fprHitRatio <= config.targetFPR:
        return False

    labels = ([+1.0] * len(tpSamples)); labels.extend([-1.0] * len(fpSamples))
    labels = np.array(labels)

    samplesLst = tpSamples; samplesLst.extend(fpSamples)
    samples = np.empty(len(samplesLst), object); samples[:] = samplesLst[:] # retain nested 2D arrays

    confs = tpConfs; confs.extend(fpConfs)
    confs = np.array(confs)

    train_bct(cascade, samples, labels, confs, config.max_trees, minTPR, config.maxFPR)
    return True

def train(cascade_name = "cascade.bin", dataset = "dataset/"):
    print(f"""
Database folder must contain image files and their corresponding labels file (image name with '.txt' ext).
Labels file is in YOLOv3 format. Negative images may omit labels file (if there are not forbidden regions).
Cascade will be written as '{cascade_name}' or read if exists.
          """)
    
    if not os.path.exists(cascade_name):        
        cascade = Cascade(config.wh_ratio, config.tree_depth)
        write_cascade(cascade, cascade_name)

    cascade = read_cascade(cascade_name)
    trainPos = PositiveDataset(dataset, cascade.wh_ratio, RoiRandomJitterTransformer())
    trainNeg = NegativeDataset(dataset, cascade.wh_ratio)

    for stage_idx in range(cascade.stage_count(), len(config.minTPRs)):
        print(f"\n---- Stage {stage_idx + 1} ----\n")
        
        minTPR = config.minTPRs[stage_idx]
        is_appended = append_stage(cascade, trainPos, trainNeg, minTPR)
        if not is_appended:
            break
            
        write_cascade(cascade, cascade_name)       