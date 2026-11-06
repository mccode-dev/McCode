import React, { useEffect, useState } from "react";
import "../../../common.css";
import { useSceneContext } from "../../../Contexts/SceneContext";
import { showShapes } from "../../../Contexts/addComponents";

// --trace=2 shapes as surfaces and/or lines, never neither: switching one off switches the other on
const ShapesButtons = () => {
  const { sceneRef } = useSceneContext();
  const [show, setShow] = useState({ surfaces: true, lines: false });

  useEffect(() => {
    // first render: no scene yet, loadComponents applies the same defaults
    if (sceneRef.current) showShapes(sceneRef.current, show.surfaces, show.lines);
  }, [show]);

  const toggle = (key, other) =>
    setShow((s) => ({ [key]: !s[key], [other]: s[other] || s[key] }));

  return (
    <div className="row">
      <button
        onClick={() => toggle("surfaces", "lines")}
        className={show.surfaces ? "active" : ""}
      >
        Surfaces
      </button>
      <button
        onClick={() => toggle("lines", "surfaces")}
        className={show.lines ? "active" : ""}
      >
        Lines
      </button>
    </div>
  );
};

export default ShapesButtons;
