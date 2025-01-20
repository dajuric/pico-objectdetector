function drawDetection(ctx, det, sH, sV)
{
    ctx.beginPath();
    ctx.lineWidth = 5;
    ctx.strokeStyle = "red";

    ctx.rect(
         det.x * sH,
         det.y * sV,
         det.w * sH,
         det.h * sV,
         0, 2 * Math.PI, false);

    ctx.stroke();
}

function drawDetections(canvas, detections, camWidth, camHeight)
{
    var ctx = canvas.getContext("2d");
    ctx.lineWidth = 5;
    ctx.strokeStyle = '#00CC00';

    for (var dIdx = 0; dIdx < detections.length; dIdx++)
    {
        var detection = detections[dIdx];
        drawDetection(ctx, detection, canvas.width / camWidth, canvas.height / camHeight);
    }
}