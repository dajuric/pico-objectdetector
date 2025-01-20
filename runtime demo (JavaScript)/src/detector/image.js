"use asm";

function Image(width, height, stride, data)
{
    this.width = width;
    this.height = height;
    this.stride = stride;
    this.data = data;
}

Image.fromImageData = function (imageData, im)
{
    if (im == null || im == undefined ||
	    (im.width != imageData.width || im.height != imageData.height) /*camera flip*/)
    {
        im = new Image(
                        imageData.width,
                        imageData.height,
                        imageData.width * 1,
                        new Uint8Array(imageData.width /*stride*/ * imageData.height)
                      );
    }

    const nPixels = im.stride * im.height;
    const src = imageData.data;
    let dst = im.data;

    let srcIdx = 0; let dstIdx = 0;
    while (dstIdx < nPixels)
    {
        dst[dstIdx] =
            (
               (src[srcIdx + 0] << 1) +                     //2 * red
               (src[srcIdx + 1] << 2) + src[srcIdx + 1] +   //5 * green
                src[srcIdx + 2]                             //1 * blue
            ) >> 3;                                         //divide by 8

        srcIdx += 4;
        dstIdx += 1;
    }

    return im;
}