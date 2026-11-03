import * as THREE from "three";
import { RAY } from "./addRays";

export function clearScene(parentnode) {
  for (let i = parentnode.children.length - 1; i >= 0; i--) {
    let child = parentnode.children[i];
    if ((child.type === "Group" && !child.name.includes(RAY))|| child.type === "GridHelper" || child.type === "ArrowHelper") {
      parentnode.remove(child);
    }
  }
}

function createSurface(geometry, color, transparency) {
  const isTransparent = transparency < 1;
  const createMaterial = (side) => new THREE.MeshLambertMaterial({
    color: color,
    transparent: isTransparent,
    opacity: transparency,
    depthWrite: !isTransparent,
    side: side,
  });

  let surface;
  if (!isTransparent) {
    surface = new THREE.Mesh(geometry, createMaterial(THREE.DoubleSide));
  } else {
    // Render all translucent back faces before all translucent front faces.
    // This avoids grouping both passes inside each object when shapes overlap.
    surface = new THREE.Group();
    const back = new THREE.Mesh(geometry, createMaterial(THREE.BackSide));
    const front = new THREE.Mesh(geometry, createMaterial(THREE.FrontSide));
    back.renderOrder = 1;
    front.renderOrder = 2;
    surface.add(back, front);
  }

  // the shape's edges as lines, shown with or instead of the surface (showShapes)
  const edges = new THREE.LineSegments(
    new THREE.EdgesGeometry(geometry),
    new THREE.LineBasicMaterial({ color: color })
  );
  edges.userData.isEdges = true;
  surface.add(edges);
  return surface;
}

export function loadComponents(parentnode, components) {
  let compList = {};
  components.forEach((component, i) => {
    compList[i] = new THREE.Group();
    let transparency = component.transparency;
    for (let j = 0; j < component.drawcalls.length; j++) {
      let color = component.drawcalls[j].color || component.color; // pencolor, else component colour
      let args = [];
      if (component.drawcalls.length > 0) {
        args = component.drawcalls[j].args;
      }
      switch (component.drawcalls[j].key) {
        case "multiline":
        case "line":
          addMultiLine(args, compList[i], color);
          break;
        case "circle":
          addCircle(
            args[0],
            args[1],
            args[2],
            args[3],
            args[4],
            compList[i],
            color,
            transparency
          );
          break;
        case "sphere":
          addSphere(
            args[0],
            args[1],
            args[2],
            args[3],
            32,
            32,
            compList[i],
            color,
            transparency
          );
          break;
        case "cone":
          addCone(
            args[0],
            args[1],
            args[2],
            args[3],
            args[4],
            args[5],
            args[6],
            args[7],
            32,
            compList[i],
            color,
            transparency
          );
          break;
        case "cylinder":
          addCylinder(
            args[0],
            args[1],
            args[2],
            args[3],
            args[4],
            args[5],
            args[6],
            args[7],
            args[8],
            32,
            compList[i],
            color,
            transparency
          );
          break;
        case "disc":
          addDisc(
            args[0],
            args[1],
            args[2],
            args[3],
            args[4],
            args[5],
            args[6],
            32,
            compList[i],
            color,
            transparency
          );
          break;
        case "annulus":
          addAnnulus(
            args[0],
            args[1],
            args[2],
            args[3],
            args[4],
            args[5],
            args[6],
            args[7],
            32,
            compList[i],
            color,
            transparency
          );
          break;
        case "new_circle":
          addNewCircle(
            args[0],
            args[1],
            args[2],
            args[3],
            args[4],
            args[5],
            args[6],
            32,
            compList[i],
            color,
            transparency
          );
          break;
        case "box":
          addBox(
            args[0],
            args[1],
            args[2],
            args[3],
            args[4],
            args[5],
            args[6],
            args[7],
            args[8],
            args[9],
            compList[i],
            color,
            transparency
          );
          break;
        case "polygon":
          addPolygon(args[0], compList[i], color, transparency);
          break;
        case "polyhedron":
          addPolyhedron(args[0], compList[i], color, transparency);
          break;
        default:
          break;
      }
    }
  });
  for (let i = 0; i < components.length; i++) {
    let m4 = components[i].m4;
    let comp_matrix = new THREE.Matrix4();
    comp_matrix.set(
      m4[0],
      m4[1],
      m4[2],
      m4[3],
      m4[4],
      m4[5],
      m4[6],
      m4[7],
      m4[8],
      m4[9],
      m4[10],
      m4[11],
      m4[12],
      m4[13],
      m4[14],
      m4[15]
    );
    compList[i].applyMatrix4(comp_matrix);
    compList[i].name = components[i].name;
    parentnode.add(compList[i]);
  }
  const shapes = parentnode.userData.shapes ?? { surfaces: true, lines: false };
  showShapes(parentnode, shapes.surfaces, shapes.lines);
}

// show the --trace=2 shapes of all components as surfaces and/or lines
// (remembered on the scene, so a reload by loadComponents keeps the choice)
export function showShapes(parentnode, surfaces, lines) {
  parentnode.userData.shapes = { surfaces, lines };
  parentnode.children
    .filter((child) => child.type === "Group" && !child.name.includes(RAY))
    .forEach((group) =>
      group.traverse((obj) => {
        if (obj.isMesh) obj.material.visible = surfaces;
        if (obj.userData.isEdges) obj.visible = lines;
      })
    );
}

// add multiline
// 		arrVector3  -  an array of THREE.Vector3 instances
function addMultiLineV3(arrVector3, parent, linecolor) {
  var multilinematerial = new THREE.LineBasicMaterial({ color: linecolor });
  var multilinegeometry = new THREE.BufferGeometry().setFromPoints(arrVector3);
  var multiline = new THREE.Line(multilinegeometry, multilinematerial);
  parent.add(multiline);
}
// add multiline proxy method
//		points  -  array of single points
export function addMultiLine(points, parent, linecolor) {
  let vectors = [];
  for (var i = 0; i < points.length / 3; i++) {
    const v = new THREE.Vector3(
      points[i * 3],
      points[i * 3 + 1],
      points[i * 3 + 2]
    );
    vectors.push(v);
  }
  addMultiLineV3(vectors, parent, linecolor);
}

// add circle to xy, xz or yz plane
//
export function addCircle(
  plane,
  x,
  y,
  z,
  radius,
  parent,
  linecolor,
  transparency
) {
  if (radius == 0) {
    return;
  }

  if (plane === "xy") {
    addNewCircle(x, y, z, radius, 0, 0, 1, 32, parent, linecolor, transparency);
  }
  if (plane === "xz") {
    addNewCircle(x, y, z, radius, 0, 1, 0, 32, parent, linecolor, transparency);
  }
  if (plane === "yz") {
    addNewCircle(x, y, z, radius, 1, 0, 0, 32, parent, linecolor, transparency);
  }
}
// add sphere
//		x,y,z - center coordinates
//		radius
//		wseg 	- width segments
//		hseg 	- height segments
export function addSphere(
  x,
  y,
  z,
  radius,
  wseg,
  hseg,
  parent,
  color,
  transparency
) {
  if (radius === 0) {
    return;
  }
  var geometry = new THREE.SphereGeometry(radius, wseg, hseg);
  var sphere = createSurface(geometry, color, transparency);

  sphere.position.x = x;
  sphere.position.y = y;
  sphere.position.z = z;

  parent.add(sphere);
}

export function addCone(
  x,
  y,
  z,
  radius,
  height,
  nx,
  ny,
  nz,
  radSeg,
  parent,
  color,
  transparency
) {
  if (radius === 0) {
    return;
  }
  var geometry = new THREE.ConeGeometry(radius, height, radSeg);
  var cone = createSurface(geometry, color, transparency);

  cone.position.x = x;
  cone.position.y = y;
  cone.position.z = z;

  var original_axis = new THREE.Vector3(0, 1, 0);
  var align_axis = new THREE.Vector3(nx, ny, nz).normalize();
  cone.quaternion.setFromUnitVectors(original_axis, align_axis.clone());

  parent.add(cone);
}

export function addCylinder(
  x,
  y,
  z,
  radius,
  height,
  thickness,
  nx,
  ny,
  nz,
  radSeg,
  parent,
  color,
  transparency
) {
  const align_axis = new THREE.Vector3(nx, ny, nz).normalize();
  const add = (geometry) => {
    const cylinder = createSurface(geometry, color, transparency);
    cylinder.position.set(x, y, z);
    cylinder.quaternion.setFromUnitVectors(new THREE.Vector3(0, 1, 0), align_axis);
    parent.add(cylinder);
  };
  add(new THREE.CylinderGeometry(radius, radius, height, radSeg));
  // hollow: closed inner cavity, as in e.g. Incoherent
  if (thickness > 0 && radius > thickness && height > 2 * thickness) {
    const r = radius - thickness;
    add(new THREE.CylinderGeometry(r, r, height - 2 * thickness, radSeg));
  }
}

export function addAnnulus(
  x,
  y,
  z,
  outer_radius,
  inner_radius,
  nx,
  ny,
  nz,
  radSeg,
  parent,
  color,
  transparency
) {
  let geometry = new THREE.RingGeometry(
    outer_radius - inner_radius,
    outer_radius,
    radSeg
  );
  let original_axis = new THREE.Vector3(0, 0, 1);
  const annulus = createSurface(geometry, color, transparency);

  annulus.position.x = x;
  annulus.position.y = y;
  annulus.position.z = z;

  var align_axis = new THREE.Vector3(nx, ny, nz).normalize();
  annulus.quaternion.setFromUnitVectors(original_axis, align_axis.clone());

  parent.add(annulus);
}

export function addDisc(
  x,
  y,
  z,
  radius,
  nx,
  ny,
  nz,
  radSeg,
  parent,
  color,
  transparency
) {
  addAnnulus(
    x,
    y,
    z,
    radius,
    radius,
    nx,
    ny,
    nz,
    radSeg,
    parent,
    color,
    transparency
  );
}

export function addNewCircle(
  x,
  y,
  z,
  radius,
  nx,
  ny,
  nz,
  radSeg,
  parent,
  color,
  transparency
) {
  addAnnulus(
    x,
    y,
    z,
    radius,
    0.01,
    nx,
    ny,
    nz,
    radSeg,
    parent,
    color,
    transparency
  );
}

export function addBox(
  x,
  y,
  z,
  xwidth,
  yheight,
  zdepth,
  thickness,
  nx,
  ny,
  nz,
  parent,
  color,
  transparency
) {
  const align_axis = new THREE.Vector3(nx, ny, nz).normalize();
  const add = (geometry) => {
    const box = createSurface(geometry, color, transparency);
    box.position.set(x, y, z);
    box.quaternion.setFromUnitVectors(new THREE.Vector3(0, 1, 0), align_axis);
    parent.add(box);
  };
  add(new THREE.BoxGeometry(xwidth, yheight, zdepth));
  // hollow: closed inner cavity, as in e.g. Incoherent
  const t2 = 2 * thickness;
  if (thickness > 0 && xwidth > t2 && yheight > t2 && zdepth > t2) {
    add(new THREE.BoxGeometry(xwidth - t2, yheight - t2, zdepth - t2));
  }
}

export function addPolyhedron(faces_vertices, parent, color, transparency) {
  const parsed_faces_vertices = JSON.parse(faces_vertices);
  // fan-triangulate each face, whatever its number of vertices
  let faces = parsed_faces_vertices.faces.flatMap(({ face: f }) =>
    f.slice(1, -1).flatMap((v, j) => [f[0], v, f[j + 2]])
  );

  let vertices = new Float32Array(
    parsed_faces_vertices.vertices.flatMap((vertex) => vertex)
  );

  let geometry = new THREE.BufferGeometry();

  geometry.setIndex(faces);
  geometry.setAttribute("position", new THREE.BufferAttribute(vertices, 3));
  // non-indexed: flat per-face normals instead of averaging across edges
  geometry = geometry.toNonIndexed();
  geometry.computeVertexNormals();
  const polyhedron = createSurface(geometry, color, transparency);

  /*
    const edges = new THREE.EdgesGeometry(geometry);
    const line = new THREE.LineSegments(edges, new THREE.LineBasicMaterial({color: 0x000000}));
    polyhedron.add(line);
    */

  parent.add(polyhedron);
}

export function addPolygon(faces_vertices, parent, color, transparency) {
  const parsed_faces_vertices = JSON.parse(faces_vertices);
  let faces = parsed_faces_vertices.faces.flatMap((index) => index.face);

  let vertices = new Float32Array(
    parsed_faces_vertices.vertices.flatMap((vertex) => vertex)
  );

  let geometry = new THREE.BufferGeometry();

  geometry.setIndex(faces);
  geometry.setAttribute("position", new THREE.BufferAttribute(vertices, 3));
  geometry.computeVertexNormals();
  const polygon = createSurface(geometry, color, transparency);

  /*
    const edges = new THREE.EdgesGeometry(geometry);
    const line = new THREE.LineSegments(edges, new THREE.LineBasicMaterial({color: 0x000000}));
    polygon.add(line);
    */
  parent.add(polygon);
}
