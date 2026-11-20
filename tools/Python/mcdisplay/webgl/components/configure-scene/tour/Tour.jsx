import React, { useRef, useState } from "react";
import * as THREE from "three";
import "../../../common.css";
import "./tour.css";
import { useSceneContext } from "../../../Contexts/SceneContext";
import { useInstrumentContext } from "../../../Contexts/InstrumentContext";
import { RAY } from "../../../Contexts/addRays";

const FLY_MS = 1000; // flight time between components

// components the tour stops at: drawn ones (e.g. not Arm), skipping any within 1 mm of the previous
const tourComps = (scene) =>
  scene.children
    .filter((c) => c.type === "Group" && !c.name.includes(RAY))
    .filter((c) => !new THREE.Box3().setFromObject(c).isEmpty())
    .filter((c, i, a) => i === 0 || c.position.distanceTo(a[i - 1].position) >= 1e-3);

// tour: fly the 3D camera from component to component (as in mcdisplay-webgl-classic), for movies
const Tour = () => {
  const { sceneRef, primaryCameraRef, primaryControlsRef, primaryViewRef, containerRef } = useSceneContext();
  const { instrument } = useInstrumentContext();
  const [open, setOpen] = useState(false);
  const [on, setOn] = useState(false);
  const [record, setRecord] = useState(false);
  const [comp, setComp] = useState("");
  const [sit, setSit] = useState("0");
  const [beam, setBeam] = useState(true);
  const [offset, setOffset] = useState(["0", "0", "0"]);
  const [view, setView] = useState(["0", "0", "0"]);

  // the timer and the flight read the latest settings
  const s = useRef({});
  s.current = { on, sit, beam, offset: offset.map(Number), view: view.map(Number) };
  const idx = useRef(-1);
  const timer = useRef(null);
  const recorder = useRef(null);
  const recFirst = useRef(null);

  const flyToNext = () => {
    const comps = tourComps(sceneRef.current);
    const camera = primaryCameraRef.current;
    const controls = primaryControlsRef.current;
    if (comps.length === 0 || !camera) return "";
    idx.current = (idx.current + 1) % comps.length;
    const c = comps[idx.current];
    let look = controls.target.clone().sub(camera.position); // keeps the orbit distance
    if (s.current.beam) {
      // view is added to the viewing direction 0 0 1, in the component frame
      const dir = new THREE.Vector3().fromArray(s.current.view).add(new THREE.Vector3(0, 0, 1));
      if (dir.length() < 1e-9) dir.set(0, 0, 1); // camera and target must differ
      look = dir.normalize().applyQuaternion(c.quaternion).multiplyScalar(look.length());
    }
    const pos = new THREE.Vector3().fromArray(s.current.offset).applyQuaternion(c.quaternion).add(c.position);
    const fly = {
      posFrom: camera.position.clone(), posTo: pos,
      tgtFrom: controls.target.clone(), tgtTo: pos.clone().add(look), t0: performance.now(),
    };
    const animate = () => {
      const t = Math.min(1, (performance.now() - fly.t0) / FLY_MS);
      const e = t * t * (3 - 2 * t); // smoothstep
      camera.position.lerpVectors(fly.posFrom, fly.posTo, e);
      controls.target.lerpVectors(fly.tgtFrom, fly.tgtTo, e);
      controls.update();
      if (t < 1) requestAnimationFrame(animate);
    };
    animate();
    return c.name;
  };

  const step = (start) => {
    clearTimeout(timer.current);
    if (!s.current.on) return;
    if (start && record) startRecording("");
    const name = flyToNext();
    setComp(name);
    if (recorder.current) {
      if (!recFirst.current) recFirst.current = name;
      else if (recFirst.current === name) { // one full loop recorded
        setRecord(false);
        stopRecording();
      }
    }
    const sit = parseFloat(s.current.sit);
    timer.current = setTimeout(() => step(false), FLY_MS + 1000 * (sit > 0 ? sit : 0));
  };

  // record the 3D view during a tour, downloaded as <instr>_tour.mp4 (or .webm) when it stops
  const startRecording = (first) => {
    recFirst.current = first;
    const src = containerRef.current; // the canvas, shared by all views: copy the 3D view out of it
    const out = document.createElement("canvas");
    const ctx = out.getContext("2d");
    const copy = () => {
      if (!recorder.current) return;
      const cr = src.getBoundingClientRect();
      const vr = primaryViewRef.current.getBoundingClientRect();
      const kx = src.width / cr.width, ky = src.height / cr.height;
      const w = Math.round(vr.width * kx), h = Math.round(vr.height * ky);
      if (out.width !== w || out.height !== h) { out.width = w; out.height = h; }
      ctx.drawImage(src, (vr.left - cr.left) * kx, (vr.top - cr.top) * ky, w, h, 0, 0, w, h);
      requestAnimationFrame(copy);
    };
    const type = ["video/mp4", "video/webm"].find((t) => MediaRecorder.isTypeSupported(t));
    const chunks = [];
    recorder.current = new MediaRecorder(out.captureStream(30), { mimeType: type, videoBitsPerSecond: 8e6 });
    recorder.current.ondataavailable = (e) => chunks.push(e.data);
    recorder.current.onstop = () => {
      const a = document.createElement("a");
      a.href = URL.createObjectURL(new Blob(chunks, { type }));
      a.download = `${instrument.name}_tour.${type.slice(6)}`;
      a.click();
    };
    copy();
    recorder.current.start();
  };
  const stopRecording = () => {
    if (recorder.current) { recorder.current.stop(); recorder.current = null; }
  };

  const toggleOn = (checked) => {
    s.current.on = checked;
    setOn(checked);
    if (checked) step(true);
    else { clearTimeout(timer.current); setComp(""); stopRecording(); }
  };
  // Record ticked during a tour: start now, at the current component
  const toggleRecord = (checked) => {
    setRecord(checked);
    if (!checked) stopRecording();
    else if (on) startRecording(comp);
  };
  // component picked from the list: the tour starts there (now, if it is on)
  const pickComp = (name) => {
    if (!name) return;
    idx.current = tourComps(sceneRef.current).findIndex((c) => c.name === name) - 1;
    setComp(name);
    if (on) step(false);
  };

  const vec = (label, title, value, setValue, unit) => (
    <span title={title}>
      {label}
      {value.map((v, i) => (
        <input key={i} type="number" step="0.1" value={v}
          onChange={(e) => setValue(value.map((w, j) => (j === i ? e.target.value : w)))} />
      ))}
      {unit}
    </span>
  );

  return (
    <div>
      <button onClick={() => setOpen(!open)} className={on ? "active" : ""}>Tour</button>
      {open ? (
        <div id="tour-panel">
          <label><input type="checkbox" checked={on} onChange={(e) => toggleOn(e.target.checked)} />Tour</label>
          <label title="record one loop of the tour from where it is, then download the video">
            <input type="checkbox" checked={record} onChange={(e) => toggleRecord(e.target.checked)} />Record
          </label>
          <span>sit<input type="number" min="0" step="0.5" value={sit} onChange={(e) => setSit(e.target.value)} />s</span>
          <label><input type="checkbox" checked={beam} onChange={(e) => setBeam(e.target.checked)} />Move along beam</label>
          {vec("offset", "camera offset from the component origin (m), in the component frame", offset, setOffset, "m")}
          {vec("view", "added to the viewing direction 0 0 1, in the component frame (with Move along beam)", view, setView, "")}
          <select value={comp} onChange={(e) => pickComp(e.target.value)} title="start the tour from this component">
            <option value=""></option>
            {sceneRef.current && tourComps(sceneRef.current).map((c) => <option key={c.name} value={c.name}>{c.name}</option>)}
          </select>
        </div>
      ) : null}
    </div>
  );
};

export default Tour;
