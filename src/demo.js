let label = document.getElementById("fps");
let preview = document.getElementById("preview");
let flipBtn = document.getElementById("flipButton");

let camera = null;
let imBuffer = null;
let cascade = null;


function calcFPS(action)
{
    const tic = performance.now();

    action();

    const toc = performance.now();
    const fps = Math.round(1000 / (toc - tic));

    return fps;
}

function scalePreview()
{
    const MARGIN = 0.3;

    let cRatio = preview.width / preview.height;
    let newHeight = Math.round(window.innerHeight * (1 - MARGIN)); 
    let newWidth  = newHeight * cRatio; 
    
    preview.style.width = newWidth;
    preview.style.height = newHeight;
}

function render()
{
    let detections = null;
    let elapsed = calcFPS(() =>
    {
        let imageData = camera.getImageData();
        imBuffer = Image.fromImageData(imageData, imBuffer); //get gray image (use buffer if possible to avoid alloc / free from GC)
        detections = detectObjects(cascade, imBuffer);
    });
    
    camera.drawToCanvas(preview, true);
    drawDetections(preview, detections, camera.frameWidth(), camera.frameHeight());
    scalePreview();
            
    label.innerHTML = "FPS: " + elapsed.toString().padStart(3, '_');
    requestAnimationFrame(render);
}

async function runDemo()
{
    camera = new Camera();
    cascade = await Cascade.parse("src/cascade.bin");
    
    try { await camera.startAsync(); }
    catch (e) { alert("Camera stream error: " + e.name + "-" + e.message); }

    camera.flipHorizontal = true;
    flipBtn.onclick = () => camera.flipHorizontal = !camera.flipHorizontal;

    window.addEventListener('resize', scalePreview, false);
    render();
}

window.onload = () => 
{
    let btn = document.querySelector("#runDemo");
    btn.onclick = () => 
    {
        let canvasContainer = document.querySelector("#canvasContainer");
        canvasContainer.classList.remove("disabled-div");

        let acceptContainer = document.querySelector("#accept");
        acceptContainer.style.display = "none";

        runDemo();
    };
}