function [comps, fig] = mcdisplay(varargin)
% mcdisplay: runs the McStas/McXtrace model in --trace mode and capture the output
%   grab all MCDISPLAY lines, and render the TRACE information into a figure.
%
%  mcdisplay instr
%     displays the given McCode model with its defaults/current parameters.
%  mcdisplay instr name1=value1 ...
%     displays the given McCode model with given parameters.
%  mcdisplay -png instr name1=value1 ...
%     same as above, and specifies an output file format to generate (here PNG)
%     Possible save_as are -png -pdf -fig -tif -jpg -eps
%  mcdisplay --inspect=COMP instr name1=value1 ...
%     same as above, and only plot component names that match 'inspect', given as
%                 a single component word for partial match, such as Monitor
%                 a component interval such as Monok:Sample or 2:10 or 2:end
%     
% Example:
%   mcdisplay Test_SX.instr TTH=13.4
%   mcdisplay Test_SX.instr TTH=13.4 --inspect=Diff
%   mcdisplay Test_SX.instr TTH=13.4 --inspect=2:end
%   
% output:
%   comps: a list of component specifications (structure array)
%   fig:   figure handle
%
% See also: http://www.mcstas.org, http://www.mcxtrace.org

  comps = []; fig = []; model = ''; match = '';
  
  % handle input arguments
  save_as = '';
  params  = '';
  instr   = '';
  options = '';
  for index= 1 : size(varargin,2)
    this_arg = varargin{index};
    switch this_arg
    case {'-png' '-pdf' '-fig' '-tif' '-jpg' '-eps'}
      save_as = [ save_as ' ' this_arg ];
      continue
    case {'-h','--help'}
          % display help and exit
          disp(' mcdisplay instr')
          disp('    displays the given McCode model with its defaults/current parameters.')
          disp(' mcdisplay instr name1=value1 ...')
          disp('    displays the given McCode model with given parameters.')
          disp(' mcdisplay [-png|-jpg|-fig|-eps|-pdf|-tif] instr name1=value1 ...')
          disp('    same as above, and specifies an output file format to generate')
          disp('    Possible save_as are -png -pdf -fig -tif -jpg -eps')
          disp(' mcdisplay --inspect=COMP instr name1=value1 ...')
          disp('    same as above, and only plot component names that match "inspect, given as')
          disp('                a single component word for partial match, such as Monitor')
          disp('                a component interval such as Monok:Sample or 2:10 or 2:end')
          exit
    end
    if strncmp(this_arg, '--inspect=', 10)  % get option for inspection 
      match=this_arg(11:end); 
      continue;
    elseif strcmp(this_arg, '--inspect')  % get option for inspection 
      match=varargin{index+1}; 
      index = index+1;
      continue;
    elseif this_arg(1) == '-'
      options = [ options ' ' this_arg ];
    elseif isempty(instr) & ~any(strcmp({ 'mxrun','mcrun'}, this_arg))
      instr = this_arg;
    end
    
    % else treat other arguments as args for the instrument executable
    model = [ model ' ' this_arg ];
  end
  
  % execute McCode command
  if ismac,      precmd = 'DYLD_LIBRARY_PATH= ; DISPLAY= ; ';
  elseif isunix, precmd = 'LD_LIBRARY_PATH= ; DISPLAY= ; ';
  else           precmd = ''; end
  % add --trace --no-output-files
  [status,output] = system([ precmd model ' --trace=1 --no-output-files' ]);
  
  disp([ mfilename ': plotting ' instr ]);
  if isempty(comps)
    index_start = strfind(output, 'MCDISPLAY: start');
    index_end   = strfind(output, 'MCDISPLAY: end');
    if numel(index_start) ~= 1 || numel(index_end) ~= 1
      warning([ mfilename ': The MCDISPLAY section is invalid (incomplete or multiple). Aborting.' ]);
      comps=output; fig=[];
      return
    end

    output_mcdisplay_init = output(1:index_start);
    output_mcdisplay_init = textscan(output_mcdisplay_init, '%s','Delimiter',sprintf('\n\r'));
    output_mcdisplay_init = output_mcdisplay_init{1};

    % initiate component structures
    % we build a struct array, with one entry per component, and fields:
    %   name: name
    %   pos: pos(3)
    %   rot: rot(3,3)
    %   x,y,z: set of points
    comps = mcdisplay_get_components(output_mcdisplay_init);
    clear output_mcdisplay_init
    
    % restrict output to the MCDISPLAY so that all searches are faster
    output_mcdisplay_section = output(index_start:index_end);
    clear output
    output_mcdisplay_section = textscan(output_mcdisplay_section, '%s','Delimiter','\n\r');
    output_mcdisplay_section = output_mcdisplay_section{1};

    % get the components in order, and identify the output section/lines
    % which are separated by e.g. MCDISPLAY: component <blah>
    index_mcdisplay_comp = find(~cellfun(@isempty, strfind(output_mcdisplay_section, 'MCDISPLAY: component ')));
    if numel(index_mcdisplay_comp) ~= numel(comps)
      warning([ mfilename ...
        ': WARNING: not the same number of declared components (' num2str(numel(comps)) ...
        ') and MCDISPLAY sections ' num2str(numel(index_mcdisplay_comp)) ])
    end

    % extract the multiline and circle stuff in each component mcdisplay section
    for index=1:numel(index_mcdisplay_comp)
      if index < numel(index_mcdisplay_comp), 
        next = index_mcdisplay_comp(index+1);
      else 
        next = numel(output_mcdisplay_section); end
      % get the MCDISPLAY section for a single component
      section = output_mcdisplay_section(index_mcdisplay_comp(index):next);
      % then we get the multiline and circle commands in this section
      for token = {'multiline' ,'mcdiscircle'}
        [x,y,z] = mcdisplay_get_token(section, token{1});
        comps(index).x = [ comps(index).x nan x ];
        comps(index).y = [ comps(index).y nan y ];
        comps(index).z = [ comps(index).z nan z ];
      end
    end
    clear output_mcdisplay_section
  end

  % PLOTTING: transform the points and plot them
  fig = gcf; 
  
  % handle match types: single name, range: i1:i2, range name1:name2
  if ~isempty(match)
    if ~isnan(str2double(match)) 
      new_match = ' ';
      for c = 1:numel(comps)
        if any(c == match)
          new_match = [ new_match comps(c).name ' ' ];
        end
      end
      match = new_match;
    elseif any(match == ':')
      [i1,i2] = strtok(match, ':'); % get 'from:to' items
      i2 = i2(2:end);
      if ~isnan(str2double(i1)) 
        i1 = str2double(i1);
        if i1 < 1, i1=1; elseif i1 > numel(comps), i1=numel(comps); end
        i1 = comps(i1).name;
      end
      if ischar(i2) && (strcmp(lower(i2), 'end') || strcmp(lower(i2), 'inf'))
        i2 = numel(comps)
      end
      if ~isnan(str2double(i2)) 
        i2 = str2double(i2);
        if i2 < 1, i2=1; elseif i2 > numel(comps), i2=numel(comps); end
        i2 = comps(i2).name;
      end
      
      % now set all names in between
      flag = false;
      new_match = ' ';
      for c = 1:numel(comps)
        if ~flag && strcmp(comps(c).name, i1), flag = true; end
        if flag, new_match = [ new_match comps(c).name ' ' ]; end
        if flag  && strcmp(comps(c).name, i2), flag = false; break; end
      end
      match = new_match; % replace range by list of names
    end
  end
  
  % create figure
  if ~isempty(match)
    t = sprintf('%s ', match);
    if numel(t) > 160, t=[ t(1:150) ' ...' ]; end
    set(fig, 'Name',[ 'Instrument: ' instr ': ' t ]);
  else
    set(fig, 'Name',[ 'Instrument: ' instr ]);
    t = '';
  end
  colors='bgrcmk';
  
  % plot components
  for index=1:numel(comps)
    comp = comps(index);

    if ~isempty(match)
      found = false;
      for m=1:numel(match)
        match = deblank(match);
        % OK when no pattern, or pattern is in comp.name or ' comp.name ' is in pattern
        if isempty(match) ...
        || (~any(match == ' ') && ~isempty(strfind(comp.name, match))) ...
        || ( any(match == ' ') && ~isempty(strfind(match, [ ' ' comp.name ' ' ])))
          found = true;
          break
        end
      end
    else found = true;
    end
    if ~found, continue; end % not found match -> skip this comp
    
    r = [ comp.x ; comp.y ; comp.z ];
    if all(isnan(r)), continue; end
    R = comp.rot*r;
    x = R(1,:)+comp.pos(1);
    y = R(2,:)+comp.pos(2);
    z = R(3,:)+comp.pos(3);
    disp([' Component: ' comp.name ' [' num2str(index) '] AT (' sprintf('%g,%g,%g', comp.pos) ')'  ])
    c = mod(comp.index, numel(colors)); c=colors(c+1);
    h = plot3(z,x,y, [ c '-' ], 'DisplayName', comp.name);
    popup=uicontextmenu;
    uimenu(popup,'label', comp.name,'ForeGroundColor',c);
    uimenu(popup,'label', [ 'AT: ' mat2str(comp.pos) ]);
    set(h,'uicontextmenu',popup);
    hold on
  end
  if isempty(match)
    % plot a red dashed line that follows the centre location of components
    centers = [ 0 0 0 ];
    for index=1:numel(comps)
      comp = comps(index);
      centers = [ centers ; comp.pos(:)' ];
    end
    plot3(centers(:,3), centers(:,1), centers(:,2), 'r:', 'DisplayName', model);
  end
  
  xlabel('Z [m]');
  ylabel('X [m]');
  zlabel('Y [m]');
  daspect([1 1 1]);
  box on;
  a0 = gca;
  if exist ('OCTAVE_VERSION', 'builtin')
    legend show
  end

  t = { sprintf([ 'Instrument: ' model ' ' match '\n']) ; t };
  if exist('trextwrap'), t = textwrap(t, 80); end
  t = sprintf('%s\n', t{:});
  if numel(t) > 160, t = [ t(1:150) ' ...' ]; end
  title(t ,'Interpreter','None');

  plot_contextmenu(gca, instr, t);
  [~,filename] = fileparts(strtok(instr));
  if isempty(filename), filename = 'instrument'; end

  if ~isempty(match)
    t = match;
    if numel(t) > 20, t = t(1:20); end
    filename = [ filename t ];
  end
  
  % export save_as to x3d/xhtml
  if exist('figure2xhtml')
    if ~isempty(strfind(save_as, 'html'))
      t(t=='<')='[';
      t(t=='>')=']';
      figure2xhtml(filename, fig, ...
        struct('title', instr, 'Description',t,'interactive',true));
      plot_exportmessage([ filename '.xhtml' ])
      plot_exportmessage([ filename '.x3d' ])
    end
    if ~isempty(strfind(save_as, 'x3d'))
      t(t=='<')='[';
      t(t=='>')=']';
      figure2xhtml(filename, fig, struct('interactive',true, ...
        'output', 'x3d','title',instr,'Description',t));
      plot_exportmessage([ filename '.x3d' ])
    end
  end
  if exist('plot2svg') && ~isempty(strfind(save_as, 'svg'))
    plot2svg([ filename '.svg' ], fig);
    plot_exportmessage([ filename '.svg' ])
  end
  
  % export to static images
  for f={'png','pdf','fig','tif','jpg','eps'}
    if ~isempty(strfind(save_as, f{1}))
      try
        saveas(fig, [ filename '.' f{1} ], f{1});
        plot_exportmessage([ filename '.' f{1} ])
      catch ME
        disp(getReport(ME))
      end
    end
  end
  axes(a0);

end % plot

% ------------------------------------------------------------------------------
% initialize the component structures by searching name and pos/rot, e.g.
%   COMPONENT: "collimador_radial"
%   POS: 1.82089, 0, 19.6314, 0.822317, 0, -0.569029, -0, 1, 0, 0.569029, -0, 0.822317
function comps = mcdisplay_get_components(output)
  token = 'COMPONENT: "';
  index_token = find(~cellfun(@isempty, strfind(output, token)));
  comps = [];
  for index = index_token'
    this_line = output{index};
    compname  = strtok(this_line(numel(token):end),'"');
    pos = [];
    if index < numel(output)
      next_line = output{index+1};
      if strncmp(next_line, 'POS: ', 5), pos = str2num(next_line(6:end)); end
    end

    if ~isempty(compname) && numel(pos) == 12
      comp.name = compname;
      comp.pos  = pos(1:3);                 % absolute position
      comp.rot  = reshape(pos(4:end),3,3);  % absolute rotation matrix
      comp.index= numel(comps)+1;
      comp.x=[]; comp.y=[]; comp.z=[];
      comps = [ comps comp ];
    end
  end
end

% search for a plot command: multiline or circle. Get the local coords back.
function [X,Y,Z] = mcdisplay_get_token(output, token)
  index_token = find(~cellfun(@isempty, strfind(output, [ 'MCDISPLAY: ' token ])));
  X=[]; Y=[]; Z=[];
  if isempty(index_token), return; end
  for index = index_token(:)'
    % in each multiline, we replace the search token and execute the remaining part in eval
    this_line = output{index};
    this_line = strrep(this_line, 'MCDISPLAY: ','[x,y,z]=');
    eval([ this_line ';' ]);  % executes multiline(..), return a set of points (local coords)
    X = [ X NaN x ];
    Y = [ Y NaN y ];
    Z = [ Z NaN z ];
  end
end

% get the multiline and circle arguments back
function [X,Y,Z]=multiline(npoints, varargin)
  X=[]; Y=[]; Z=[];
  for index=1:3:numel(varargin)
    X=[ X varargin{index} ];
    Y=[ Y varargin{index+1} ];
    Z=[ Z varargin{index+2} ];
  end
end

function [X,Y,Z]=mcdiscircle(plane, x0,y0,z0, radius)
  % we create a set of points
  if radius ~=0
    phi=linspace(0,2*pi, 36); % 36 points along the circle
  else phi=0; end
  x=radius*sin(phi);
  y=radius*cos(phi);
  zero = 0*x;
  switch plane
  case {'xy','yx'}
    X=x; Y=y; Z=zero;
  case {'xz','zx'}
    X=x; Y=zero; Z=y;
  case {'zy','yz'}
    X=zero; Y=x; Z=y;
  otherwise
    X=[]; Y=[]; Z=[]; 
    warning([ mfilename ': unknown plane: ' plane ]);
    return
  end
  X=X+x0; Y=Y+y0; Z=Z+z0;
end

function plot_contextmenu(a, name, pars)
  % install a context menu on the axis object of the figure
  
  % build the contextual menu
  if iscell(pars), pars = sprintf('%s ', pars{:}); end
  uicm = uicontextmenu('Tag','plot_contextmenu_gca');
  uimenu(uicm, 'Label', [ 'About ' name '...' ], ...
           'Callback', [ 'helpdlg(''' strrep(pars, sprintf('\n'),' ') ''',''' name ''')' ]);
    
  uimenu(uicm, 'Separator','on', 'Label', 'Duplicate View...', 'Callback', ...
         [ 'tmp_cb.g=gca;' ...
           'tmp_cb.f=figure; tmp_cb.c=copyobj(tmp_cb.g,gcf); ' ...
           'set(tmp_cb.c,''position'',[ 0.1 0.1 0.85 0.8]);' ...
           'set(gcf,''Name'',''Copy of McCode display: ' name '''); ' ...
           'set(gca,''XTickLabelMode'',''auto'',''XTickMode'',''auto'');' ...
           'set(gca,''YTickLabelMode'',''auto'',''YTickMode'',''auto'');' ...
           'set(gca,''ZTickLabelMode'',''auto'',''ZTickMode'',''auto'');']);
           
  uimenu(uicm, 'Label','Toggle grid', 'Callback','grid');
  uimenu(uicm, 'Label','Toggle aspect ratio','Callback','if all(daspect == 1) daspect(''auto''); else daspect([ 1 1 1 ]); end');
  uimenu(uicm, 'Label','Toggle Perspective','Callback', 'if strcmp(get(gca,''Projection''),''orthographic'')  set(gca,''Projection'',''perspective''); else set(gca,''Projection'',''orthographic''); end');
  if exist ('OCTAVE_VERSION', 'builtin')
  uimenu(uicm, 'Label','Toggle legend','Callback','legend(''toggle'');');
  else
  uimenu(uicm, 'Label','Toggle legend','Callback','tmp_h=legend(''toggle''); set(tmp_h,''Interpreter'',''None''); if strcmp(get(tmp_h,''Visible''),''off''), legend(gca,''off''); end; clear tmp_h;');
  end
  uimenu(uicm, 'Label','Top View (XZ)', 'Callback', 'view([0 90])')
  uimenu(uicm, 'Label','Side View (YZ)', 'Callback', 'view([0 0])')
  uimenu(uicm, 'Label','Front View (XY)', 'Callback', 'view([90 0])')
  uimenu(uicm, 'Label','Reset Flat/3D View', 'Callback', [ ...
      '[tmp_a,tmp_e]=view; if (tmp_a==0 & tmp_e==90) view(3); else view(2); end;' ...
      'clear tmp_a tmp_e; rotate3d off;axis tight;' ]);
  
  % attach the contextual menu
  set(a, 'UIContextMenu', uicm);
  
  % ==============================================================================
  % contextual menu for the figure
  % add rotate/pan/zoom tools to the figure in case java machine is not started
  if ~usejava('jvm')
    if isempty(get(gcf, 'UIContextMenu'))
      uicmf = uicontextmenu('Tag','iData_plot_contextmenu_fig');
      uimenu(uicmf, 'Label','Zoom on/off', 'Callback','zoom');
      uimenu(uicmf, 'Label','Pan on/off',  'Callback','pan');
      if ndims(a) >= 2
        uimenu(uicmf, 'Label', 'Rotate on/off', 'Callback','rotate3d');
      end
      if exist ('OCTAVE_VERSION', 'builtin')
      uimenu(uicmf, 'Label','Legend on/off', 'Callback','legend(''toggle'');');
      else
      uimenu(uicmf, 'Label','Legend on/off', 'Callback','legend(gca, ''toggle'',''Location'',''Best'');');
      end
      uimenu(uicmf, 'Label','Print...', 'Callback','printpreview');

      set(gcf, 'UIContextMenu', uicmf);
      set(gcf, 'KeyPressFcn', @(src,evnt) eval('if lower(evnt.Character)==''r'', axis tight;rotate3d off; zoom off; pan off; end') );
    end
  end
    
end

function plot_exportmessage(filename)
  if ~isdeployed
    disp([ mfilename ': exported instrument view as: <a href="' filename '">' filename '</a>'])
  else
    disp([ mfilename ': exported instrument view as: ' filename ])
  end
end

function instr = mccode_search_instrument(instr, d)

  % get/search instrument
  % check if the instrument exists, else attempt to find it

  if strncmp(instr,'http://',7) || strncmp(instr,'https://',8) || strncmp(instr,'ftp://',6)
    tmpfile = tempname;
    % Keep file extension, may be useful for iData load
    [filepath,name,ext] = fileparts(instr);
    tmpfile = [tmpfile ext];
    use_wget = false;
    if ~usejava('jvm')
      use_wget = true;
    else
      % access the net. Proxy settings must be set (if any).
      try
        % write to temporary file
        tmpfile = urlwrite(instr, tmpfile);
      catch ME
        use_wget = true;
      end
    end
    if use_wget
      % Fall back to using wget
      cmd = ['wget ' instr ' -O ' tmpfile]; disp(cmd)
      [status, result] = system(cmd);
      if status
        disp(result);
        error([ mfilename ': Can not get URL ' instr ]);
      end
    end
    instr = tmpfile;
  end


  if ~isempty(instr)
    index = dir(instr);
  else return;
  end

  if ~isempty(index), return; end % given file is fully qualified

  for ext={'.instr','.out','.exe',''}
    out = [ instr ext{1} ];
    % check for instrument in McStas/McXtrace libraries
    search_dir = { '.', d, getenv('MCSTAS'), getenv('MCXTRACE'), ...
      '/usr/local/lib/mc*', 'C:\mc*', '/usr/share/mcstas/'};
    if isempty(index)
      % search the instrument recursively in all existing directories in this list
      index = getAllFiles(search_dir, out);
      % check if we have more than one match
      if ~isempty(index)
        if numel(index) > 1
          % filter with extension
          found = {};
          for i=1:numel(index)
            [p,f,e] = fileparts(index{i});
            if strcmp(e, '.instr'), found{end+1} = index{i}; end
          end
          if ~isempty(found), index=found; end
          disp([ mfilename ': Found instruments:' ] );
          fprintf(1,'  %s\n', index{:});
        end
        instr = index;
        return
      end
    end
  end

  if isempty(index)
    disp([ mfilename ': ERROR: Can not find instrument ' instr ]);
    instr = '';
  end
end


function [info, exe] = instrument_get_info(executable)
  % calls the instrument with --info or --help to attempt to get information
  % parse the output and return a structure.

  info = ''; exe = '';
  if ismac,      precmd = 'DYLD_LIBRARY_PATH= ; DISPLAY= ; ';
  elseif isunix, precmd = 'LD_LIBRARY_PATH= ; DISPLAY= ; ';
  else           precmd = ''; end

  [p,f,e] = fileparts(executable);

  for name={ fullfile(p,f), f,  fullfile('.',f) }
    for ext={'','.out','.exe'}
      if isempty(dir([ name{1} ext{1} ])), continue; end
      for opt={' --info',' --help',' -h'}
        % look for executable and test with various extensions
        exe = [ name{1} ext{1} ];
        [status, result] = system([ precmd exe opt{1} ]);
        if any(status == [ -1 0 255 ])
          info = result;
          return
        end
      end
    end
  end
end
