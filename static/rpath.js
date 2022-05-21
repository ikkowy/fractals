export let x = Math.random() * 60 - 30;
export let y = Math.random() * 60 - 30;
export let z = Math.random() * 60 - 30;

const a = 15;
const b = 42;
const c = 2;

const canvas = document.getElementById("lorenz_screen");
const ctx = canvas.getContext("2d");

ctx.fillStyle = "transparent";
ctx.fillRect(0, 0, canvas.width, canvas.height);

function point(x, y) {
    //clear
    ctx.fillStyle = "transparent";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ctx.fillStyle = "red";
    ctx.beginPath();
    ctx.arc(x, y, 1, 0, 2 * Math.PI);
    ctx.fill();
}

function step() {
    let dt = 0.01;
    let dx = (a * (y - x)) * dt;
    let dy = (x * (b - z) - y) * dt;
    let dz = (x * y - c * z) * dt;
    x += dx;
    y += dy;
    z += dz;
}

export function next() {
    step();
    point(7 * x + 250, 7 * y + 250);
}