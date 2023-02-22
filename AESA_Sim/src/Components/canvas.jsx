import { Component, createRef } from 'react';
import style from './canvas.module.scss';

const elementSize = 0.25;
let speedScaleFactor = 0.1;


export default class Canvas extends Component {
    constructor(props) {
        super(props);
        this.state = {
            dimensions: { width: 0, height: 0 },
            pagePosition: { x: 0, y: 0 },
            forceUpdate: 0,
        };
        this.sizeRef = createRef();
        this.canvasRef = createRef();
        this.TIMER_TIMEOUT = 100;
        this.spamPreventionTimer = null;
        this.bufferCanvas = new OffscreenCanvas(1, 1);
    }

    updateDimensions() {
        if (!this.sizeRef) {
            return;
        }

        if (!this.sizeRef.current || !this.sizeRef.current.getBoundingClientRect) {
            return;
        }

        const boundingRect = this.sizeRef.current.getBoundingClientRect();
        this.bufferCanvas.width = boundingRect.width * speedScaleFactor;
        this.bufferCanvas.height = boundingRect.height * speedScaleFactor;

        this.setState({
            dimensions: {
                width: boundingRect.width,
                height: boundingRect.height,
            },
            pagePosition: {
                x: boundingRect.x,
                y: boundingRect.y,
            },
        });
    }

    componentDidMount() {
        window.addEventListener("resize", (event) => {
            // console.log("resize fired");
            clearInterval(this.spamPreventionTimer);
            this.spamPreventionTimer = setTimeout(this.updateDimensions.bind(this), this.TIMER_TIMEOUT);
        });
        this.updateDimensions();
    }

    drawCircle(context, centerX, centerY, radius, color, lineWidth, lineColor) {
        context.lineWidth = lineWidth;
        context.strokeStyle = lineColor;
        context.fillStyle = color;
        context.beginPath();
        context.arc(centerX, centerY, radius, 0, 2 * Math.PI, false);
        context.fill();
    }

    lineAtAngle(x1, y1, length, angle, canvas) {
        canvas.beginPath();
        canvas.moveTo(x1, y1);
        let radians = angle * (Math.PI / 180);
        let x2 = x1 + Math.cos(radians) * length;
        let y2 = y1 + Math.sin(radians) * length;
        canvas.lineTo(x2, y2);
        canvas.stroke();
    }

    componentDidUpdate() {
        speedScaleFactor = this.props.simulationResolution;
        const contextRender = this.canvasRef.current.getContext('2d');
        const context = this.bufferCanvas.getContext('2d');
        //clear buffer canvas
        context.clearRect(0, 0, this.bufferCanvas.width, this.bufferCanvas.height);

        const numElem = this.props.numElem;
        const spacing = this.props.spacing;
        const size = this.props.size;

        const boundingRect = this.canvasRef.current.getBoundingClientRect();
        const width = Math.floor(boundingRect.width * speedScaleFactor);
        const height = Math.floor(boundingRect.height * speedScaleFactor);

        const widthFloat = boundingRect.width * speedScaleFactor;
        const heightFloat = boundingRect.height * speedScaleFactor;

        const actualScaleFactor = width / boundingRect.width;

        const centerX = width / 2;
        const lambdaPx = (heightFloat / size);

        const elementSpacingPx = spacing * lambdaPx;
        const elementSizePx = elementSize * lambdaPx;
        const elementRadiusPx = elementSizePx / 2;
        const leftSide = centerX - (numElem - 1) * elementSpacingPx / 2;
        const ypos = height - elementRadiusPx;

        const imgData = context.createImageData(width, height);
        const txPositions = [];
        for (let i = 0; i < numElem; i++) {
            const xpos = leftSide + i * elementSpacingPx;
            txPositions.push([xpos, ypos]);
        }
        const image = imgData.data;
        
        for (let i = 0; i < image.length; i += 4) {
            let totalValue = 0;
            const pxX = (i / 4) % width;
            const pxY = Math.floor((i / 4) / width);
            
            let lastDist = 0;

            for (let j = 0; j < txPositions.length; j++) {
                const txPos = txPositions[j];
                const txX = txPos[0];
                const txY = txPos[1];
                
                
                const dist = Math.sqrt((txX - pxX) ** 2 + (txY - pxY) ** 2);
                lastDist = dist;  
                let value = Math.sin(2 * Math.PI * dist / lambdaPx + (this.props.phaseShift * lambdaPx * j));
                const angleFromTxDeg = Math.abs(Math.atan2(txY - pxY, txX - pxX) * 180 / Math.PI - 90);
                let directivityScalar = 0.5 ** (angleFromTxDeg/45);
                value *= directivityScalar**2;
                totalValue += value;
            }
            
            totalValue /= txPositions.length;

            totalValue = Math.abs(totalValue);
            
            image[i] = 255;
            image[i + 1] = 255;
            image[i + 2] = 255;
            image[i + 3] = totalValue * 200;
            
            
            if (lastDist < elementRadiusPx) {
                image[i] = 255;
                image[i + 1] = 0;
                image[i + 2] = 0;
                image[i + 3] = 255;
            }
        }
        this.bufferCanvas.width = width;
        this.bufferCanvas.height = height;
        const offscreenContext = this.bufferCanvas.getContext('2d');
        offscreenContext.putImageData(imgData, 0, 0);
        
        
        for (let i = 0; i < txPositions.length; i++) {
            const txPos = txPositions[i];
            const txX = txPos[0];
            const txY = txPos[1];
            this.drawCircle(offscreenContext, txX, txY, elementRadiusPx, 'purple', 1, 'black');
        }
        
        const lineLength = height - elementRadiusPx;

        const lineColor = `rgba(25, 118, 210, ${0.6/speedScaleFactor}`
        const lineWidth = 2*speedScaleFactor;
        const lineStartX = centerX;
        const lineStartY = height - elementRadiusPx;
        offscreenContext.strokeStyle = lineColor;
        offscreenContext.lineWidth = lineWidth;
        for (let i = 0; i <= 12; i++) {
            const angle = 15 * i;
            this.lineAtAngle(lineStartX, lineStartY, lineLength, -angle, offscreenContext);
        }

        contextRender.clearRect(0, 0, boundingRect.width, boundingRect.height);
        contextRender.scale(1 / actualScaleFactor, 1 / actualScaleFactor);
        contextRender.drawImage(this.bufferCanvas, 0, 0);
        contextRender.scale(actualScaleFactor, actualScaleFactor);
        
    }
    
    render() {
        return (
            <div ref={this.sizeRef} className={style.canvasWrapper}>
                <canvas
                    id="simCanvas"
                    ref={this.canvasRef}
                    width={this.state.dimensions.width}
                    height={this.state.dimensions.height}
                ></canvas>
            </div>
        );
    }

}