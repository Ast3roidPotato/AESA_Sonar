import { Component } from "react";
import style from "./controls.module.scss";
import SliderWrapped from "./slider";
import * as React from 'react';

export default class Controls extends Component {
    constructor(props) {
        super(props);
        this.state = {
            degree: 0
        }
    }

    componentDidUpdate(prevProps) {
        if (
            prevProps.phaseShift !== this.props.phaseShift ||
            prevProps.spacing !== this.props.spacing
        ) {
            const newDeg = 57.29577951 * Math.asin(this.props.phaseShift / (this.props.spacing));
            this.setState({ degree: Math.round(100 * newDeg) / 100 });
        }
    }


    render() {
        return (
            <div className={style.controlsOuterWrapper}>
                <div className={style.allSliderWrapper}>
                    <div className={style.labelFont}>Number Of Elements</div>
                    <SliderWrapped
                        label="numElem"
                        min={1}
                        max={100}
                        step={1}
                        default={1}
                        setVal={this.props.setElem}
                    />
                    <div className={style.labelFont}>Spacing of elements in &#955;</div>
                    <SliderWrapped
                        label="spacing"
                        min={0}
                        max={3}
                        step={0.01}
                        default={0.5}
                        setVal={this.props.setSpacing}
                    />
                    <div className={style.labelFont}>Size of Simulation in &#955;</div>
                    <SliderWrapped
                        label="size"
                        min={1}
                        max={1000}
                        step={1}
                        default={20}
                        setVal={this.props.setSize}
                    />
                    <div className={style.labelFont}>Simulation Resolution (1 is full)</div>
                    <SliderWrapped
                        label="size"
                        min={0.05}
                        max={1}
                        step={0.05}
                        default={0.1}
                        setVal={this.props.setSimulationResolution}
                    />
                    <div className={style.labelFont}>Phase Shift in &#955;</div>
                    <SliderWrapped
                        label="size"
                        min={-this.props.spacing}
                        max={this.props.spacing}
                        step={0.01}
                        default={0}
                        setVal={this.props.setPhaseShift}
                    />
                    <div className={style.labelFont}>Degree Equivilent: {this.state.degree}</div>
                </div>
            </div>
        );
    }
}