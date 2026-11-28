import { Ray, RayData } from "../model/Rays";
import * as THREE from "three";
import { Lut } from "three/examples/jsm/Addons.js";
import { Component } from "../model/Component";

// Constants
export const RAY = "ray";

// LUT Configuration
const LUT = new Lut("cooltowarm", 512);

// all rays are drawn by two objects, as drawing many objects is slow: the lines and the scatter points;
// ranges: per ray where it starts in them, so that a single ray is drawn by a draw range
type DrawnRays = {
  group: THREE.Group;
  lines: THREE.LineSegments;
  points: THREE.Mesh;
  ranges: { line: number; point: number }[];
};

const drawnRays = (parentnode): DrawnRays | undefined =>
  parentnode.userData.rays;

export function setRaysInvisible(parentnode) {
  const rays = drawnRays(parentnode);
  if (rays) rays.group.visible = false;
}

export function setRaysVisible(parentnode) {
  const rays = drawnRays(parentnode);
  if (!rays) return;
  rays.lines.geometry.setDrawRange(0, Infinity);
  rays.points.geometry.setDrawRange(0, Infinity);
  rays.group.visible = true;
}

// show ray index alone, or hide the rays
export function setRayVisibility(
  parentnode,
  index: number,
  visibility: boolean
) {
  const rays = drawnRays(parentnode);
  if (!rays || index >= rays.ranges.length - 1) return;
  const from = rays.ranges[index];
  const to = rays.ranges[index + 1];
  rays.lines.geometry.setDrawRange(from.line, to.line - from.line);
  rays.points.geometry.setDrawRange(from.point, to.point - from.point);
  rays.group.visible = visibility;
}

export function setScatterPointsInvisible(parentnode) {
  const rays = drawnRays(parentnode);
  if (rays) rays.points.visible = false;
}

export function setScatterPointsVisible(parentnode) {
  const rays = drawnRays(parentnode);
  if (rays) rays.points.visible = true;
}

export function addRays(parentNode, rayData: RayData, components: Component[]) {
  LUT.setMin(rayData.vmin);
  LUT.setMax(rayData.vmax);
  const matrices = new Map(
    components.map((c) => [c.name, new THREE.Matrix4().fromArray(c.m4).transpose()])
  );
  const sphere = new THREE.SphereGeometry(0.004, 4, 2).toNonIndexed().attributes.position;
  const linePos: number[] = [], lineColor: number[] = [];
  const pointPos: number[] = [], pointColor: number[] = [];
  const ranges = [{ line: 0, point: 0 }];
  rayData.rays.forEach((ray: Ray) => {
    const c = LUT.getColor(ray.speed);
    const vertices = ray.groups.flatMap((group) =>
      group.events.map(({ args }) => {
        const v = new THREE.Vector3(args[0], args[1], args[2]);
        const matrix = matrices.get(group.compname);
        return matrix ? v.applyMatrix4(matrix) : v;
      })
    );
    vertices.forEach((v, i) => {
      if (i > 0) {
        const u = vertices[i - 1];
        linePos.push(u.x, u.y, u.z, v.x, v.y, v.z);
        lineColor.push(c.r, c.g, c.b, c.r, c.g, c.b);
      }
      for (let j = 0; j < sphere.count; j++) {
        pointPos.push(v.x + sphere.getX(j), v.y + sphere.getY(j), v.z + sphere.getZ(j));
        pointColor.push(c.r, c.g, c.b);
      }
    });
    ranges.push({ line: linePos.length / 3, point: pointPos.length / 3 });
  });

  const group = new THREE.Group();
  group.name = RAY;
  group.visible = false;
  const lines = new THREE.LineSegments(
    createGeometry(linePos, lineColor),
    new THREE.LineBasicMaterial({ vertexColors: true })
  );
  const points = new THREE.Mesh(
    createGeometry(pointPos, pointColor),
    new THREE.MeshBasicMaterial({ vertexColors: true })
  );
  group.add(lines, points);
  const old = drawnRays(parentNode); // e.g. rays from another file dropped on the page
  if (old) parentNode.remove(old.group);
  parentNode.add(group);
  parentNode.userData.rays = { group, lines, points, ranges };
}

function createGeometry(positions: number[], colors: number[]) {
  const geometry = new THREE.BufferGeometry();
  geometry.setAttribute("position", new THREE.Float32BufferAttribute(positions, 3));
  geometry.setAttribute("color", new THREE.Float32BufferAttribute(colors, 3));
  return geometry;
}
