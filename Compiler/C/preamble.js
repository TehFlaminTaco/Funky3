var cDrawHook;

Module.print = function(s){
    postMessage(s);
}

function render(){
    if(!runtimeInitialized)return requestAnimationFrame(render); // Sleep a bit longer
    if(cDrawHook())requestAnimationFrame(render);
}

addEventListener("message", e=>{
    Module.canvas = e.data.canvas;
    Module.ctx = e.data.canvas.getContext("2d");
    cDrawHook = cwrap("DrawHook", "number", []);
    requestAnimationFrame(render);
}, true);