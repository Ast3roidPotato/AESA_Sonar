import * as React from 'react';
import { styled } from '@mui/material/styles';
import Slider from '@mui/material/Slider';
import MuiInput from '@mui/material/Input';
import style from './slider.module.scss';

const Input = styled(MuiInput)`
width: 42px;
color:white;
`;





export default function SliderWrapped(props) {
    const [value, _setValue] = React.useState(props.default);

    const setValue = (newValue) => {
        _setValue(newValue);
        props.setVal(newValue);
    }

    const handleSliderChange = (event, newValue) => {
        setValue(newValue);
    };

    const handleInputChange = (event) => {
        setValue(event.target.value === '' ? '' : Number(event.target.value));
    };

    const handleBlur = () => {
        if (value < props.min) {
            setValue(props.min);
        } else if (value > props.max) {
            setValue(props.max);
        }
    };

    return (
        <div className={style.sliderWrapper}>
            <Slider
                value={typeof value === 'number' ? value : 1}
                onChange={handleSliderChange}
                className={style.slider}
                aria-labelledby={props.label}
                defaultValue={props.default}
                step={props.step}
                marks
                min={props.min}
                max={props.max}
                valueLabelDisplay="auto"
            />
            <Input
                value={value}
                size="small"
                onChange={handleInputChange}
                className={style.sliderInput}
                onBlur={handleBlur}
                inputProps={{
                    step: props.step,
                    min: props.min,
                    max: props.max,
                    type: 'number',
                    'aria-labelledby': props.label,
                }}
            />
        </div>
    );
}