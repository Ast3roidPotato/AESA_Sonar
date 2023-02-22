
let Socket;
let angleElement;
let distanceElement;
let canvasElement;
let angleToDistanceArray = {};

const minAngle = -30;
const maxAngle = 30;
const angleStep = 2;
const medianSamples = 3;

function init() {
  angleElement = document.getElementById("angleSpan");
  distanceElement = document.getElementById("distanceSpan");
  canvasElement = document.getElementById("canvas");

  window.onresize = (event) => {
    // console.log("window resize");
    scaleCanvas();
    // fillCanvas();
  }


  console.log("initiating socket");
  Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  Socket.onmessage = function (event) {
    processCommand(event);
  };
}

function scaleCanvas() {
  canvasElement.style.width = '100%';
  canvasElement.style.height = '100%';
  canvasElement.width = canvasElement.offsetWidth;
  canvasElement.height = canvasElement.offsetHeight;
}

function drawPoint(degAngle, distance) {
  const angle = degAngle * (Math.PI / 180);
  const ctx = canvasElement.getContext('2d');
  const canvasWidth = canvasElement.width;
  const canvasHeight = canvasElement.height;
  const centerX = canvasWidth / 2;
  const centerY = canvasHeight;
  const radius = 5;
  const canvasRelDistance = (distance / 2000) * canvasHeight;
  const x = centerX + (canvasRelDistance * Math.cos(angle));
  const y = centerY - (canvasRelDistance * Math.sin(angle));
  ctx.beginPath();
  ctx.arc(x, y, radius, 0, 2 * Math.PI, false);
  ctx.fillStyle = 'white';
  ctx.fill();
  ctx.lineWidth = 1;
  ctx.strokeStyle = '#003300';
  ctx.stroke();
}

function clearCanvas() {
  const ctx = canvasElement.getContext('2d');
  ctx.clearRect(0, 0, canvasElement.width, canvasElement.height);
}

function mapMMtoCanvas(mm) {
  const canvasHeight = canvasElement.height;
  return (mm / 2000) * canvasHeight;
}

//draws rings at 100mm intervals from 100-2000mm
function drawRings() {
  const ctx = canvasElement.getContext('2d');
  const canvasWidth = canvasElement.width;
  const canvasHeight = canvasElement.height;
  const centerX = canvasWidth / 2;
  const centerY = canvasHeight;
  for (let i = 1; i <= 20; i++) {
    ctx.beginPath();
    ctx.arc(centerX, centerY, mapMMtoCanvas(i * 100), 0, 2 * Math.PI, false);

    ctx.lineWidth = 1;
    if (i % 5 == 0) {
      ctx.lineWidth = 3;
      ctx.strokeStyle = 'rgba(255, 255, 255, 0.8)';
    }
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.5)';
    ctx.stroke();
  }

}

function addPointToAngleToDistanceArray(degAngle, distance) {
  if (angleToDistanceArray[degAngle] == undefined) {
    angleToDistanceArray[degAngle] = [];
  }
  angleToDistanceArray[degAngle].push(distance);
  if (angleToDistanceArray[degAngle].length > medianSamples) {
    angleToDistanceArray[degAngle].shift();
  }
}

function medianOfArray(array) {
  const sortedArray = [...array].sort();
  const middleIndex = Math.floor(sortedArray.length / 2);
  return sortedArray[middleIndex];
}

function averageOfArray(array) {
  const sum = array.reduce((a, b) => a + b, 0);
  return sum / array.length;
}
//draw a line at angle that is one canvas height long
function drawLineAtAngle(angle) {
  const ctx = canvasElement.getContext('2d');
  const canvasWidth = canvasElement.width;
  const canvasHeight = canvasElement.height;
  const centerX = canvasWidth / 2;
  const centerY = canvasHeight;
  const angleInRadians = (angle + 90) * (Math.PI / 180);
  const x1 = centerX;
  const y1 = centerY;
  const x2 = centerX + (canvasHeight * Math.cos(angleInRadians));
  const y2 = centerY - (canvasHeight * Math.sin(angleInRadians));
  ctx.beginPath();
  ctx.moveTo(x1, y1);
  ctx.lineTo(x2, y2);
  ctx.lineWidth = 1;
  ctx.strokeStyle = 'rgba(255, 255, 255, 0.8)';
  ctx.stroke();
}

function displayMedianPoints(minAngle, maxAngle, angleStep) {
  clearCanvas();
  drawRings();
  drawLineAtAngle(minAngle);
  drawLineAtAngle(maxAngle);
  for (let i = minAngle; i <= maxAngle; i += angleStep) {
    if (angleToDistanceArray[i] != undefined) {
      const medianDistance = averageOfArray(angleToDistanceArray[i]);
      // const medianDistance = medianOfArray(angleToDistanceArray[i]);
      drawPoint(i + 90, medianDistance);
    }
  }
}


function processCommand(event) {
  const obj = JSON.parse(event.data);
  const type = obj.type;
  // console.log("received: " + type);
  if (type.localeCompare("angleRange") == 0) {
    const receivedArray = obj.value;
    // console.log(obj);
    const angle = receivedArray[0];
    const distance = receivedArray[1];
    angleElement.innerHTML = angle;
    distanceElement.innerHTML = distance;
    addPointToAngleToDistanceArray(angle, distance);
    displayMedianPoints(minAngle, maxAngle, angleStep);
  }
}

function fillCanvas() {
  const ctx = canvasElement.getContext('2d');
  ctx.fillStyle = 'rgb(200, 0, 0)';
  ctx.fillRect(0, 0, canvasElement.width, canvasElement.height);
}


window.onload = function (event) {
  console.log("window onload");
  init();
  scaleCanvas();
  drawRings();
}

