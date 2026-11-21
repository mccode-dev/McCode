import React, { useEffect, useState } from "react";
import "../../../common.css";
import { useSceneContext } from "../../../Contexts/SceneContext";
import { useInstrumentContext } from "../../../Contexts/InstrumentContext";
import { curvedOffset, showMonitorData } from "../../../Contexts/addMonitorData";

// --simdir monitor data on flat and on curved (cylinder, banana, sphere) monitors, shown only when given
const MonitorDataButtons = () => {
  const { sceneRef } = useSceneContext();
  const { instrument } = useInstrumentContext();
  const [show, setShow] = useState({ flat: true, curved: true });

  useEffect(() => {
    // first render: no scene yet, loadComponents applies the same defaults
    if (sceneRef.current) showMonitorData(sceneRef.current, show.flat, show.curved);
  }, [show]);

  const mons = Object.values(instrument.monitors || {});
  if (mons.length === 0) return null;
  const toggle = (key) => setShow((s) => ({ ...s, [key]: !s[key] }));
  return (
    <div className="row">
      <button onClick={() => toggle("flat")} className={show.flat ? "active" : ""}>
        Monitor data
      </button>
      {mons.some((m) => curvedOffset(m) !== null) ? (
        <button onClick={() => toggle("curved")} className={show.curved ? "active" : ""}
          title="monitor data on cylinder, banana and sphere monitors">
          curved
        </button>
      ) : null}
    </div>
  );
};

export default MonitorDataButtons;
