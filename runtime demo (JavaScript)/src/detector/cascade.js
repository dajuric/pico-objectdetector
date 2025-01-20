"use asm";

function Cascade(binTests, leafs, thresholds, treeDepth, treeCount, whRatio)
{
    this.binTests = new Int16Array(binTests);
    this.leafs = new Float32Array(leafs);
    this.thresholds = new Float32Array(thresholds);

    this.treeDepth = treeDepth;
    this.treeCount = treeCount;
    this.whRatio = whRatio;
}

Cascade.parse = async function(cascadePath)
{
    let response = await fetch(cascadePath);
    if (!response.ok)
        throw Error("Can not fetch the cascade.");

    let data = await response.arrayBuffer();
    let dataView = new DataView(data);
    let offset = 0;
    
    let rowScale = dataView.getFloat32(offset, true); offset += 4;
    let colScale = dataView.getFloat32(offset, true); offset += 4;
    let treeDepth = dataView.getInt32(offset,  true); offset += 4;
    let treeCount = dataView.getInt32(offset,  true); offset += 4;

    let binTests = [];
    let leafs = [];
    let thresholds = [];

    for (let treeIdx = 0; treeIdx < treeCount; treeIdx++)
    {
        let nodeCount = Math.pow(2, treeDepth) - 1;
        for (let nodeIdx = 0; nodeIdx < nodeCount; nodeIdx++)
        {
            binTests.push(dataView.getInt8(offset, true)); offset += 1;
            binTests.push(dataView.getInt8(offset, true)); offset += 1;
            binTests.push(dataView.getInt8(offset, true)); offset += 1;
            binTests.push(dataView.getInt8(offset, true)); offset += 1;
        }
        binTests.push(0); binTests.push(0); binTests.push(0); binTests.push(0); //to align node and leaf arrays

        let leafCount = Math.pow(2, treeDepth);
        for (let leafIdx = 0; leafIdx < leafCount; leafIdx++)
        {
            leafs.push(dataView.getFloat32(offset, true)); offset += 4;
        }

        thresholds.push(dataView.getFloat32(offset, true)); offset += 4;
    }

    let cascade = new Cascade(binTests, leafs, thresholds, treeDepth, treeCount, colScale);
    return cascade;
};