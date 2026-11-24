import React, { useMemo, useRef, useState } from "react";
import * as THREE from "three";
import "../../../common.css";
import "./tour.css";
import { useSceneContext } from "../../../Contexts/SceneContext";
import { useInstrumentContext } from "../../../Contexts/InstrumentContext";
import { RAY } from "../../../Contexts/addRays";

const FLY_MS = 1000; // flight time between components at speed 1

// components the tour stops at: drawn ones (e.g. not Arm), skipping any within 1 mm of the previous
const tourComps = (scene) =>
  scene.children
    .filter((c) => c.type === "Group" && !c.name.includes(RAY))
    .filter((c) => !new THREE.Box3().setFromObject(c).isEmpty())
    .filter((c, i, a) => i === 0 || c.position.distanceTo(a[i - 1].position) >= 1e-3);

// stops P1, P2, ... from the campath file points: x y z, optionally the viewing direction dx dy dz,
// else towards the next point (the last keeps the previous direction); the stop's z axis is the viewing direction
const campathStops = (points) => {
  const Z = new THREE.Vector3(0, 0, 1);
  const pts = points.map((p) => new THREE.Vector3(p[0], p[1], p[2]));
  return points.map((p, i) => {
    const j = Math.min(i, pts.length - 2);
    const dir = p.length === 6 ? new THREE.Vector3(p[3], p[4], p[5]) : pts[j + 1].clone().sub(pts[j]);
    return { name: `P${i + 1}`, position: pts[i], quaternion: new THREE.Quaternion().setFromUnitVectors(Z, dir.normalize()) };
  });
};

// tour: fly the 3D camera from component to component (as in mcdisplay-webgl-classic), for movies
const Tour = () => {
  const { sceneRef, primaryCameraRef, primaryControlsRef, primaryViewRef, containerRef } = useSceneContext();
  const { instrument } = useInstrumentContext();
  const [open, setOpen] = useState(false);
  const [on, setOn] = useState(false);
  const [record, setRecord] = useState(false);
  const [comp, setComp] = useState("");
  const [sit, setSit] = useState("0");
  const [smooth, setSmooth] = useState(true);
  const [speed, setSpeed] = useState("0"); // log10 of the speed: 0.1x - 10x
  const [paused, setPaused] = useState(false);
  const [beam, setBeam] = useState(true);
  const [offset, setOffset] = useState(["0", "0", "0"]);
  const [view, setView] = useState(["0", "0", "0"]);
  const campath = instrument.campath; // from --campath, if given
  const path = useMemo(() => campath && campathStops(campath.points), [campath]);
  const [pathOff, setPathOff] = useState(false); // campath file unticked

  // the tour clock and the flight read the latest settings
  const s = useRef({});
  s.current = { on, paused, usePath: !!path && !pathOff, sit, smooth, speed: 10 ** Number(speed), beam, offset: offset.map(Number), view: view.map(Number) };
  const idx = useRef(-1);
  const fly = useRef(null); // the current flight and sit
  const last = useRef(0); // time of the last frame
  const raf = useRef(0); // the clock's next frame
  const recorder = useRef(null);
  const recFirst = useRef(null);

  // the stops: the campath file points, else the components
  const stops = () => (s.current.usePath ? path : tourComps(sceneRef.current));

  // fly to the next stop, then sit sitMs and call next
  const flyToNext = (sitMs, next) => {
    const comps = stops();
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
    fly.current = {
      posFrom: camera.position.clone(), posTo: pos,
      tgtFrom: controls.target.clone(), tgtTo: pos.clone().add(look), t: 0, ms: FLY_MS, sit: sitMs, next,
    };
    return c.name;
  };

  // the tour clock, every frame while the tour is on: runs at the speed setting, unless paused
  const tick = (now) => {
    if (!s.current.on) return;
    const dt = Math.min(100, now - last.current); // no jump after a hidden tab
    last.current = now;
    const f = fly.current;
    if (f && !s.current.paused) {
      const flying = f.t < f.ms;
      f.t += dt * s.current.speed;
      if (flying) { // after arrival, the camera is free during the sit
        const t = Math.min(1, f.t / f.ms);
        const e = s.current.smooth ? t : t * t * (3 - 2 * t); // smooth: steady, no stop at the component; else smoothstep
        primaryCameraRef.current.position.lerpVectors(f.posFrom, f.posTo, e);
        primaryControlsRef.current.target.lerpVectors(f.tgtFrom, f.tgtTo, e);
        primaryControlsRef.current.update();
      }
      if (f.t >= f.ms + f.sit) {
        fly.current = null;
        f.next();
      }
    }
    raf.current = requestAnimationFrame(tick);
  };

  // pause or go on, with the recording; on going on, the flight continues from where the camera is (it may have been moved)
  const togglePause = (p) => {
    s.current.paused = p;
    setPaused(p);
    const f = fly.current;
    if (!p && f && f.t < f.ms) {
      f.posFrom.copy(primaryCameraRef.current.position);
      f.tgtFrom.copy(primaryControlsRef.current.target);
      f.ms -= f.t;
      f.t = 0;
    }
    if (recorder.current) p ? recorder.current.pause() : recorder.current.resume();
  };

  const step = (start) => {
    fly.current = null;
    if (!s.current.on) return;
    if (start && record) startRecording("");
    const sit = s.current.smooth ? 0 : parseFloat(s.current.sit);
    // a campath file tour stops at its last point
    const name = flyToNext(1000 * (sit > 0 ? sit : 0), () => (s.current.usePath && idx.current === path.length - 1 ? endTour() : step(false)));
    setComp(name);
    if (recorder.current) {
      if (!recFirst.current) recFirst.current = name;
      else if (recFirst.current === name) { // one full loop recorded
        setRecord(false);
        stopRecording();
      }
    }
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
    if (checked) {
      step(true);
      last.current = performance.now();
      cancelAnimationFrame(raf.current);
      raf.current = requestAnimationFrame(tick);
    } else { fly.current = null; togglePause(false); setComp(""); stopRecording(); }
  };
  const endTour = () => { setRecord(false); toggleOn(false); };
  // campath file ticked/unticked: tour its points, or the components
  const togglePath = (checked) => {
    s.current.usePath = checked;
    setPathOff(!checked);
    idx.current = -1;
    setComp("");
    if (on) step(false);
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
    idx.current = stops().findIndex((c) => c.name === name) - 1;
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
          {path ? (
            <label title={`campath: ${campath.file}, ${path.length} points`}>
              <input type="checkbox" checked={!pathOff} onChange={(e) => togglePath(e.target.checked)} />campath file
            </label>
          ) : null}
          <span>sit<input type="number" min="0" step="0.5" value={sit} onChange={(e) => setSit(e.target.value)} />s</span>
          <label title="no stops: travel through the components at a steady pace (sit is ignored)">
            <input type="checkbox" checked={smooth} onChange={(e) => setSmooth(e.target.checked)} />Smooth
          </label>
          <span title="speed of the tour, also during a flight or sit">speed
            <input type="range" min="-1" max="1" step="0.05" value={speed} onChange={(e) => setSpeed(e.target.value)} />
            <span className="tour-speed">{(10 ** Number(speed)).toPrecision(2)}x</span>
          </span>
          <label><input type="checkbox" checked={beam} onChange={(e) => setBeam(e.target.checked)} />Move along beam</label>
          {vec("offset", "camera offset from the component origin (m), in the component frame", offset, setOffset, "m")}
          {vec("view", "added to the viewing direction 0 0 1, in the component frame (with Move along beam)", view, setView, "")}
          <select value={comp} onChange={(e) => pickComp(e.target.value)} title="start the tour from here">
            <option value=""></option>
            {sceneRef.current && stops().map((c) => <option key={c.name} value={c.name}>{c.name}</option>)}
          </select>
        </div>
      ) : null}
      {on ? (
        <button id="tour-pause" onClick={() => togglePause(!paused)} title="pause or continue the tour (the camera can be moved while paused)">
          <svg viewBox="0 0 24 24" width="24" height="24">
            <path d={paused ? "M7 4v16l13-8z" : "M6 4h4v16h-4zM14 4h4v16h-4z"} fill="currentColor" />
          </svg>
        </button>
      ) : null}
    </div>
  );
};

export default Tour;
