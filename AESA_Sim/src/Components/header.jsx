import { Component } from "react";
import style from "./header.module.scss";

export default class Header extends Component {
    render() {
        return (
            <div className={style.headerWrapper}>
                <p className={style.title}>
                    Actively Steered Electronic Phased Array Sonar
                </p>
            </div>
        );
    }
}