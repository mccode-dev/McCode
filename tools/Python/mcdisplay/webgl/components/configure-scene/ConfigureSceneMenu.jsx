import React from "react";
import GridButtons from "./grid-buttons/GridButtons";
import ViewButtons from "./view-buttons/ViewButtons";
import "./configure-scene-menu.css";
import ComponentStyler from "./component-styler/ComponentStyler";
import RaysMenu from "./rays-menu/RaysMenu";
import BackgroundColorButton from "./background-color-button/BackgroundColorButton";
import AxesButton from "./axes-button/AxesButton";
import ShapesButtons from "./shapes-buttons/ShapesButtons";

const ConfigureSceneMenu = () => {
  return (
    <div id="configure-scene-menu">
      <RaysMenu />
      <ComponentStyler />
      <ShapesButtons />
      <ViewButtons />
      <GridButtons />
      <AxesButton />
      <BackgroundColorButton />
    </div>
  );
};

export default ConfigureSceneMenu;
