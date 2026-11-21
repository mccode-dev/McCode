import React, { useEffect, useState } from "react";
import "../../../common.css";
import { useSceneContext } from "../../../Contexts/SceneContext";
import { useInstrumentContext } from "../../../Contexts/InstrumentContext";
import { curvedOffset, setMonitorLog, showMonitorData } from "../../../Contexts/addMonitorData";

// --simdir monitor data: 1D, 2D, on curved (cylinder, banana, sphere) monitors and log scale, shown only when given
const MonitorDataButtons = () => {
  const { sceneRef } = useSceneContext();
  const { instrument } = useInstrumentContext();
  const [show, setShow] = useState({ "1D": true, "2D": true, curved: true });
  const [log, setLog] = useState(false);

  useEffect(() => {
    // first render: no scene yet, loadComponents applies the same defaults
    if (sceneRef.current) showMonitorData(sceneRef.current, show);
  }, [show]);
  useEffect(() => {
    if (sceneRef.current) setMonitorLog(sceneRef.current, log);
  }, [log]);

  const mons = Object.values(instrument.monitors || {});
  if (mons.length === 0) return null;
  const toggle = (key) => setShow((s) => ({ ...s, [key]: !s[key] }));
  return (
    <div className="row">
      {["1D", "2D"].filter((d) => mons.some((m) => !!m.z === (d === "2D"))).map((d) => (
        <button key={d} onClick={() => toggle(d)} className={show[d] ? "active" : ""} title={`${d} monitor data`}>
          {d} data
        </button>
      ))}
      {mons.some((m) => curvedOffset(m) !== null) ? (
        <button onClick={() => toggle("curved")} className={show.curved ? "active" : ""}
          title="monitor data on cylinder, banana and sphere monitors">
          curved
        </button>
      ) : null}
      <button onClick={() => setLog(!log)} className={log ? "active" : ""} title="monitor data on a log scale">
        log
      </button>
    </div>
  );
};

export default MonitorDataButtons;
