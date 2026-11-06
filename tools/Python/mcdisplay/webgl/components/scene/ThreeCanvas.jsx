import React, { useState, useEffect, useRef } from "react";
import { useGridContext } from "../../Contexts/GridContext";
import { useCameraContext } from "../../Contexts/CameraContext";
import "./three-canvas.css";
import {
  initializeScene,
  initializeCameras,
  initializeRenderer,
  addGrids,
  addAxes,
  initializeDirectionalLight,
  initializeAmbientLight,
} from "./initializeScene";
import { useInstrumentContext } from "../../Contexts/InstrumentContext";
import { clearScene, loadComponents } from "../../Contexts/addComponents";
import { useRaysContext } from "../../Contexts/RaysContext";
import {
  setRayVisibility,
  setScatterPointsVisible,
  setScatterPointsInvisible,
  addRays,
  setRaysVisible,
  setRaysInvisible,
} from "../../Contexts/addRays";
import { useAppContext } from "../../Contexts/AppContext";
import * as THREE from "three";
import { views } from "./views";
import TwoDView from "./two-d-view/TwoDView";
import InfoView from "./info-view/InfoView";
import { usePlotRangeContext } from "../../Contexts/PlotRangeContext";
import { useSceneContext } from "../../Contexts/SceneContext";

const ThreeCanvas = () => {
  const { showXY, showXZ, showYZ, gridSize, gridDivisions, showAxes } =
    useGridContext();
  const { camPos, setCamPosSide, setCamPosTop, setCamPosHome } =
    useCameraContext();
  const { instrument, setInstrument } = useInstrumentContext();
  const {
    showAllRays,
    toggleShowAllRays,
    play,
    setPlay,
    prevRayIndex,
    currentRayIndex,
    setCurrentRayIndex,
    showRays,
    rays,
    setRays,
    handleNextClick,
  } = useRaysContext();
  const { loading, setLoading } =
    useAppContext();

  const { plotlyRanges, updatePlotlyRanges } = usePlotRangeContext();
  const {
    containerRef,
    primaryCameraRef,
    primaryControlsRef,
    primaryViewRef,
    TopView2DRef,
    SideView2DRef,
    BackView2DRef,
    rendererRef,
    sceneRef,
    gridsRef,
    axesRef,
  } = useSceneContext();

  const [aspectRatio, setAspectRatio] = useState(1.0);
  const [hoverInfo, setHoverInfo] = useState("");
  const raycaster = new THREE.Raycaster();
  const pointer = new THREE.Vector2();
  let width, height;
  let mouseX = 0,
    mouseY = 0;

  const primaryView = views[0];
  const topView = views[1];
  const backView = views[2];
  const sideView = views[3];

  const playRef = useRef(play);

  let cameraZoom = {
    Top: 1,
    Side: 1,
    End: 1,
  };
  const [zoomTop, setZoomTop] = useState(1);
  const [zoomSide, setZoomSide] = useState(1);
  const [zoomEnd, setZoomEnd] = useState(1);

  function updateSize() {
    let correctWidth = window.innerWidth;
    let correctHeight = window.innerHeight;

    if (containerRef.current) {
      correctWidth = containerRef.current.clientWidth;
      correctHeight = containerRef.current.clientHeight;
    }

    if (width !== correctWidth || height !== correctHeight) {
      width = correctWidth;
      height = correctHeight;

      rendererRef.current.setSize(width, height);
    }
  }

  // Resize handling to keep renderer size in sync with the window
  useEffect(() => {
    const handleResize = () => {
      rendererRef.current.setSize(window.innerWidth, window.innerHeight * 2);
      render();
    };

    window.addEventListener("resize", handleResize);
    return () => {
      window.removeEventListener("resize", handleResize);
    };
  }, []);

  function onScrollZoom(
    event,
    viewName,
    camera,
    originalXRange,
    originalYRange,
    originalXCenter,
    originalYCenter,
    setZoom
  ) {
    console.log("scrolling");
    if (event.deltaY < 0) {
      camera.zoom *= 1.1; // zoom in
    } else {
      camera.zoom /= 1.1; // zoom out
    }
    cameraZoom[viewName] = camera.zoom;
    setZoom(camera.zoom);
    camera.updateProjectionMatrix();

    updatePlotlyZoom(
      viewName,
      originalXRange,
      originalYRange,
      originalXCenter,
      originalYCenter
    );
  }

  function updatePlotlyZoom(
    viewName,
    originalXRange,
    originalYRange,
    originalXCenter,
    originalYCenter
  ) {
    const xRange =
      (originalXRange[1] - originalXRange[0]) / cameraZoom[viewName];
    const yRange =
      (originalYRange[1] - originalYRange[0]) / cameraZoom[viewName];

    updatePlotlyRanges(viewName, {
      xaxis: [originalXCenter - xRange / 2, originalXCenter + xRange / 2],
      yaxis: [originalYCenter - yRange / 2, originalYCenter + yRange / 2],
    });
  }

  function render() {
    updateSize();
    /*
    raycaster.setFromCamera(pointer, cameraRef.current);
    const intersects = raycaster.intersectObjects(sceneRef.current.children);
    let currHoverInfo = "";
    for (let i = 0; i < intersects.length; i++) {
      if (
        intersects[i].object.visible === true &&
        !intersects[i].object.name.includes("grid")
      ) {
        currHoverInfo = intersects[i].object.name + " ";
        if(intersects[i].object.name==="" && intersects[i].object.parent.name !== undefined && intersects[i].object.parent.visible === true){          
          currHoverInfo += intersects[i].object.parent.name + " ";
        }
      }
    }
    if(currHoverInfo !== undefined) console.log(currHoverInfo);
    insertText(document.getElementById("hover-info"), currHoverInfo);
    */
    rendererRef.current.setScissorTest(true);

    if (containerRef.current) {
      views.forEach((view) => {
        const aspect = setScissorForElement(view.domElement);
        setAspectRatio(aspect);
        const camera = view.camera;
        view.updateCamera(camera, sceneRef.current, mouseX, mouseY);
        view.updateControls(view.controls);
        if (camera.type === "PerspectiveCamera") {
          camera.aspect = aspect;
          primaryCameraRef.current = camera;
          primaryControlsRef.current = view.controls;
        }
        camera.updateProjectionMatrix();
        rendererRef.current.render(sceneRef.current, camera);
      });
    }
  }

  function setScissorForElement(elem) {
    const canvasRect = containerRef.current.getBoundingClientRect();
    const elemRect = elem.getBoundingClientRect();

    // compute a canvas relative rectangle
    const right = Math.min(elemRect.right, canvasRect.right) - canvasRect.left;
    const left = Math.max(0, elemRect.left - canvasRect.left);
    const bottom =
      Math.min(elemRect.bottom, canvasRect.bottom) - canvasRect.top;
    const top = Math.max(0, elemRect.top - canvasRect.top);

    const width = Math.min(canvasRect.width, right - left);
    const height = Math.min(canvasRect.height, bottom - top);

    // setup the scissor to only render to that part of the canvas
    const positiveYUpBottom = canvasRect.height - bottom;
    rendererRef.current.setScissor(left, positiveYUpBottom, width, height);
    rendererRef.current.setViewport(left, positiveYUpBottom, width, height);

    // return the aspect
    return width / height;
  }

  useEffect(() => {
    const width = window.innerWidth * 2;
    const height = window.innerHeight * 2;

    const scene = initializeScene();
    sceneRef.current = scene;
    const renderer = initializeRenderer(width, height);
    rendererRef.current = renderer;
    initializeCameras(
      width,
      height,
      views,
      gridSize,
      BackView2DRef.current,
      TopView2DRef.current,
      SideView2DRef.current,
      primaryViewRef.current,
      updatePlotlyRanges
    );

    initializeDirectionalLight(scene);
    initializeAmbientLight(scene);

    function animate() {
      requestAnimationFrame(animate);
      render();
    }

    animate();
  }, []);

  useEffect(() => {
    if (primaryCameraRef.current) {
      primaryCameraRef.current.position.set(camPos.x, camPos.y, camPos.z);
      if (primaryControlsRef.current) {
        primaryControlsRef.current.target.set(0, 0, camPos.z);
        primaryControlsRef.current.update();
      }
    }
  }, [camPos]);

  useEffect(() => {
    clearScene(sceneRef.current);
    loadComponents(sceneRef.current, instrument.components);
    const gridsInitialized = gridsRef.current;
    if (instrument.components.length > 0) {
      const bbox = new THREE.Box3().setFromObject(sceneRef.current);
      const bboxSize = Math.ceil(bbox.min.distanceTo(bbox.max));
      setCamPosHome(
        new THREE.Vector3(bboxSize / 2, bboxSize / 2, bboxSize / 2)
      );
      const topPos = new THREE.Vector3(0, bboxSize, bboxSize / 2);
      const sidePos = new THREE.Vector3(bboxSize / 2, 0, bboxSize / 2);
      setCamPosSide(sidePos);
      setCamPosTop(topPos);

      //set top 2D view camera position for centering
      topView.camera.position.set(0, bboxSize, bboxSize / 2);
      topView.controls.target.set(0, 0, bboxSize / 2);

      //set side 2D view camera position for centering
      sideView.camera.position.set(-bboxSize / 2, 0, bboxSize / 2);
      sideView.controls.target.set(0, 0, bboxSize / 2);

      //set end 2D view camera position for centering
      backView.camera.position.set(0, 0, bboxSize);
      backView.controls.target.set(0, 0, bboxSize / 2);

      const grids = addGrids(
        sceneRef.current,
        bboxSize * 2,
        bbox.getCenter(new THREE.Vector3())
      );
      gridsRef.current = grids;

      const axes = addAxes(sceneRef.current);
      axesRef.current = axes;

      // Setup zoom sync for each orthographic camera
      views.forEach((view) => {
        if (view.camera.type === "OrthographicCamera") {
          console.log("bboxSize", bboxSize);
          const camera = view.camera;
          let originalXRange = [0, bboxSize];
          let originalYRange = [-bboxSize / 2, bboxSize / 2];

          if (view.view === "Top") {
            originalXRange = [0, bboxSize];
            originalYRange = [
              -bboxSize / 2 / aspectRatio,
              bboxSize / 2 / aspectRatio,
            ];
          } else if (view.view === "Side") {
            originalXRange = [0, bboxSize];
            originalYRange = [
              -bboxSize / 2 / aspectRatio,
              bboxSize / 2 / aspectRatio,
            ];
          } else if (view.view === "End") {
            originalXRange = [-bboxSize / 2, bboxSize / 2];
            originalYRange = [
              -bboxSize / 2 / aspectRatio,
              bboxSize / 2 / aspectRatio,
            ];
          }

          const originalXCenter = (originalXRange[0] + originalXRange[1]) / 2;
          const originalYCenter = (originalYRange[0] + originalYRange[1]) / 2;
          const setZoom =
            view.view === "Top"
              ? setZoomTop
              : view.view === "Side"
              ? setZoomSide
              : setZoomEnd;

          view.domElement.addEventListener(
            "wheel",
            (event) =>
              onScrollZoom(
                event,
                view.view,
                camera,
                originalXRange,
                originalYRange,
                originalXCenter,
                originalYCenter,
                setZoom
              ),
            false
          );
        }
      });
    }
    render();
  }, [instrument]);

  useEffect(() => {
    console.log("Rays updated");
    addRays(sceneRef.current, rays, instrument.components);
    render();
  }, [rays]);

  const handleShowAllRays = async () => {
    setLoading(true);
    if (!showRays || (showRays && !showAllRays)) {
      setRaysInvisible(sceneRef.current);
    } else if (showRays && showAllRays) {
      setPlay(false);
      setRaysVisible(sceneRef.current);
    }
    setLoading(false);
  };

  useEffect(() => {
    handleShowAllRays();
  }, [showAllRays]);

  const handleRayChange = async (index, prevIndex) => {
    setLoading(true);
    setRayVisibility(sceneRef.current, prevIndex, false);
    setRayVisibility(sceneRef.current, index, true);
    render();
    setLoading(false);
  };

  useEffect(() => {
    handleRayChange(currentRayIndex, prevRayIndex);
  }, [currentRayIndex]);

  const loop = () => {
    setTimeout(() => {
      handleRayChange(currentRayIndex, prevRayIndex);
      handleNextClick();
      if (playRef.current) {
        loop();
      }
    }, 1000);
  };

  useEffect(() => {
    playRef.current = play;
    if (play) loop();
  }, [play]);

  return (
    <div id="canvas-container">
      <canvas id="canvas" ref={containerRef}></canvas>
      <div id="views">
        <div className="view gray-color" id="primaryView" ref={primaryViewRef}>
          3D
        </div>
        <div className="column fill row-gap two-D">
          <div className="row fill">
            <TwoDView
              viewRef={TopView2DRef}
              text="Top"
              x_label={topView.x_label}
              y_label={topView.y_label}
              unit="m"
            />
            <TwoDView
              viewRef={SideView2DRef}
              text="Side"
              x_label={sideView.x_label}
              y_label={sideView.y_label}
              unit="m"
            />
          </div>
          <div className="row fill">
            <TwoDView
              viewRef={BackView2DRef}
              text="End"
              x_label={backView.x_label}
              y_label={backView.y_label}
              unit="m"
            />
            <div className="view">
              <InfoView />
            </div>
          </div>
        </div>
      </div>
    </div>
  );
};

export default ThreeCanvas;
