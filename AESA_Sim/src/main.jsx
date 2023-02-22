import { Component } from 'react';
import style from './main.module.scss';
import Header from './Components/header';
import Canvas from './Components/canvas';
import Controls from './Components/controls';


export default class Main extends Component {

    constructor(props) {
        super(props);
        this.state = {
            numElem: 1,
            spacing: 0.5,
            size: 10,
            phaseShift: 0,
            simulationResolution: 0.1,
        };
    }

    setElem = (numElem) => {
        this.setState({ numElem: numElem });
    }

    setSpacing = (spacing) => {
        this.setState({ spacing: spacing });
    }

    setSize = (size) => {
        this.setState({ size: size });
    }

    setPhaseShift = (phaseShift) => {
        this.setState({ phaseShift: phaseShift });
    }

    setSimulationResolution = (simulationResolution) => {
        this.setState({ simulationResolution: simulationResolution });
    }

    render() {
        return (
            <div className={style.outerWrapper}>
                <div className={style.pageWrapper}>
                    <Header />
                    <div className={style.contentOuterWrapper}>
                        <div className={style.contentInnerWrapper}>
                            <Canvas
                                numElem={this.state.numElem}
                                spacing={this.state.spacing}
                                size={this.state.size}
                                phaseShift={this.state.phaseShift}
                                simulationResolution={this.state.simulationResolution}
                            />
                            <Controls
                                numElem={this.state.numElem}
                                spacing={this.state.spacing}
                                size={this.state.size}
                                phaseShift={this.state.phaseShift}
                                simulationResolution={this.state.simulationResolution}
                                setElem={this.setElem}
                                setSpacing={this.setSpacing}
                                setSize={this.setSize}
                                setPhaseShift={this.setPhaseShift}
                                setSimulationResolution={this.setSimulationResolution}

                            />
                        </div>
                    </div>
                </div>
            </div>
        );
    }

}