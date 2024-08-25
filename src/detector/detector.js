"use asm";

function Detection(x, y, w, h, conf)
{
    this.x = x;
    this.y = y;
    this.w = w;
    this.h = h;

    this.conf = conf;
}


function evalFeature(binTests, binTestIdx, 
                     imgData, cx, cy, imW, imH, stride)
{
    const rA = (((imH / 2) * 256 + binTests[binTestIdx + 0] * imH) / 256) | 0;
    const cA = (((imW / 2) * 256 + binTests[binTestIdx + 1] * imW) / 256) | 0;

    const rB = (((imH / 2) * 256 + binTests[binTestIdx + 2] * imH) / 256) | 0;
    const cB = (((imW / 2) * 256 + binTests[binTestIdx + 3] * imW) / 256) | 0;

    const result = imgData[(cy + rA) * stride + (cx + cA)] <= 
                   imgData[(cy + rB) * stride + (cx + cB)];

    return result; 
}

function evalTree(cascade, treeOffset,
                  imgData, cx, cy, imW, imH, stride)
{
    let nodeIdx = 0;
    for (let depth = 0; depth < cascade.treeDepth; depth++)
    {
        const isTrue = evalFeature(cascade.binTests, (treeOffset + nodeIdx) * 4, 
                                   imgData, cx, cy, imW, imH, stride);
        if (isTrue)
            nodeIdx = nodeIdx * 2 + 2;
        else
            nodeIdx = nodeIdx * 2 + 1;
    }

    const leafIdx = nodeIdx - (Math.pow(2, cascade.treeDepth) - 1);
    const confidence = cascade.leafs[treeOffset + leafIdx];
    return confidence;
}

function evalPatch(cascade, 
                   imgData, x, y, imW, imH, stride)
{
    let confidence = 0.0;
    let treeOffset = 0 | 0;
    
    for (let treeIdx = 0; treeIdx < cascade.treeCount; treeIdx++)
    {
        const treeConf = evalTree(cascade, treeOffset, 
                                  imgData, x, y, imW, imH, stride);

        confidence += treeConf;
        treeOffset += Math.pow(2, cascade.treeDepth);

        if (confidence < cascade.thresholds[treeIdx])
            return -1;
    }

    return confidence;
}

function detectObjects(cascade, im, scaleFactor = 1.2, minHeight = 50)
{
    const maxSize = Math.min(im.width, im.height);

    const SHIFT_FACTOR = 0.1;
    const MAX_DETECTIONS = 256;

    let nDetections = 0;
    let scale = minHeight;
    let detections = [];

    while (scale <= maxSize)
    {
        const patchWidth  = (scale * cascade.whRatio) | 0;
        const patchHeight = scale | 0;

        const vStep = Math.max(SHIFT_FACTOR * patchHeight, 1) | 0;
        const maxRow = (im.height - patchHeight - 1);

        const hStep = Math.max(SHIFT_FACTOR * patchWidth, 1) | 0;
        const maxCol = (im.width - patchWidth - 1);
   
        for (let r = 0; r <= maxRow; r += vStep)
        {
            for (let c = 0; c <= maxCol; c += hStep)
            {
                if (nDetections > MAX_DETECTIONS)
                    return detections;

                const confidence = evalPatch(cascade, im.data, c, r, patchWidth, patchHeight, im.stride);
                if (confidence > 0)
                {
                    let detection = new Detection(c, r, patchWidth, patchHeight, confidence);
                    detections[nDetections] = detection;
                    nDetections++;
                }
            }
        }

        scale *= scaleFactor;
    }

    return detections;
}