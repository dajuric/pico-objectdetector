window.URL = window.URL || window.webkitURL;

function Camera()
{
    let video = document.createElement('video'); video.setAttribute("playsinline", "");
    let hCanvas = document.createElement('canvas');

    function videoToCanvas(canvasCtx, video, flipH, flipV)
    {
        let canvas = canvasCtx.canvas;
 
        let scaleX = flipH ? -1 : 1;
        let scaleY = flipV ? -1 : 1;
        let startX = flipH ? -canvas.width : 0;
        let startY = flipV ? -canvas.height : 0;

        canvasCtx.save();
        canvasCtx.scale(scaleX, scaleY);

        canvasCtx.drawImage(video, 0, 0, video.videoWidth, video.videoHeight,
                            startX, startY, canvas.width, canvas.height);

        canvasCtx.restore();
    }

    function successCallback(videoStream)
    {     
        video.srcObject = videoStream;		
        video.onloadedmetadata = () => { video.play(); };
				
		return new Promise((a, r) => video.onplay = a);
    }


    this.flipHorizontal = false;
    this.flipVertical = false;

    this.frameWidth = function () { return video.videoWidth; };
    this.frameHeight = function () { return video.videoHeight; };

    this.startAsync = function ()
    {
		let constraints = {
			audio: false,
			video: { width: { ideal: 320 }, height: { ideal: 240 }, facingMode: "" }
		};

        if (navigator.mediaDevices == null)
            throw Error("Can not read media devices. Is your connection HTTPS, if not on localhost?");
		
        return navigator.mediaDevices
					    .getUserMedia(constraints)
					    .then(stream => successCallback(stream));
    };

    this.getImageData = function ()
    {		
        hCanvas.width = video.videoWidth;
        hCanvas.height = video.videoHeight;
				
		let cc = hCanvas.getContext('2d');	
        videoToCanvas(cc, video, this.flipHorizontal, this.flipVertical);
				
        let cImData = cc.getImageData(0, 0, video.videoWidth, video.videoHeight);
        return cImData;
    }

    this.drawToCanvas = function (canvas, resetSize = true)
    {
        if (canvas == null)
            return;

        if (resetSize)
        {
            canvas.width = video.videoWidth;
            canvas.height = video.videoHeight;
        }

        var cc = canvas.getContext('2d');		
		videoToCanvas(cc, video, this.flipHorizontal, this.flipVertical);
    }

    let obj = this;
}