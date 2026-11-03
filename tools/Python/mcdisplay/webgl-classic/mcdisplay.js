// transforms vertices by and returns the transformed
//      apoints  -  an array of vertices
//      transform  -  matrix4
var transformPoints = function(apoints, transform)
{
    var geometry = new THREE.Geometry();
    geometry.vertices = apoints;
    geometry.applyMatrix(transform);
    return geometry.vertices.slice();
}
// "main" class which is a collection of scene graph setup routives and data objects
//
var Main = function ()
{
    this.scene;
    this.camera;
    this.renderer;
    this.controls;

    this.compnodes = {};
    this.rootnode = new THREE.Object3D();
    this.bbnode = new THREE.Object3D();
    this.rootnode.add(this.bbnode);

    this.compColors = [0xf80000, 0x00f800, 0x0000f8, 0x00f8f8, 0xf800f8, 0x00f880, 0xf8f800, 0xf88000, 0x80f800, 0x0080f8, 0x800f8, 0xf80080, 0xa8a8a8];
    this.iColor = -1;

    this.raynodes = [];
    this.linenodes = []; // --trace=2 shapes as lines (all edges)
    this.surfnodes = []; // --trace=2 shapes as surfaces

    this.iRay = -1;

    this.lut = null;
}
// add circle to xy, xz or yz plane
//
Main.prototype.addCircle = function(plane, x, y, z, radius, parent, linecolor)
{
    if (radius == 0)
    {
        return;
    }

    var wrapper = new THREE.Object3D();
    var m = new THREE.Matrix4()
    if (plane == 'xy') { } // identity
    if (plane == 'xz') { m.makeRotationX( Math.PI/2 ); }
    if (plane == 'yz') { m.makeRotationY( Math.PI/2 ); }

    var segments = 48;
    var circleGeometry = new THREE.CircleGeometry( radius, segments );
    circleGeometry.vertices.shift(); // removes center vertex
    var material = new THREE.MeshBasicMaterial( {color: linecolor} );
    material.side = THREE.DoubleSide;

    var circle = new THREE.Line( circleGeometry, material ); // THREE.Mesh results in solid coloring

    circle.applyMatrix(m);

    circle.position.x = x;
    circle.position.y = y;
    circle.position.z = z;

    parent.add( circle );
}
// add a geometry as lines (its edges) and a semi-transparent surface (unless outline: lines only),
// centred at (x, y, z), turning the geometry's own axis (a THREE.Vector3) to n = [nx, ny, nz] (--trace=2 shapes)
Main.prototype.addShape = function(geometry, x, y, z, axis, n, parent, linecolor, outline)
{
    var node = new THREE.Object3D();
    var lines = new THREE.LineSegments(new THREE.EdgesGeometry(geometry), new THREE.LineBasicMaterial({color: linecolor}));
    node.add(lines);
    if (!outline) {
        var surf = new THREE.Mesh(geometry, new THREE.MeshLambertMaterial({color: linecolor, transparent: true, opacity: 0.4, side: THREE.DoubleSide, depthWrite: false}));
        node.add(surf);
        lines.visible = false; // default: surfaces only, lines with the Lines checkbox
        this.linenodes.push(lines);
        this.surfnodes.push(surf);
    }
    node.quaternion.setFromUnitVectors(axis, new THREE.Vector3(n[0], n[1], n[2]).normalize());
    node.position.set(x, y, z);
    parent.add(node);
}
// show --trace=2 shapes as surfaces and/or lines
Main.prototype.showShapes = function(surfaces, lines)
{
    for (var i = 0; i < this.surfnodes.length; i++) {
        this.surfnodes[i].visible = surfaces;
        this.linenodes[i].visible = lines;
    }
}
// opacity (0..1) of the --trace=2 shape surfaces
Main.prototype.setSurfaceOpacity = function(opacity)
{
    for (var i = 0; i < this.surfnodes.length; i++) { this.surfnodes[i].material.opacity = opacity; }
}
// add a polyhedron given as {"vertices": [[x,y,z], ...], "faces": [{"face": [i,j,k,...]}, ...]}
// (OFF files and polygons at --trace=2)
Main.prototype.addPolyhedron = function(json, parent, linecolor)
{
    var data = JSON.parse(json);
    var geometry = new THREE.Geometry();
    for (var i = 0; i < data.vertices.length; i++)
    {
        var v = data.vertices[i];
        geometry.vertices.push(new THREE.Vector3(v[0], v[1], v[2]));
    }
    for (var i = 0; i < data.faces.length; i++)
    {
        var f = data.faces[i].face; // n-gon: fan of triangles
        for (var j = 1; j < f.length - 1; j++) { geometry.faces.push(new THREE.Face3(f[0], f[j], f[j+1])); }
    }
    geometry.computeFaceNormals(); // flat shading
    this.addShape(geometry, 0, 0, 0, Y_AXIS, [0, 1, 0], parent, linecolor);
}
// add pointlight
// 		center 	- THREE.Vector3 instance
Main.prototype.addLight = function(center)
{
    // pointlight (required to light up sphereMaterial)
    var pointLight = new THREE.PointLight(0xFFFFFF);
    pointLight.position.x = center.x;
    pointLight.position.y = center.y;
    pointLight.position.z = center.z;
    this.scene.add(pointLight);
    this.scene.add(new THREE.AmbientLight(0x404040));
}
//  initialize the scene
//
Main.prototype.init = function(campos, invert, box)
{
    this.scene = new THREE.Scene();

    // far plane: keep long instruments (box = [x1, x2, y1, y2, z1, z2]) in view when zooming out
    var span = Math.max(box[1] - box[0], box[3] - box[2], box[5] - box[4]);
    this.camera = new THREE.PerspectiveCamera(60, window.innerWidth/window.innerHeight, 0.001, Math.max(130, 10*span));
    this.camera.position.x = campos.x; // -50;
    this.camera.position.y = campos.y; // 0;
    this.camera.position.z = campos.z; // 50;

    // NOTE: initial camera view direction is along the x axis

    this.renderer = new THREE.WebGLRenderer();
    this.renderer.setSize(window.innerWidth, window.innerHeight);
    if (invert) {
        this.renderer.setClearColor( 0xffffff );
    }

    document.getElementById("3dcanvas").appendChild(this.renderer.domElement);

    // on the canvas only: on the whole document it swallows mousedown, so range inputs can not be dragged
    this.controls = new THREE.OrbitControls(this.camera, this.renderer.domElement);
    this.controls.target.x = -campos.x/2; //1;
    this.controls.target.y = 0; //0;
    this.controls.target.z = campos.z; //49;

    this.camera.lookAt(this.controls.target);

    this.addLight(new THREE.Vector3(10, 50, 130))
    this.scene.add(this.rootnode);
}
//  cat camera view according to campos, a Vector3
//
Main.prototype.setCameraView = function(campos)
{
    this.camera.position.x = campos.x;
    this.camera.position.y = campos.y;
    this.camera.position.z = campos.z;

    this.controls.target.x = 0;
    this.controls.target.y = 0;
    this.controls.target.z = campos.z;

    this.camera.lookAt(this.controls.target);
}
// add multiline
// 		arrVector3  -  an array of THREE.Vector3 instances
Main.prototype.addMultiLineV3 = function(arrVector3, parent, linecolor)
{
    var multilinematerial = new THREE.LineBasicMaterial({color: linecolor});
    var multilinegeometry = new THREE.Geometry();
    multilinegeometry.vertices = arrVector3;
    var multiline = new THREE.Line(multilinegeometry, multilinematerial);
    parent.add(multiline);
}
// add multiline proxy method
//		points  -  array of single points
Main.prototype.addMultiLine = function(points, parent, linecolor)
{
    vectors = [];
    for (var i = 0; i < points.length / 3; i++)
    {
        points[i];
        v = new THREE.Vector3(points[i*3], points[i*3+1], points[i*3+2]);
        vectors.push(v);
    }
    this.addMultiLineV3(vectors, parent, linecolor);
}
//  adds tiny boxes to each particle ray scatter point
//
Main.prototype.putScatterPoints = function(raynode)
{
    var v;
    var vtx = raynode.vtx;
    for (var i = 0; i < vtx.length; i++)
    {
        var geometry = new THREE.BoxGeometry( 0.007, 0.007, 0.007 );
        v = vtx[i];
        geometry.translate(v.x, v.y, v.z);
        var material = new THREE.MeshBasicMaterial({ color: raynode.lut_color });
        var cube = new THREE.Mesh( geometry, material );

        raynode.annotcubes.add( cube );
    }
}

//  set color lookuptable min/max range
//  color maps: rainbow, cooltowarm, blackbody, grayscale
Main.prototype.setLutRange = function(min, max)
{
    this.lut = new THREE.Lut( "cooltowarm", 512 );
    this.lut.setMin(min);
    this.lut.setMax(max);
}
// add a ray node
//
Main.prototype.addRayNode = function(rayObj, vertices, speed)
{
    var lut_color = this.lut.getColor(speed);
    rayObj.lut_color = lut_color;

    var multilinematerial = new THREE.LineBasicMaterial({color: lut_color});
    var multilinegeometry = new THREE.Geometry();
    multilinegeometry.vertices = vertices;
    var multiline = new THREE.Line(multilinegeometry, multilinematerial);
    rayObj.add(multiline);

    rayObj.vtx = vertices; // WARNING: this is a hidden field hacky stuff, but only used in the function putScatterPoints
    annotcubes = new THREE.Object3D();
    rayObj.add(annotcubes);
    rayObj.annotcubes = annotcubes;

    this.raynodes.push(rayObj);
    rayObj.visible = false;
}
// add a component origo
//		compname  -  component name for the reference dict
Main.prototype.addComponent = function(compname)
{
    var comp = new THREE.Object3D();
    this.rootnode.add(comp);
    this.compnodes[compname] = comp;
    return comp;
}
// returns a cyclically new color intended for a component draw node
//
Main.prototype.getNextComponentColor = function()
{
    this.iColor += 1;
    if (this.iColor >= this.compColors.length)
    {
        this.iColor = 0;
    }
    return this.compColors[this.iColor];
}
// remove all rays in this.raynodes from the scene graph
//
Main.prototype.hideAllRays = function()
{
    for (var i = 0; i < this.raynodes.length; i++)
    {
        this.raynodes[i].visible = false;
    }
}
// hide all annotation cubes on all rays
//
Main.prototype.hideCubes = function()
{
    for (var i = 0; i < this.raynodes.length; i++)
    {
        ray = this.raynodes[i];
        ray.annotcubes.visible = false;
    }
}
// show all annotation cubes on all rays
//
Main.prototype.showCubes = function()
{
    for (var i = 0; i < this.raynodes.length; i++)
    {
        ray = this.raynodes[i];
        ray.annotcubes.visible = true;
    }
}
// iterates to attach the next ray in the global ray sequence
//
Main.prototype.hideRay = function(idx)
{
    if (this.raynodes.length == 0) { return idx; }

    if (idx >= this.raynodes.length | idx < 0)
    {
        throw "idx out of range"
    }
    this.raynodes[idx].visible = false;
}
// iterates to attach the next ray in the global ray sequence
//
Main.prototype.showRay = function(idx)
{
    if (this.raynodes.length == 0) { return idx; }

    if (idx >= this.raynodes.length | idx < 0)
    {
        throw "idx out of range"
    }
    //this.rootnode.add(this.raynodes[idx]);
    node = this.raynodes[idx];
    // NOTE: the following two lines should instead hvae been implemented via a function called "update" and set in a proper maner
    if (node.parent == null) { this.putScatterPoints(node); this.rootnode.add(node); };
    node.visible = true;
}

// loads json data into the scene graph
//      instrdata    -  json instrument definition data
//      particledata  -  json particle ray data
//      main         -  reference to scene wrapper
var TraceLoader = function(instrdata, particledata, main)
{
    this.main = main;
    this.instrdata = instrdata;
    this.particledata = particledata;
}
// synchronously load instrument component draw data
//
TraceLoader.prototype.loadInstr = function()
{
    var main = this.main;

    // utility function
    var drawFunc = function(call, parentnode, color)
    {
        key = call['key'];
        args = call['args'];

        if (key == 'multiline') {
            main.addMultiLine(args, parentnode, color);
        }
        if (key == 'line') {
            main.addMultiLine(args, parentnode, color);
        }
        if (key == 'circle') {
            main.addCircle(args[0], args[1], args[2], args[3], args[4], parentnode, color);
        }
        // --trace=2 shapes (disc, annulus, new_circle are also sent at --trace=1),
        // args as in mccodelib/instrgeom.py: centre x, y, z first, axis n last
        var a = args;
        if (key == 'new_circle' || key == 'disc') {
            main.addShape(new THREE.CircleGeometry(a[3], SEGMENTS), a[0], a[1], a[2], Z_AXIS, a.slice(4, 7), parentnode, color, key == 'new_circle');
        }
        if (key == 'annulus') { // outer radius, thickness
            main.addShape(new THREE.RingGeometry(Math.max(0, a[3] - a[4]), a[3], SEGMENTS), a[0], a[1], a[2], Z_AXIS, a.slice(5, 8), parentnode, color);
        }
        if (key == 'sphere') {
            main.addShape(new THREE.SphereGeometry(a[3], SEGMENTS, SEGMENTS/2), a[0], a[1], a[2], Y_AXIS, [0, 1, 0], parentnode, color);
        }
        if (key == 'cone') { // radius, height
            main.addShape(new THREE.CylinderGeometry(0, a[3], a[4], SEGMENTS), a[0], a[1], a[2], Y_AXIS, a.slice(5, 8), parentnode, color);
        }
        if (key == 'cylinder') { // radius, height, thickness: hollow = closed inner cavity, as in e.g. Incoherent
            var r = a[3], h = a[4], t = a[5], n = a.slice(6, 9);
            main.addShape(new THREE.CylinderGeometry(r, r, h, SEGMENTS), a[0], a[1], a[2], Y_AXIS, n, parentnode, color);
            if (t > 0 && r > t && h > 2*t) {
                main.addShape(new THREE.CylinderGeometry(r-t, r-t, h-2*t, SEGMENTS), a[0], a[1], a[2], Y_AXIS, n, parentnode, color);
            }
        }
        if (key == 'box') { // xwidth, yheight, zdepth, thickness: hollow as for cylinder
            var w = a[3], h = a[4], l = a[5], t = a[6], n = a.slice(7, 10);
            main.addShape(new THREE.BoxGeometry(w, h, l), a[0], a[1], a[2], Y_AXIS, n, parentnode, color);
            if (t > 0 && w > 2*t && h > 2*t && l > 2*t) {
                main.addShape(new THREE.BoxGeometry(w-2*t, h-2*t, l-2*t), a[0], a[1], a[2], Y_AXIS, n, parentnode, color);
            }
        }
        if (key == 'polyhedron') {
            main.addPolyhedron(args[0], parentnode, color);
        }
    }

    // INSTRUMENT
    var instr = this.instrdata;
    var instname = instr['name'];
    var abspath = instr['abspath'];

    // COMPONENTS
    var comps = instr['components'];
    var comp;
    var comp_node;
    var compname;
    var m4;
    var acolor;

    for (var i = 0; i < comps.length; i++)
    {
        comp = comps[i];

        compname = comp['name'];
        m4 = comp['m4'];
        comp_node = main.addComponent(compname);
        acolor = main.getNextComponentColor();

        drawcalls = comp['drawcalls']
        var call;
        for (var j = 0; j < drawcalls.length; j++) {
            call = drawcalls[j];

            drawFunc(call, comp_node, call['color'] || acolor);
        }

        comp_matrix = new THREE.Matrix4();
        comp_matrix.set(m4[0], m4[1], m4[2], m4[3], m4[4], m4[5], m4[6], m4[7], m4[8], m4[9], m4[10], m4[11], m4[12], m4[13], m4[14], m4[15]);
        comp_node.applyMatrix(comp_matrix);
    }

    // BOUNDING BOX
    var bb = instr['boundingbox'];
    var drawcalls = bb['drawcalls'];
    var call;
    for (var i = 0; i < drawcalls.length; i++)
    {
        call = drawcalls[i];
        key = call['key'];

        drawFunc(call, main.bbnode, 0x808080);
    }
}
// load particle rays
//
TraceLoader.prototype.loadParticles = function()
{
    var main = this.main;

    // set Lut stuff
    main.setLutRange(this.particledata['vmin'], this.particledata['vmax']);

    // RAYS
    var rays = this.particledata['rays'];
    var ray;
    var aVertices;
    for (var i = 0; i < rays.length; i++) {
        ray = rays[i];

        rayobj = new THREE.Object3D();
        aVertices = [];
        var aCompVertices;
        var compname;
        var speed = ray['speed'];

        var groups =  ray['groups'];
        var group;
        for (var j = 0; j < groups.length; j++) {
            group = groups[j];
            compname = group['compname'];
            aCompVertices = [];

            // PARTICLE STATES
            var events = group['events'];
            var localstate;
            var args;
            for (var k = 0; k < events.length; k++) {
                localstate = events[k];

                args = localstate['args'];
                aCompVertices.push(new THREE.Vector3(args[0], args[1], args[2]));
            }

            // transform these vertices by component matrix and add to vertex container for this ray
            if (main.compnodes[compname])
            {
                aVertices = aVertices.concat(transformPoints(aCompVertices, main.compnodes[compname].matrix));
            }
        }
        // add ray as a multiline
        main.addRayNode(rayobj, aVertices, speed);
    }
}
//  program controller
//      campos_x/y/z  -  determines initial camera position, this is used with --inspect
var Controller = function(campos_x, campos_y, campos_z, box_lst, invert_canvas)
{
    this.camPosInitial = new THREE.Vector3(campos_x, campos_y, campos_z);
    this.invert_canvas = invert_canvas;
    this.box_lst = box_lst; // this would be [x_min, x_max, ...]
    this.main = new Main();
    this.loader = new TraceLoader(MCDATA_instrdata, MCDATA_particledata, this.main);
    this.numRays = MCDATA_particledata["numrays"]
    this.viewmodel = new ViewModel(this.numRays);
}
Controller.prototype.setUpdateGuiFunc = function(updateGuiFunc)
{
    this.updateGuiFunc = updateGuiFunc;
}
//  main program execution loops are set up here
//
Controller.prototype.run = function()
{
    // init mcdisplay
    this.main.init(this.camPosInitial, this.invert_canvas, this.box_lst);

    // execution loops
    var _this = this;
    var renderLoop = function()
    {
    	requestAnimationFrame(renderLoop);
    	_this.main.renderer.render(_this.main.scene, _this.main.camera);
    }
    var busy = false;
    var dataLoop = function()
    {
        // add a single-threaded critical section / lock
        if (busy == true) { console.log("busy"); return; }
        busy = true;

        // show/hide ray nodes
        if (_this.viewmodel.playBack == PlayBack.RUN)
        {
            _this.incSingleRay();
        }
        if (_this.viewmodel.playBack == PlayBack.PAUSE)
        {
            // TODO: use viewmodel.updateVersion to skip this step if needed
            _this.showCurrentRay();
        }
        if (viewmodel.getDisplayMode() == DisplayMode.SINGLE)
        {
            _this.hidePrevRays();
        }

        // set bounding box visible property
        _this.main.bbnode.visible = viewmodel.getShowBoundingBox();

        // set scatter cubes on/off
        if (viewmodel.getShowScatterCubes())
        {
            _this.main.showCubes();
        }
        else
        {
            _this.main.hideCubes();
        }

        setTimeout(dataLoop, 1000/_this.viewmodel.raysPrSec);

        busy = false;
    }
    var updateGuiLoop = function()
    {
        setTimeout(updateGuiLoop, 100);
        _this.updateGuiFunc();
    }

    // initiate timed execution loops
    renderLoop();
    dataLoop();
    updateGuiLoop();

    // load data - possibly heavy
    this.loader.loadInstr();
    this.loader.loadParticles();

    // set pause playback mode now after everything else, in case of zero or one rays
    if (this.numRays <= 1) { this.viewmodel.playBack = PlayBack.PAUSE }
}
Controller.prototype.hidePrevRays = function()
{
    var arrLast = this.viewmodel.shiftNonFirstRayIdxs();
    for (var i = 0; i < arrLast.length; i++)
    {
        this.main.hideRay(arrLast[i]);
    }
}
Controller.prototype.incSingleRay = function()
{
    this.viewmodel.setRayIdx(this.viewmodel.getRayIdx() + 1)
    var retidx = this.main.showRay(this.viewmodel.getRayIdx());
}
Controller.prototype.showCurrentRay = function()
{
    this.main.showRay(this.viewmodel.getRayIdx());
}
Controller.prototype.setViewTop = function()
{
    var box = this.box_lst;
    var x = - 0.01; // must not be zero due to the euler angle rotation lock stuff
    var y = (box[5] - box[4])/2;
    var z = (box[5] - box[4])/2;
    this.main.camera.up = new THREE.Vector3(1, 0, 0);
    this.main.setCameraView(new THREE.Vector3(x, y, z));
    this.main.camera.up = new THREE.Vector3(0, 1, 0);
    // next line just to be safe
    this.main.camera.updateMatrix();
}
Controller.prototype.setViewSide = function()
{
    var box = this.box_lst;
    var x = - (box[5] - box[4])/2;
    var y = 0;
    var z = (box[5] - box[4])/2;
    this.main.setCameraView(new THREE.Vector3(x, y, z));
    // next line just to be safe
    this.main.camera.updateMatrix();
}
Controller.prototype.setViewHome = function()
{
    this.main.setCameraView(this.camPosInitial);
}

//  default axes of three.js shapes, and segments for round ones (as the C fallbacks)
//
var Y_AXIS = new THREE.Vector3(0, 1, 0);
var Z_AXIS = new THREE.Vector3(0, 0, 1);
var SEGMENTS = 24;

//  enum for playback state
//
PlayBack = { RUN : 0, PAUSE : 1 };

//  displaymode e.g. keep rays animated or display one at the time
//
DisplayMode = { SINGLE : 0, KEEP : 1 }

//  viewmodel for keeping control data free of the gui
//
var ViewModel = function(numRays)
{
    this.playBack = PlayBack.RUN;
    this.displayMode = DisplayMode.SINGLE;

    this.showBoundingbox = true;
    this.showScattCubes = false;

    this.numRays = numRays;
    this.rayIdx = [-1];
    this.raysPrSec = 5;

    this.updateVersion = 0; // incremented on update
}
ViewModel.prototype.getUpdateVersion = function()
{
    return this.updateVersion;
}
ViewModel.prototype.setShowBoundingBox = function(bbOnOff)
{
    if (bbOnOff)
    {
        this.showBoundingbox = true;
    }
    else
    {
        this.showBoundingbox = false;
    }
    this.updateVersion += 1;
}
ViewModel.prototype.getShowBoundingBox = function()
{
    return this.showBoundingbox;
}
ViewModel.prototype.setShowScatterCubes = function(scOnOff)
{
    if (scOnOff)
    {
        this.showScattCubes = true;
    }
    else
    {
        this.showScattCubes = false;
    }
    this.updateVersion += 1;
}
ViewModel.prototype.getShowScatterCubes = function()
{
    return this.showScattCubes;
}
ViewModel.prototype.setRayIdx = function(idx)
{
    if (this.numRays == 0) { return; }
    // the socalled "js modulus bug" for negative numbers, using ((n % m) + m) % m;
    idx = ((idx % this.numRays) + this.numRays) % this.numRays;
    this.rayIdx.push(idx);
    this.updateVersion += 1;
}
ViewModel.prototype.getRayIdx = function(idx)
{
    return this.rayIdx[this.rayIdx.length-1];
}
ViewModel.prototype.shiftNonFirstRayIdxs = function()
{
    if (this.rayIdx.length <= 1)
    {
        return [];
    }

    var arr = [];
    for (var i=0; i < this.rayIdx.length -1; i++)
    {
        arr.push(this.rayIdx.shift());
    }
    this.updateVersion += 1;
    return arr;
}
ViewModel.prototype.setPlayBack = function(playBack)
{
    this.playBack = playBack;
    this.updateVersion += 1;
}
ViewModel.prototype.getPlayBack = function()
{
    return this.playBack;
}
ViewModel.prototype.setDisplayMode = function(displayMode)
{
    this.displayMode = displayMode;
    this.updateVersion += 1;
}
ViewModel.prototype.getDisplayMode = function()
{
    return this.displayMode;
}
