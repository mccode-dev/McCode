import * as THREE from "three";
import { Lut } from "three/examples/jsm/math/Lut";

// angle offset (deg) of a curved monitor's x axis from atan2(x, z): longitude in Monitor_nD and
// PSD_monitor_4PI, PSDcyl_monitor adds 180 deg; null: not an angle around the monitor
export const curvedOffset = (mon) =>
  /^Longitude/.test(mon.xlabel) ? 0 : /^radial position/.test(mon.xlabel) ? -180 : null;

// --simdir monitor data on the monitor's drawing (component frame), as in mcplot (first row at ymin), as in
// webgl-classic: flat monitors: 2D as a colour map on the drawn xy rectangle, 1D as a curve;
// curved monitors (drawn with depth, angle along x): a colour map on a cylinder or sphere (latitude along y)
export function addMonitorData(group, mon, transparency) {
  group.updateMatrixWorld(true);
  const box = new THREE.Box3().setFromObject(group); // the drawing, before the component transform
  const w = box.max.x - box.min.x;
  let h = box.max.y - box.min.y;
  const off = curvedOffset(mon);
  const curved = box.max.z - box.min.z > 1e-9 && off !== null;
  if (!curved && !(w > 0 && h > 0)) return;
  const canvas = document.createElement("canvas");
  const ctx = canvas.getContext("2d");
  const z = mon.z || (curved ? [mon.y] : null); // curved 1D: a colour strip
  if (z) {
    const ny = z.length, nx = z[0].length;
    const max = Math.max(...z.map((r) => Math.max(...r)));
    const lut = new Lut("rainbow", 256);
    lut.setMin(0);
    lut.setMax(max > 0 ? max : 1);
    canvas.width = nx;
    canvas.height = ny;
    const img = ctx.createImageData(nx, ny);
    for (let iy = 0; iy < ny; iy++) {
      for (let ix = 0; ix < nx; ix++) {
        // Lut keeps the colour map values as given (linear): read them back as such, as in webgl-classic
        const c = lut.getColor(z[iy][ix]).getHex(THREE.LinearSRGBColorSpace), k = 4 * ((ny - 1 - iy) * nx + ix);
        img.data.set([(c >> 16) & 255, (c >> 8) & 255, c & 255, 255], k);
      }
    }
    ctx.putImageData(img, 0, 0);
  } else {
    const y = mon.y, n = y.length, ymax = Math.max(...y) || 1;
    canvas.width = 256;
    canvas.height = 256;
    ctx.fillStyle = "white";
    ctx.fillRect(0, 0, 256, 256);
    ctx.translate(256, 0);
    ctx.scale(-1, 1); // +x is on the left looking along the beam: mirror, so it reads from upstream
    ctx.strokeStyle = "blue";
    ctx.lineWidth = 3;
    ctx.beginPath();
    y.forEach((v, i) => ctx.lineTo(8 + (240 * i) / Math.max(1, n - 1), 248 - (220 * v) / ymax));
    ctx.stroke();
    ctx.fillStyle = "black";
    ctx.font = "16px sans-serif";
    ctx.fillText(mon.xlabel || "", 8, 20);
  }
  const tex = new THREE.CanvasTexture(canvas);
  tex.colorSpace = THREE.SRGBColorSpace;
  tex.magFilter = THREE.NearestFilter;
  let geometry;
  const pos = box.getCenter(new THREE.Vector3());
  if (curved) {
    // radius: farthest drawn point, from the centre (sphere) or the y axis (cylinder)
    let r = 0, rxz = 0;
    const v = new THREE.Vector3();
    group.traverse((o) => {
      const p = o.geometry && o.geometry.attributes.position;
      for (let i = 0; p && i < p.count; i++) {
        v.fromBufferAttribute(p, i).applyMatrix4(o.matrixWorld);
        r = Math.max(r, v.length());
        rxz = Math.max(rxz, Math.hypot(v.x, v.z));
      }
    });
    const D = Math.PI / 180, lim = mon.limits, a0 = lim[0] + off, a1 = lim[1] + off;
    if (mon.z && /^Latitude/.test(mon.ylabel)) {
      geometry = new THREE.SphereGeometry(r, 64, 32, (a0 + 90) * D, (a1 - a0) * D, (90 - lim[3]) * D, (lim[3] - lim[2]) * D);
      pos.set(0, 0, 0);
    } else {
      if (mon.z && /\[m\]/.test(mon.ylabel)) { h = lim[3] - lim[2]; pos.y = (lim[3] + lim[2]) / 2; } // e.g. PSDcyl_monitor draws no height
      geometry = new THREE.CylinderGeometry(rxz, rxz, h, 64, 1, true, a0 * D, (a1 - a0) * D);
      pos.set(0, pos.y, 0);
    }
  } else {
    geometry = new THREE.PlaneGeometry(w, h);
  }
  const surf = new THREE.Mesh(geometry, new THREE.MeshBasicMaterial({
    map: tex, transparent: transparency < 1, opacity: transparency, side: THREE.DoubleSide, depthWrite: transparency >= 1,
  }));
  surf.position.copy(pos);
  surf.name = mon.title;
  surf.userData.monitor = curved ? "curved" : "flat";
  group.add(surf);
}

// show or hide the monitor data on flat and curved monitors
// (remembered on the scene, so a reload by loadComponents keeps the choice)
export function showMonitorData(parentnode, flat, curved) {
  parentnode.userData.monitorData = { flat, curved };
  parentnode.traverse((o) => {
    if (o.userData.monitor) o.visible = o.userData.monitor === "curved" ? curved : flat;
  });
}
