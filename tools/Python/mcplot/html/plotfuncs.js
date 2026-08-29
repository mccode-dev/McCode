function nzmin(lst) {
  let mval = lst[0];
  let val = null;
  for (var i=1;i<lst.length;i++)  {
    val = lst[i];
    if (val > 0 && val < mval) mval = val;
  }
  return mval;
}

// binary search with target in |R.
function binsearch(lst, target) {
  // lst is empty
  if (lst.length == 0) {
    return null;
  }
  // init
  let n = lst.length;
  let t = target;
  let l = 0;
  let r = n-1;
  // t is to the left of the interval
  if (t <= lst[l]) {
    return l;
  }
  // t is to the right of the interval
  else if (t >= lst[r]) {
    return r;
  }
  // input is trivial
  if (lst.length == 1) {
    return 0;
  }
  // t is inside the interval
  else {
    while (r-l > 1) {
      let m = Math.floor((l+r)/2);
      if (lst[m] > t) {
        r = m;
      }
      if (lst[m] <= t) {
        l = m;
      }
    }
    if (t-lst[l] <= lst[r]-t) {
      return l;
    }
    else {
      return r;
    }
  }
}


function _pltfc_fireEvents(lst, sCaller, ...args) {
  // Utility function to help listener interfaces.
  let f = null;
  for (i=0;i<lst.length;i++) {
    f = lst[i];
    try {
      f(...args);
    }
    catch(error) {
      console.log("fail calling " + sCaller + " listener: ", error);
    }
  }
}


class Plot1D {
  rgstrMouseClickPlot(f) { this._mouseClickPlotListeners.push(f); } // supports x,y args
  fireMouseClickPlot(...args) { _pltfc_fireEvents(this._mouseClickPlotListeners, "mouseClickPlot", ...args); }
  rgstrMouseRClickPlot(f) { this._mouseRClickPlotListeners.push(f); }
  fireMouseRClickPlot(...args) { _pltfc_fireEvents(this._mouseRClickPlotListeners, "mouseRClickPlot", ...args); }
  rgstrMouseCtrlClickPlot(f) { this._mouseCtrlClickPlotListeners.push(f); }
  fireMouseCtrlClickPlot(...args) { _pltfc_fireEvents(this._mouseCtrlClickPlotListeners, "mouseCtrlClickPlot", ...args); }

  constructor(params, svg_branch, logscale=false, wname = null) {
    this._mouseClickPlotListeners = [];
    this._mouseRClickPlotListeners = [];
    this._mouseCtrlClickPlotListeners = [];

    // plot size - fixed or expand to window size
    let p = params;
    let width = p['w'];
    let height = p['h'];
    if (p['autosize']) {
      width = document.documentElement.clientWidth;
      height = document.documentElement.clientHeight;
    }

    this.params_lst = [params];
    this.wname = wname; // optinal parameter is used with a "plotwindow" element for view clipping
    this.hdl = _draw_labels(width, height, p['xlabel'], p['ylabel'], p['title'], svg_branch);

    this.xmin = d3.min(p['x']);
    this.xmax = d3.max(p['x']);
    this.ymin = nzmin(p['y']);
    this.ymax = d3.max(p['y'])*1.05;

    this.x_lst = [p['x']];
    this.y_lst = [p['y']];
    this.yErrData_lst = _makeErrorBarsData([p['x']], [p['y']], [p['yerr']]);
    if (params.colour != null)
      this.colours = [params.colour];
    else
      this.colours = ["black"];

    this.pointGroups = null;
    this.last_xScale = null;
    this.last_yScale = null;

    this.logscale = logscale;
    this._draw_1d_axes(this.hdl.wplt, this.hdl.hplt, this.xmin, this.xmax, this.ymin, this.ymax, this.hdl.axisGroup);
  }
  toggleLogscale() {
    this.logscale = !this.logscale;
    this._draw_1d_axes(this.hdl.wplt, this.hdl.hplt, this.xmin, this.xmax, this.ymin, this.ymax, this.hdl.axisGroup);
    this.rePlotMany(this.params_lst);
  }
  rePlotMany(params_lst) {
    this.params_lst = params_lst;

    this.x_lst = params_lst.map(p => p['x']);
    this.y_lst = params_lst.map(p => p['y']);
    let yErr_lst = params_lst.map(p => p['yerr']);
    this.yErrData_lst = _makeErrorBarsData(this.x_lst, this.y_lst, yErr_lst);
    this.colours = params_lst.map(p=>p.colour);

    this._drawPoints(this.last_xScale, this.last_yScale);
  }
  plotOneMore(params) {
    this.params_lst.push(params);
    this.rePlotMany(this.params_lst);
    // TODO: do something about his so we don't have to replot everything every time...
  }
  _drawPoints(xScl, yScl, data_line_style=false) {
    this.pointGroups.selectAll("*").remove();

    // draw each data set
    for (var j=0;j<this.x_lst.length;j++) {
      let x = this.x_lst[j];

      // log plotting
      let y = null;
      let ymin = Math.min.apply(null, this.y_lst[j].filter((y0)=>{return y0>0}));
      if (this.logscale==true) {
        y = this.y_lst[j].map(function(y0) {
          if (y0<=0) return ymin/10;
          return y0;
        });
      }
      else {
        y = this.y_lst[j];
      }
      let yErrData = this.yErrData_lst[j];

      // create a custom point group for this set
      let pg = this.pointGroups.append("g");

      // plot
      let style_as_data = (this.params_lst[j]["style_as_data"] == true);
      if (style_as_data) {
        // data as point annotations / symbols
        pg.selectAll("path")
          .data(x)
          .enter()
          .append('path')
          .attr("d", d3.symbol()
            .size(20)
            .type(d3.symbolCircle))
          .attr("transform", function(d, i) { return 'translate('+xScl(x[i])+','+yScl(y[i])+')'; })
          .attr("fill", function(d, i) { return this.colours[j]; }.bind(this))
          .attr("style", "cursor: crosshair;");
      }
      else {
        // data as lines
        pg.append("path")
          .attr("d", d3.line()
            .x( function(d, i) { return xScl(x[i]); })
            .y( function(d, i) { return yScl(y[i]); })
            (x) // since we use the function-wide scoped x, y directly, this just causes index generation
          )
          .attr("stroke", function(d, i) { return this.colours[j]; }.bind(this))
          .attr("stroke-width", "1px")
          .attr("fill", "none");
      }

      // error bars
      pg.selectAll("line")
        .data(yErrData)
        .enter()
        .append("line")
        .attr("x1", function (d) { return xScl(d.p1[0]); })
        .attr("y1", function (d) { return yScl(d.p1[1]); })
        .attr("x2", function (d) { return xScl(d.p2[0]); })
        .attr("y2", function (d) { return yScl(d.p2[1]); })
        //.attr("stroke", "black")
        .attr("stroke", function(d, i) { return this.colours[j]; }.bind(this))
        .attr("stroke-width", "1px")
        .attr("fill", "none");
    }
  }
  _draw_1d_axes(w, h, xmin, xmax, ymin, ymax, axisGroup) {
    // clear the branch we are working on
    axisGroup.selectAll("*").remove();

    // axis span points
    let x0 = 0;
    let y0 = h;
    let x1 = w;
    let y1 = 0;

    // font size - normal/small mode
    var fontSizeStr = _get_fontsize_str(w, h);
    var tickSize = _get_ticksize_int(w, h);

    // zoom
    var zoom = d3.zoom()
      .on("zoom", () => {
      var new_xScale = d3.event.transform.rescaleX(xScale);
      var new_yScale = d3.event.transform.rescaleY(yScale);
      xAxisGroup.call(xAxis.scale(new_xScale));
      yAxisGroup.call(yAxis.scale(new_yScale));
      //_drawPonts_1D(pointGroup, new_xScale, new_yScale, x_lst, y_lst, yErrData_lst);
      this._drawPoints(new_xScale, new_yScale);
      this.last_xScale = new_xScale;
      this.last_yScale = new_yScale;
    });
    var view = axisGroup.append("rect")
      .attr("width", w)
      .attr("height", h)
      .attr("style", "cursor: crosshair; fill: none; pointer-events: all;")
      .call(zoom);

    // clip
    var clip = axisGroup.append("clipPath")
      .attr("id", "viewClip_"+this.wname)
      .append("rect")
      .attr("width", w)
      .attr("height", h);

    // axes
    var xScale = d3.scaleLinear()
      .domain([xmin, xmax])
      .range([x0, x1]);
    var xAxis = d3.axisBottom()
      .ticks(5)
      .tickFormat(d3.format(".2e"))
      .tickSize(tickSize)
      .scale(xScale);
    var xAxisGroup = axisGroup.append("g")
      .style("font-size", fontSizeStr)
      .attr("transform", "translate(0," + y0 + ")")
      .classed("noselect", true)
      .call(xAxis);

    if (this.logscale==true) {
      if (ymin <= 0) ymin=0.1;
      var yScale = d3.scaleLog()
        .domain([ymin, ymax])
        .range([y0, y1])
        .base(10);
    } else {
      var yScale = d3.scaleLinear()
        .domain([ymin, ymax])
        .range([y0, y1]);
    }
    var yAxis = d3.axisLeft()
      .ticks(5)
      .tickFormat(d3.format(".2e"))
      .tickSize(tickSize)
      .scale(yScale);
    var yAxisGroup = axisGroup.append("g")
      .style("font-size", fontSizeStr)
      .attr("transform", "translate(" + x0 + ", 0)")
      .classed("noselect", true)
      .call(yAxis);

    this.last_xScale = xScale;
    this.last_yScale = yScale;
    this.pointGroups = axisGroup.append("g")
      .attr("clip-path", "url(#viewClip_"+this.wname+")");

    // when clicked, print x axis value to console
    // WARNING: plt1_self may behave like a global variable, this should be rewritten,
    //    but unfortunately, we need the >caller's this< for d3.mouse(this) to work,
    //    meaning that this particular design isn't great in js, or should be implemented
    //    differently
    let plt1_self = this;
    axisGroup
      .on("click", function(d) {
        let m = d3.mouse(this);
        let xpt = xScale.invert(m[0]);
        let ypt = yScale.invert(m[1]);

        // only report on the first data set
        let x = plt1_self.x_lst[0];
        let y = plt1_self.y_lst[0];
        let idx = binsearch(x, xpt);

        xpt = x[idx];
        ypt = y[idx];

        if (d3.event.ctrlKey)
          plt1_self.fireMouseCtrlClickPlot(xpt.toExponential(4), ypt.toExponential(4));
        else
          plt1_self.fireMouseClickPlot(xpt.toExponential(4), ypt.toExponential(4));
      })
      .on("contextmenu", function () {
        d3.event.preventDefault();
        plt1_self.fireMouseRClickPlot();
      });

    // draw on initial zoom
    this._drawPoints(xScale, yScale);
  }
}


class Plot2D {
  rgstrMouseClickPlot(f) { this._mouseClickPlotListeners.push(f); } // supports x,y args
  fireMouseClickPlot(...args) { _pltfc_fireEvents(this._mouseClickPlotListeners, "mouseClickPlot", ...args); }
  rgstrMouseRClickPlot(f) { this._mouseRClickPlotListeners.push(f); }
  fireMouseRClickPlot(...args) { _pltfc_fireEvents(this._mouseRClickPlotListeners, "mouseRClickPlot", ...args); }
  rgstrMouseCtrlClickPlot(f) { this._mouseCtrlClickPlotListeners.push(f); }
  fireMouseCtrlClickPlot(...args) { _pltfc_fireEvents(this._mouseCtrlClickPlotListeners, "mouseCtrlClickPlot", ...args); }

  constructor(params, svg_branch, log=false) {
    this._mouseClickPlotListeners = [];
    this._mouseRClickPlotListeners = [];
    this._mouseCtrlClickPlotListeners = [];

    this._plot_2d(params, svg_branch, log);
  }

  // plot 2d
  _plot_2d(params, svg_branch, log) {
    // plot size - fixed or expand to window size
    var p = params;
    let width = p['w'];
    let height = p['h'];
    if (p['autosize']) {
      width = document.documentElement.clientWidth;
      height = document.documentElement.clientHeight;
    }

    this.hdl = _draw_labels(width, height, p['xlabel'], p['ylabel'], p['title'], svg_branch);

    let data = p['img2dData'];
    let cb = p['imgColorbar'];
    let cbmin = p['cbMin'];
    let cbmax = p['cbMax'];
    if (log==true) {
      data = p['img2dDataLog'];
      cb = p['imgColorbarLog'];
      cbmin = p['cbMinLog'];
      cbmax = p['cbMaxLog'];
    }

    this._plot_2d_data(
      this.hdl.wplt,
      this.hdl.hplt,
      p['xmin'],
      p['xmax'],
      p['ymin'],
      p['ymax'],
      data,
      cb,
      cbmin,
      cbmax,
      this.hdl.axisGroup);

    // put some mouse events on the axisgroup
    this.hdl.axisGroup
      .on("click", function(d) {
        if (d3.event.ctrlKey)
          this.fireMouseCtrlClickPlot();
        else
          this.fireMouseClickPlot();
      }.bind(this))
      .on("contextmenu", function () {
        d3.event.preventDefault();
        this.fireMouseRClickPlot();
      }.bind(this));
  }
  _plot_2d_data(w, h, xmin, xmax, ymin, ymax, img2dData, imgColorbar, cbMin, cbMax, anchorElement) {
    // colorbar width
    var w_cb = 100; // this is the total width of space, image and ticks
    var w_cbimg = 15;
    var w_cbticks = 60; // widened from 45 to give the colorbar's tick numbers more room on the right edge

    if (_is_small_mode(w, h)) {
      var w_cb = 10; // this is the total width of space, image and ticks
      var w_cbimg = 3;
      var w_cbticks = 0;
    }

    // axes lengths
    var wi = w - w_cb;
    var hi = h;
    // orego
    var x0 = 0;
    var y0 = h;
    // end coords
    var x1 = x0 + wi;
    var y1 = y0 - hi;

    // font size
    var fontSizeStr = _get_fontsize_str(w, h);
    var tickSize = _get_ticksize_int(w, h);

    // zoom
    var zoom = d3.zoom()
      .on("zoom", zoomFunction);

    function zoomFunction() {
      // create new scale ojects based on event
      var new_xScale = d3.event.transform.rescaleX(xScale);
      var new_yScale = d3.event.transform.rescaleY(yScale);

      // update axes using the new scales
      xAxisGroup.call(xAxis.scale(new_xScale));
      yAxisGroup.call(yAxis.scale(new_yScale));

      // apply pan/zoom to the image
      img.attr("transform", d3.event.transform);
    };
    var view = anchorElement.append("rect")
      .attr("class", "zoom")
      .attr("width", w)
      .attr("height", h)
      .attr("style", "cursor: crosshair; fill: none; pointer-events: all;");

    // clip
    var clip = anchorElement.append("clipPath")
      .attr("id", "viewClip")
      .append("rect")
      .attr("width", wi)
      .attr("height", hi);

    // set up axes
    var xScale = d3.scaleLinear()
      .domain([xmin, xmax])
      .range([x0, x1]);
    var xAxis = d3.axisBottom()
      .ticks(5)
      .tickFormat(d3.format(".2e"))
      .tickSize(tickSize)
      .scale(xScale);
    var xAxisGroup = anchorElement.append("g")
      .style("font-size", fontSizeStr)
      .attr("transform", "translate(0," + y0 + ")")
      .classed("noselect", true)
      .call(xAxis);

    var yScale = d3.scaleLinear()
      .domain([ymin, ymax])
      .range([y0, y1]);
    var yAxis = d3.axisLeft()
      .ticks(5)
      .tickFormat(d3.format(".2e"))
      .tickSize(tickSize)
      .scale(yScale);
    var yAxisGroup = anchorElement.append("g")
      .style("font-size", fontSizeStr)
      .attr("transform", "translate(" + x0 + ", 0)")
      .classed("noselect", true)
      .call(yAxis);

    var pointGroup = anchorElement.append("g")
      .attr("clip-path", "url(#viewClip)")
      .call(zoom);

    var img = pointGroup.append("svg:image")
      .attr('width', wi)
      .attr('height', hi)
      .attr('preserveAspectRatio', 'none')
      .attr("xlink:href","data:image/jpg;base64," + img2dData)
      .attr("style",
        "image-rendering: optimizeSpeed;" +      // legacy SVG value (old Firefox/IE)
        "image-rendering: -moz-crisp-edges;" +   // older Firefox
        "image-rendering: crisp-edges;" +        // spec value
        "image-rendering: pixelated;");          // Chrome/Safari/modern Firefox (nearest-neighbor)

    var cbScale = d3.scaleLinear()
      .domain([cbMin, cbMax])
      .range([y0, y1]);
    var cbyAxis = d3.axisRight()
      .ticks(5)
      .tickFormat(d3.format(".2e"))
      .tickSize(tickSize)
      .scale(cbScale);
    var cbAxisGroup = anchorElement.append("g")
      .style("font-size", fontSizeStr)
      .attr("transform", "translate(" + (x1 + w_cb - w_cbimg - w_cbticks) + ", 0)")
      .classed("noselect", true)
      .call(cbyAxis);
    var cb = cbAxisGroup.append("svg:image")
      .attr('width', w_cbimg)
      .attr('height', hi)
      .attr("transform", "translate(" + (-w_cbimg) + ", 0)")
      .attr('preserveAspectRatio', 'none')
      .attr("xlink:href","data:image/jpg;base64," + imgColorbar);
  }
}

function _get_fontsize_str(w, h) {
  if (_is_small_mode(w, h))
    return "5px";
  else
    return "14px";
}
function _get_ticksize_int(w, h) {
  if (_is_small_mode(w, h))
    return 3;
  else
    return 6;
}
function _is_small_mode(w, h) {
  return (w<300 || h<300);
}

// private
function _draw_labels(w, h, xlabel, ylabel, title, svg_branch, plotfunc_inner) {
  if (!svg_branch) throw "svg branch required";

  // positioning
  var margin = 5;
  var wlab = w - 2*margin;
  var hlab = h - 2*margin;

  // font size
  var fontSizeStr = _get_fontsize_str(w, h);

  // axis labels
  var lblGroup = svg_branch
    .attr("width", w)
    .attr("height", h)
    .append("g")
    .attr("transform", "translate(" + margin +"," + margin + ")");

  // multi-line titles
  if (!title) title="";
  var titleLines = title.split(/\r?\n/)
  var titleGrp = lblGroup
    .append("text")
    .attr("dominant-baseline", "hanging")
    .attr("text-anchor", "middle")
    .style("font-size", fontSizeStr);

  if (titleLines.length > 1 && !_is_small_mode(w, h)) {
    for (i=0; i<titleLines.length; i++) {
      titleGrp
        .append("tspan")
        .attr("x", 0)
        .attr("dy", "15px")
        .text(titleLines[i]);
    }
  } else {
      titleGrp
        .append("tspan")
	.attr("x", 0)
        .text(titleLines[0]);  
  }

  var xLabelGrp = lblGroup
    .append("text")
    .text(xlabel)
    .attr("dominant-baseline", "middle")
    .attr("text-anchor", "middle")
    .style("font-size", fontSizeStr)
    .classed("noselect", true);

  var yLabelGrp = lblGroup
    .append("text")
    .attr("x", ylabx)
    .attr("y", ylaby)
    .text(ylabel)
    .attr("text-anchor", "middle")
    .style("font-size", fontSizeStr)
    .classed("noselect", true);

  var dt = titleGrp.node().getBBox().height;
  var dl = yLabelGrp.node().getBBox().height; // height and width are confused here, due to the rotation
  var db = xLabelGrp.node().getBBox().height;

  var wplt = wlab - 4.75*dl;
  var hplt = hlab - (1.3*dt + 2.1*db);
  var xplt = 5.0*dl; // widened from 4.0*dl to give y-axis tick labels more clearance from the y-axis title
  var yplt = dt + 0.3*dl;

  var titx = wlab/2;
  var tity = 0;//dt/2;
  var xlabx = wplt/2 + xplt;
  var xlaby = hlab - 2;
  var ylabx = dl/2 + 2;
  var ylaby = yplt + hplt/2;

  // position labels based on what was drawn
  titleGrp.attr("transform", "translate(" + titx + "," + tity + ")");
  xLabelGrp.attr("transform", "translate(" + xlabx + "," + xlaby + ")");
  yLabelGrp.attr("transform", "rotate(-90," + ylabx + ',' + ylaby + ")  translate(" + ylabx + "," + ylaby + ")");

  var axisGroup = lblGroup
    .append("g")
    .attr("transform", "translate(" + xplt +"," + yplt + ")");

  return { wplt: wplt, hplt: hplt, axisGroup: axisGroup };
}

// generates error bar vertical line coordinates from x, y and yErr, NOTE: inner loop over yErr, allowing it to be empty
function _makeErrorBarsData(x_lst, y_lst, yErr_lst) {
  let dataErr_lst = [];
  for (var j=0;j<yErr_lst.length;j++) {
    var dataErr = [];
    let x = x_lst[j];
    let y = y_lst[j];
    let yErr = yErr_lst[j];

    let bin = x[1]-x[0];
    // ignore any "strange" error bars
    for (var i=0; i < yErr.length; i++) {
      if (yErr[i] > 0 && yErr[i] < Math.abs(y[i])) {
        dataErr.push({ "p1": [x[i], y[i]-yErr[i]], "p2" : [x[i], y[i]+yErr[i]] });
        dataErr.push({ "p1": [x[i]-bin/3, y[i]-yErr[i]], "p2" : [x[i]+bin/3, y[i]-yErr[i]] });
        dataErr.push({ "p1": [x[i]-bin/3, y[i]+yErr[i]], "p2" : [x[i]+bin/3, y[i]+yErr[i]] });
      }
    }
    dataErr_lst.push(dataErr);
  }
  return dataErr_lst;
}
