'''
mcgui UI.
'''
import sys
import os
import re
import pathlib
from widgets import *

try:
    from PyQt6 import QtWidgets, QtCore
    from PyQt6.QtWidgets import QApplication, QWidget
    import PyQt6 as PyQt
    try:
        from PyQt6 import Qsci
    except ImportError:
        Qsci = None

except ImportError:
    from PyQt5 import QtWidgets, QtCore
    from PyQt5.QtWidgets import QApplication, QWidget
    import PyQt5 as PyQt
    try:
        from PyQt5 import Qsci
    except ImportError:
        Qsci = None

sys.path.append(os.path.join(os.path.dirname(__file__), '..'))
from mccodelib import mccode_config
from mccodelib.utils import ComponentParser, ComponentParInfo

'''
View class containing windows and dialogs as delegates.
All ui widget updates are handled here.
'''
class McView(object):
    def __init__(self):
        # create main window
        self.mw = McMainWindow()
        self.mw.ui.lblInstrument.setText("")
        self.ew = McCodeEditorWindow()
        # a hack to enable mw to close ew
        self.mw.ew = self.ew

    def initMainWindowDynamicElements(self, args, callback):
        self.mw.initDynamicView(args, callback)

    def initCodeEditorComponentMenu(self, args):
        self.ew.initComponentMenu(args)

    def showMainWindow(self):
        self.mw.show()
        self.mw.raise_()
    
    def showErrorDialogue(self, title, message):
        msg = QtWidgets.QMessageBox()
        msg.setIcon(QtWidgets.QMessageBox.Critical)
        msg.setWindowTitle(title)
        msg.setText(message)
        msg.exec()
    
    def showCodeEditorWindow(self, instr):
        self.ew.initCodeEditor(instr)
        self.ew.show()
        self.ew.raise_()
    
    def closeCodeEditorWindow(self):
        return self.ew.close()
    
    ''' Update UI data
    '''
    def updateInstrument(self, labels, instr):
        ''' labels: <instrument path>, <work dir> '''
        if len(labels[0])>70:
            label=str(pathlib.Path("(..long dirname..)",pathlib.Path(labels[0]).name))
            tooltip="Located in: "+str(pathlib.Path(labels[0]).parent)
        else:
            label=labels[0]
            tooltip=labels[0]
        self.mw.ui.lblInstrument.setText(label)
        self.mw.ui.lblInstrument.setToolTip(tooltip)
        if str(labels[0]) == '':
            self.__ssd = None
        if Qsci:
            self.ew.initCodeEditor(instr)

    def updateStatus(self, text=''):
        self.mw.ui.statusbar.showMessage(text)

    def updateLog(self, text='', error=False, gui=False, clear=False):
    
        if clear:
            self.mw.ui.txtbrwMcgui.setText('Cleared messages.')
        if error:
            self.mw.ui.txtbrwMcgui.setTextColor(QtGui.QColor(mccode_config.configuration["GUIERRCOLOR"]))
        elif gui:
            self.mw.ui.txtbrwMcgui.setTextColor(QtGui.QColor(mccode_config.configuration["GUILOGCOLOR"]))
        else:
            self.mw.ui.txtbrwMcgui.setTextColor(QtGui.QColor(mccode_config.configuration["GUIINFOCOLOR"]))
        self.mw.ui.txtbrwMcgui.append(text)
    
    def disableRunBtn(self):
        self.mw.ui.btnRun.setEnabled(False)

    def enableRunBtn(self):
        self.mw.ui.btnRun.setEnabled(True)

    def updateSimState(self, state=[]):
        enableRun = state[0] == 'True'
        enablePlot = state[1] == 'True'
        enableInterrupt = False;
        if len(state)>2:
            enableInterrupt = state[2] == 'True'

        # clear start simulation dialog
        if not enableRun:
            self.__ssd = None

        # set enabled/disabled states on menus and buttons
        ui = self.mw.ui
        ui.btnRun.setEnabled(enableRun)
        ui.btnJuPy.setEnabled(enableRun)
        ui.btnEdit.setEnabled(enableRun)
        ui.btnPlot.setEnabled(enablePlot)
        if enableRun:
            ui.lblInstrument.setStyleSheet('color: ' + mccode_config.configuration["GUILOGCOLOR"])
        else:
            ui.lblInstrument.setStyleSheet('color: ' + mccode_config.configuration["GUIERRCOLOR"])
        ui.actionClose_Instrument.setEnabled(enableRun)
        ui.actionPlot.setEnabled(enablePlot)
        ui.actionDisplay.setEnabled(enableRun)
        ui.actionDisplay_2d.setEnabled(enableRun)
        ui.actionRun_Simulation.setEnabled(enableRun)
        ui.actionSave_As.setEnabled(enableRun)
        ui.actionOpen_instrument.setEnabled(True)
        ui.actionNew_Instrument.setEnabled(True)
        ui.menuNew_From_Template.setEnabled(True)
        ui.actionEdit_Instrument.setEnabled(enableRun)
        ui.actionEditExt_Instrument.setEnabled(enableRun)
        ui.actionCompile_Instrument.setEnabled(enableRun)
        ui.actionCompile_Instrument_MPI.setEnabled(enableRun)

        # set action of run button:
        if enableInterrupt:
            ui.btnRun.setText('Halt')
            ui.btnRun.setToolTip('Interrupt current simulation')
            ui.actionRun_Simulation.setEnabled(False)
            ui.actionCompile_Instrument.setEnabled(False)
            ui.actionCompile_Instrument_MPI.setEnabled(False)
            ui.actionClose_Instrument.setEnabled(False)
            ui.actionSave_As.setEnabled(False)
            ui.actionOpen_instrument.setEnabled(False)
            ui.actionNew_Instrument.setEnabled(False)
            ui.menuNew_From_Template.setEnabled(False)
        else:
            ui.btnRun.setText('Run..')
            ui.btnRun.setToolTip('Compile and Run the current instrument')

    ''' UI actions
    '''
    def showOpenInstrumentDlg(self, lookDir):
        dlg = QtWidgets.QFileDialog()
        dlg.setDirectory(lookDir)
        
        dlg.setNameFilters([mccode_config.configuration["MCCODE"]+" instruments (*.instr)", "All files (*)"]);
        dlg.selectNameFilter(mccode_config.configuration["MCCODE"]+" instruments (*.instr)")
        if dlg.exec():
            return dlg.selectedFiles()[0]
        
    def showOpenPlotDirDlg(self, lookDir):
        dlg = QtWidgets.QFileDialog()
        dlg.setDirectory(lookDir)
        # dlg.ShowDirsOnly # not supported in Qt5/Qt6/PySide
        return dlg.getExistingDirectory(self.mw,"Open a folder")
        
    def showChangeWorkDirDlg(self, lookDir):
        dlg = QtWidgets.QFileDialog()
        dlg.setFileMode(QtWidgets.QFileDialog.Directory)
        dlg.setDirectory(lookDir)
        if dlg.exec():
            return dlg.selectedFiles()[0]

    def showStartSimDialog(self, instr, params, comps, mcdisplays, mcplots, formats, buffersize):
        if self.__ssd == None:
            self.__ssd = McStartSimDialog()
        self.__ssd.ui.gbxGrid.setTitle('Instrument parameters: '+ os.path.basename(str(instr)))
        self.__ssd.createParamsWidgets(params)
        self.__ssd.set_components(comps)
        self.__ssd.set_mcdisplays(mcdisplays)
        self.__ssd.set_mcplots(mcplots)
        self.__ssd.set_formats(formats)
        self.__ssd.set_buffersize(buffersize)
        if self.__ssd.exec():
            return self.__ssd.getValues()
        else:
            return None, None, None, None, None, None

    def showNewInstrDialog(self, lookdir):
        dlg = QtWidgets.QFileDialog()
        dlg.setDirectory(lookdir)
        dlg.setNameFilter(mccode_config.configuration["MCCODE"]+" instruments (*.instr)");
        return dlg.getSaveFileName(parent=None, caption='Create Instrument file...')[0]

    def showNewInstrFromTemplateDialog(self, instr):
        dlg = QtWidgets.QFileDialog()
        return dlg.getSaveFileName(parent=None, caption='Create Instrument file from Template...', directory=instr)[0]

    def showSaveAsDialog(self, instr):
        dlg = QtWidgets.QFileDialog()
        dlg.setFileMode(QtWidgets.QFileDialog.AnyFile)
        return dlg.getSaveFileName(parent=None, caption='Save Instrument As...', directory=instr)[0]

    def showConfigDialog(self):
        dlg = McConfigDialog()
        dlg.initConfigData(None)
        dlg.exec()

    def showAboutBox(self, text):
        if mccode_config.configuration["MCCODE"] == "mcstas":
            prefix = "mc"
        else:
            prefix = "mx"
        QtWidgets.QMessageBox.about(self.mw, prefix+'gui: About', text)


''' Main Window widgets wrapper class
Events callbacks are hooked elsewhere.
'''
class McMainWindow(QtWidgets.QMainWindow):
    def __init__(self, parent=None):
        super(McMainWindow, self).__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.ui.dynamicMenuClicked = QtCore.pyqtSignal(str)

        # set main window title depending on flavour
        prefix = 'mx'
        if mccode_config.configuration["MCCODE"] == "mcstas":
            prefix = 'mc'
        self.setWindowTitle(prefix + 'gui-py')
        self.ui.actionMcdoc.setText(prefix + "doc Component Reference")
        self.ui.actionMcDocCurrent.setText(prefix +"doc current instrument")


        mccode = mccode_config.configuration["MCCODE"]
        self.ui.actionMcstas_User_Manual.setText(mccode + " User Manual")
        self.ui.actionMcstas_Component_Manual.setText(mccode + " Component Manual")
        self.ui.actionMcstas_Web_Page.setText(mccode + " Web Page")
        self.ui.lblIcon.setPixmap(QtGui.QPixmap(os.path.join(str(pathlib.Path(__file__).parent.resolve()),mccode + "-py.png")))

    def initDynamicView(self, args, callback):
        ''' - args ([str, [], []]): list of triplets consisting of site name,
                                    [instrument names], [instrument file paths]
            - callback (func(str)): function which takes a single string parameter, call with full path
                                    name of selected instrument
        '''
        class InfoHider:
            def __init__(self, itm, cb):
                self.itm = itm
                self.cb = cb
            def handle(self):
                self.cb(self.itm)
        
        self.ui.menuNew_From_Template.clear()

        for i in range(len(args)):
            site = args[i][0]
            instrs = args[i][1]
            instrs_fulpath = args[i][2]

            menu = self.ui.menuNew_From_Template.addMenu(site)

            for j in range(len(instrs)):
                action = menu.addAction(instrs[j])
                
                h = InfoHider(instrs_fulpath[j], callback)
                action.h = h
                action.triggered.connect(h.handle)
    
    def add_conf_menu(self,label):
        confmenu = QAction(self)
        self.ui.menuFile.addAction(confmenu)
        confmenu.setText(QApplication.translate("MainWindow", label, None))
        confmenu.setToolTip(QApplication.translate("MainWindow", "mccode " + label, None))
        return confmenu

    def closeEvent(self, event):
        ''' allow close down only if editor window did not reject '''
        if not self.ew.close():
            event.ignore()


''' Code editor window widgets wrapper class
'''
class McCodeEditorWindow(QtWidgets.QMainWindow):
    volatileDataExists = False
    volatileDataTransition = QtCore.pyqtSignal(bool)
    saveRequest = QtCore.pyqtSignal(str)

    def __init__(self, parent=None):
        super(McCodeEditorWindow, self).__init__(parent)
        self.ui =  Ui_EditorWindow()
        self.ui.setupUi(self)

        sheight = QtGui.QGuiApplication.primaryScreen().availableGeometry().height()
        if sheight < 1080:
            self.resize(920, sheight)

        # dynamically added widgets
        if Qsci:
            self.__scintilla = None
            self.__edtSearch = None
            self.__initScintilla()
            self.__initCallbacks()
            self.__initSearchbar()

    def __initSearchbar(self):
        ''' set focus, search action events '''
        def __sbEventFilter(subject, object, event):
            ''' focus event handler '''

            edt = QtWidgets.QLineEdit()
            edt = subject
            # handle focus on
            if event.type() == QtCore.QEvent.Type.FocusIn:
                if edt.text() == 'search...':
                    edt.setText('')
                    font = QtGui.QFont()
                    font.setItalic(False)
                    self.__edtSearch.setFont(font)
                    edt.setStyleSheet("color: black;")

            # handle focus off
            elif event.type() == QtCore.QEvent.Type.FocusOut:
                if edt.text() == '':
                    font = QtGui.QFont()
                    font.setItalic(True)
                    self.__edtSearch.setFont(font)
                    edt.setStyleSheet("color: grey;")
                    edt.setText('search...')

            # handle enter keypress (search)
            elif event.type() == QtCore.QEvent.Type.KeyPress:
                # return & enter
                if event.key() in [0x01000004, 0x01000005]:
                    self.__search(subject.text().casefold())
                # escape
                elif event.key() == 0x01000000:
                    subject.setText('')
                    self.__scintilla.setFocus()
                # tab
                #elif event.key() == 0x01000001:
                #    print "tab"

            return False

        self.__edtSearch = QtWidgets.QLineEdit()
        self.__edtSearch.setObjectName("edtSearch")
        font = QtGui.QFont()
        font.setItalic(True)
        self.__edtSearch.setFont(font)
        self.__edtSearch.setText("search...")

        # set events
        edts = self.__edtSearch
        edts.eventFilter = lambda o, e: __sbEventFilter(edts, o, e)
        edts.installEventFilter(edts)

        self.ui.vlayout.addWidget(self.__edtSearch)

    def __search(self, search):
        ''' implements a search action in scintilla '''
        # get cursor position
        i, j = self.__scintilla.getCursorPosition()
        curs = self.__scintilla.positionFromLineIndex(i, j)

        # get match position after cursor
        text = self.__scintilla.text().casefold()
        pos = str(text)[curs:].find(search)

        # get match position before cursor
        if pos == -1:
            pos = str(text).find(search)
        else:
            pos = pos + curs

        if not pos == -1:
            self.__setCursorPos(pos + len(search))
            self.__selectText(pos, pos + len(search))

    def __setCursorPos(self, pos):
        k, l = self.__scintilla.lineIndexFromPosition(pos)
        self.__scintilla.setCursorPosition(k, l)

    def __selectText(self, pos1, pos2):
        if not pos1 < pos2:
            raise Exception('__selectText: pos2 must be larger than pos1.')
        self.__scintilla.selectAll(False)
        k1, l1 = self.__scintilla.lineIndexFromPosition(pos1)
        k2, l2 = self.__scintilla.lineIndexFromPosition(pos2)
        self.__scintilla.setSelection(k1, l1, k2, l2)

    def initComponentMenu(self, args):
        ''' args - [category, comp_names[], comp_parsers[]]
        '''
        class InfoHider:
            def __init__(self, comp_parser, cb):
                self.comp_parser = comp_parser
                self.cb = cb
            def handle(self):
                self.cb(self.comp_parser)
        
        all_comp_names = []
        for i in range(len(args)):
            category = args[i][0]
            comp_names = args[i][1]
            for name in comp_names:
                all_comp_names.append(name)
            comp_parsers = args[i][2]

            menu = self.ui.menuInsert.addMenu(category)

            for j in range(len(comp_names)):
                
                h = InfoHider(comp_parsers[j], self.__handleComponentClicked)
                action = menu.addAction(comp_names[j])
                action.h = h
                action.triggered.connect(h.handle)

        if Qsci:
            self.setLexerComps(self.__scintilla.__myApi, all_comp_names)

    def initCodeEditor(self, instr):
        if instr != '':
            self.__scintilla.setText(open(instr, encoding='utf-8', errors='ignore').read())
        else:
            self.__scintilla.setText('')
        self.setWindowTitle(mccode_config.configuration["MCCODE"] + ": " + instr)
        self.assumeDataSaved()

    def assumeDataSaved(self):
        self.volatileDataTransition.emit(False)

    def save(self):
        ''' external save text hook '''
        self.__handleSaveAction()

    def closeEvent(self, event):
        ''' hook to display a "save changes?" dialog if there are unsaved changes
        '''
        if self.volatileDataExists:
            reply = QtWidgets.QMessageBox.question(self,
                                                   'The instrument has been modified.',
                                                   'Do you want to save changes?',
                                                   QtWidgets.QMessageBox.Save | QtWidgets.QMessageBox.Discard | QtWidgets.QMessageBox.Cancel,
                                                   QtWidgets.QMessageBox.Cancel)
            if reply == QtWidgets.QMessageBox.Save:
                self.saveRequest.emit(self.__scintilla.text())
                self.assumeDataSaved()
                event.accept()
            elif reply == QtWidgets.QMessageBox.Discard:
                self.assumeDataSaved()
                event.accept()
            elif reply == QtWidgets.QMessageBox.Cancel:
                event.ignore()
        else:
            event.accept()

    def __handleComponentClicked(self, comp_parser):
        dlg = McInsertComponentDialog()
        dlg.initComponentData(comp_parser)
        if dlg.exec():
            comp_type, inst_name, params, atrel = dlg.getValues()
        else:
            return
        
        

        text = "COMPONENT " + inst_name + " = " + comp_type + "("
        i_max = len(params)-1
        for i in range(len(params)):
            p = params[i]
            text += "\n    " + p[0] + "=" + p[1]
            if i < i_max:
                text += ", "

        text += ")"
        text += "\nAT (" + atrel[0] + ", " + atrel[1] + ", " + atrel[2] + ") RELATIVE " + atrel[3]
        # NOTE: the ROTATED line may be missing
        if len(atrel) > 4:
            text += "\nROTATED (" + atrel[4] + ", " + atrel[5] + ", " + atrel[6] + ") RELATIVE " + atrel[7]

        self.__scintilla.insert(text)

        # set cursor position
        i, j = self.__scintilla.getCursorPosition()
        pos = self.__scintilla.positionFromLineIndex(i, j)
        k, l = self.__scintilla.lineIndexFromPosition(pos + len(text))
        self.__scintilla.setCursorPosition(k, l)

    def __initScintilla(self):
        # delete text editor placeholder
        assert Qsci
        scintilla = Qsci.QsciScintilla(self)

        ########################
        # setup scintilla
        # set default font
        font = QtGui.QFont()
        font.setFixedPitch(True)
        font.setPointSize(int(mccode_config.configuration["GUIFONTSIZE"]))

        # brace matching
        scintilla.setBraceMatching(Qsci.QsciScintilla.BraceMatch.SloppyBraceMatch)

        # set lexer
        lexer = Qsci.QsciLexerCPP()
        lexer.setDefaultFont(font)
        lexer.setFont(font)
        scintilla.setLexer(lexer)

        scintilla.setLexer(lexer)
        scintilla.__myLexer = lexer # save reference to retain scope

        # auto-completion api
        scintilla.__myApi = Qsci.QsciAPIs(lexer)

        scintilla.setAutoCompletionThreshold(1)
        scintilla.setAutoCompletionSource(Qsci.QsciScintilla.AutoCompletionSource.AcsAPIs)

        # remove horizontal scrollbar
        scintilla.SendScintilla(Qsci.QsciScintilla.SCI_SETHSCROLLBAR, 0)

        # display default line numbers
        fm = QtGui.QFontMetrics(font)
        scintilla.setMarginWidth(0, fm.horizontalAdvance( "00000" ))
        scintilla.setMarginLineNumbers(0, True)
        ########################

        # insert widget into main vlayout
        self.ui.vlayout.addWidget(scintilla)
        self.__scintilla = scintilla

    @staticmethod
    def setLexerComps(api, all_comp_names):
        api.clear()

        # add mcstas meta keywords
        api.add("ABSOLUTE")
        api.add("AT")
        api.add("COMPONENT")
        api.add("DECLARE")
        api.add("DEFINE")
        api.add("DEFINITION")
        api.add("END")
        api.add("MCDISPLAY")
        api.add("FINALLY")
        api.add("INITIALIZE")
        api.add("INSTRUMENT")
        api.add("OUTPUT")
        api.add("PARAMETERS")
        api.add("RELATIVE")
        api.add("ROTATED")
        api.add("PREVIOUS")
        api.add("SETTING")
        api.add("STATE")
        api.add("POLARISATION")
        api.add("TRACE")
        api.add("SHARE")
        api.add("EXTEND")
        api.add("GROUP")
        api.add("SAVE")
        api.add("JUMP")
        api.add("WHEN")
        api.add("NEXT")
        api.add("ITERATE")
        api.add("MYSELF")
        api.add("COPY")
        api.add("SPLIT")
        api.add("REMOVABLE")
        api.add("DEPENDENCY")
        api.add("SHELL")
        # add components
        for name in all_comp_names:
            api.add(name)

        api.prepare()

    def __initCallbacks(self):
        # connect menu items to corresponding scintilla slots
        ui = self.ui
        ui.actionUndo.triggered.connect(self.__scintilla.undo)
        ui.actionRedo.triggered.connect(self.__scintilla.redo)
        ui.actionSelect_All.triggered.connect(lambda: self.__scintilla.selectAll()) # why is l. expr. needed here?
        ui.actionCopy.triggered.connect(self.__scintilla.copy)
        ui.actionCut.triggered.connect(self.__scintilla.cut)
        ui.actionPaste.triggered.connect(self.__scintilla.paste)
        ui.actionSave.triggered.connect(self.__handleSaveAction)
        ui.actionClose_Instrument_Editor.triggered.connect(self.close)
        ui.actionFind.triggered.connect(lambda: self.__edtSearch.setFocus())
        ui.actionComponent_Browser.triggered.connect(self.__handleComponentBrowser)

        # TODO: create a ctr-a on a menu to __scintilla.selectAll(bool select)

        def __keyEventFilterFct(subject, object, event):
            if event.type() == QtCore.QEvent.Type.KeyRelease:
                # ctrl-q
                if event.key() == 81 and int(event.modifiers()) == 67108864:
                    self.close()
            return False
        self.eventFilter = lambda o, e: __keyEventFilterFct(self.ui, o, e)
        self.installEventFilter(self)

        # connect "text changed" signal to our handler to detect unsaved changes
        self.__scintilla.textChanged.connect(self.__handleTextChanged)

        self.volatileDataTransition.connect(self.__handleVolatileDataPresent)

    def __handleComponentBrowser(self):
        dlg = QtWidgets.QFileDialog()
        dlg.setDirectory(mccode_config.configuration["MCCODE_LIB_DIR"])
        dlg.setNameFilter(mccode_config.configuration["MCCODE"]+"component files (*.comp)");
        if dlg.exec():
            comp_file = dlg.selectedFiles()[0]
            parser = ComponentParser(comp_file)
            self.__handleComponentClicked(parser)

    def __handleTextChanged(self):
        if not self.volatileDataExists:
            self.volatileDataTransition.emit(True)

    def __handleSaveAction(self):
        if Qsci:
            if self.volatileDataExists:
                self.saveRequest.emit(self.__scintilla.text())


    def __handleVolatileDataPresent(self, volatileDataExists=False):
        if volatileDataExists:
            title = self.windowTitle()
            self.setWindowTitle('*' + title)
        else:
            title = str(self.windowTitle())
            self.setWindowTitle(title.replace('*', ''))
        self.volatileDataExists = volatileDataExists
        self.ui.actionSave.setEnabled(volatileDataExists)


''' Start simulation widgets wrapper class
Programatically alters the dialog to match current instrument.
Supports reuse of widgets from sim to sim, to retain input values.
Works as a dialog - call _exec(), probe for return behavior and
state to proceed.
'''
class McStartSimDialog(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super(McStartSimDialog, self).__init__(parent)
        self._last_inspect_compnames = None
        self._last_mcdisplays = None
        self._last_mcplots = None
        self._last_formats = None
        self._last_buffersize = mccode_config.configuration["NDBUFFERSIZE"]
        self.ui = Ui_dlgStartSim()
        self.ui.setupUi(self)
        if not mccode_config.configuration["PARTICLE"] == "neutron":
            self.ui.lblGravity.setHidden(True)
            self.ui.cbxGravity.setHidden(True)

        self.ui.btnStart.clicked.connect(self.accept)
        self.ui.btnCancel.clicked.connect(self.reject)
        self._set_inspect_visible(False)
        self.ui.cbxSimTrace.currentIndexChanged.connect(lambda i: self._set_inspect_visible(i))

    def set_components(self, compnames):
        if compnames == self._last_inspect_compnames:
            return
        self._last_inspect_compnames = compnames
        self.ui.cbxInspect.clear()
        self.ui.cbxInspect.addItem("-- None --")
        for c in compnames:
            self.ui.cbxInspect.addItem(c)

    def set_mcdisplays(self, mcdisplays):
        if mcdisplays == self._last_mcdisplays:
            return
        self._last_mcdisplays = mcdisplays
        self.ui.cbxMcdisplays.clear()
        for m in mcdisplays:
            self.ui.cbxMcdisplays.addItem(m)

    def set_mcplots(self, mcplots):
        if mcplots == self._last_mcplots:
            return
        self._last_mcplots = mcplots
        self.ui.cbxAutoPlotters.clear()
        self.ui.cbxAutoPlotters.addItem("-- None --")
        for m in mcplots:
            self.ui.cbxAutoPlotters.addItem(m)

    def set_formats(self, formats):
        if formats == self._last_formats:
            return
        self._last_formats = formats
        self.ui.cbxFormats.clear()
        for f in formats:
            self.ui.cbxFormats.addItem(f)

    def set_buffersize(self, buffersize):
        self.ui.edtNDbufsiz.setText(buffersize)

    def _set_inspect_visible(self, sim_run_idx):
        visible = False
        if sim_run_idx == 1 or sim_run_idx == 2:
            visible = True
        self.ui.lblInspect.setVisible(visible)
        self.ui.cbxInspect.setVisible(visible)
        self.ui.lblMcdisplays.setVisible(visible)
        self.ui.cbxMcdisplays.setVisible(visible)
        self.ui.lblAutoPlot.setVisible(not visible)
        self.ui.cbxAutoPlotters.setVisible(not visible)
        self.ui.lblFormat.setVisible(not visible)
        self.ui.cbxFormats.setVisible(not visible)
        if sim_run_idx == 2:
            self.ui.lblMcdisplays.setVisible(False)
            self.ui.cbxMcdisplays.setVisible(False)
        
    def getValues(self):
        ''' Return values:

            fixed_params[]:
                0 - simulation = 0, trace = 1
                1 - neutron/photon count (int)
                2 - steps count (int)
                3 - gravity (bool)
                4 - clustering 0/1/2 (single/MPI/MPIrecompile) (int)
                5 - clustering # nodes (int)
                6 - random seed (int)
                7 - output directory (str)
                8 - autoplotter
                9 - format
               10 - Monitor_nD buffersize
            params[]:
                [<par_name>, <value>] pairs
        '''
        # simulation, trace or optimize option
        p0 = None
        if self.ui.cbxSimTrace.currentIndex() == 0:
            p0 = SimTraceEnum.SIM
        elif self.ui.cbxSimTrace.currentIndex() == 1:
            p0 = SimTraceEnum.TRACE
        elif self.ui.cbxSimTrace.currentIndex() == 2:
            p0 = SimTraceEnum.OPTIMIZE
        else:
            raise Exception('simdialog.getValues: invalid execution mode (simulate/trace/optimize).')

        # neutron/photon count
        p1 = self.ui.edtNeutronCnt.text()

        # steps
        p2 = self.ui.edtSteps.text()

        # gravity
        p3 = self.ui.cbxGravity.currentIndex() == 1

        # clustering option
        p4 = None
        if self.ui.cbxClustering.currentIndex() == 0:
            p4 = ClusteringEnum.SINGLE
        if self.ui.cbxClustering.currentIndex() == 1:
            p4 = ClusteringEnum.MPI
        if self.ui.cbxClustering.currentIndex() == 2:
            p4 = ClusteringEnum.MPI_RC
        if self.ui.cbxClustering.currentIndex() == 3:
            p4 = ClusteringEnum.OACC
        if self.ui.cbxClustering.currentIndex() == 4:
            p4 = ClusteringEnum.OACC_RC
        if self.ui.cbxClustering.currentIndex() == 5:
            p4 = ClusteringEnum.OACC_MPI
        if self.ui.cbxClustering.currentIndex() == 6:
            p4 = ClusteringEnum.OACC_MPI_RC
        
        # clustring option
        p5 = self.ui.edtNodes.text()
        
        # seed
        p6 = self.ui.edtRandomSeed.text()
        
        # output dir
        p7 = str(self.ui.edtOutputDir.text())
        
        # autoplot
        mcplot = None
        idx = self.ui.cbxAutoPlotters.currentIndex()
        p8 = idx > 0
        if idx > 0:
            mcplot = self._last_mcplots[idx-1]

        # format
        Format = None
        idx = self.ui.cbxFormats.currentIndex()
        p9 = idx > 0
        if idx > 0:
            Format = self._last_formats[idx]

        # Monitor_nD buffersize
        p10 = self.ui.edtNDbufsiz.text()
 
        fixed_params =[p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10]
        
        # get dynamic params
        params = []
        for w in self._wParams:
            p = []
            p.append(str(w[0].text()).rstrip(':').strip())
            p.append(str(w[1].text()))
            params.append(p)
        
        inspect = None
        idx = self.ui.cbxInspect.currentIndex()
        if idx > 0:
            inspect = self._last_inspect_compnames[idx]
        
        mcdisplay = None
        idx = self.ui.cbxMcdisplays.currentIndex()
        if p0 == SimTraceEnum.TRACE:
            mcdisplay = self._last_mcdisplays[idx]
        
        return fixed_params, params, inspect, mcdisplay, mcplot, Format

    _wParams = []
    __oldParams = []
    def createParamsWidgets(self, params):

        # this logics keeps params values of existing/previous non-dummy widgets, for value reuse
        self.__oldParams = []
        for w in self._wParams:
            old_name = 'no_re_match'
            name_match = re.search('(.*):', w[0].text())
            if name_match:
                old_name = name_match.group(1)
            old_value = w[1].text()
            self.__oldParams.append([old_name, old_value])

        # clear the containing grid
        grd = self.ui.gridGrid
        for i in reversed(range(grd.count())):
            grd.itemAt(i).widget().setParent(None)

        # prepare new params widgets
        self._wParams = []

        # insert custom params widgets
        i = -1
        x = 0
        y = 0
        p_index = 0
        for p in params:
            # get param name, value
            name = p[0]
            value = p[1]

            # reuse old param values, if matching position in grid (p_index) and param name
            if len(self.__oldParams) > p_index:
                old_name = self.__oldParams[p_index][0]
                old_value = self.__oldParams[p_index][1]
                if str(old_name) == str(name):
                    value = old_value

            i = i + 1
            x = i % (int(mccode_config.configuration["GUICOLS"])*2)
            y = i // (int(mccode_config.configuration["GUICOLS"])*2)

            lbl = QtWidgets.QLabel(self.ui.gbxGrid)
            lbl.setAlignment(QtCore.Qt.AlignmentFlag.AlignRight|QtCore.Qt.AlignmentFlag.AlignTrailing|QtCore.Qt.AlignmentFlag.AlignVCenter)
            lbl.setObjectName("lbl" + name)
            lbl.setText(name + ':')
            self.ui.gridGrid.addWidget(lbl, y, x, 1, 1)

            i = i + 1
            x = i % (int(mccode_config.configuration["GUICOLS"])*2)

            edt = QtWidgets.QLineEdit(self.ui.gbxGrid)
            edt.setAlignment(QtCore.Qt.AlignmentFlag.AlignRight|QtCore.Qt.AlignmentFlag.AlignTrailing|QtCore.Qt.AlignmentFlag.AlignVCenter)
            edt.setObjectName("edt" + name)
            edt.setText(value)
            self.ui.gridGrid.addWidget(edt, y, x, 1, 1)

            self._wParams.append([lbl, edt])

            p_index += 1
                
        self.ui.btnStart.setFocus()

class SimTraceEnum:
    SIM = 0
    TRACE = 1
    OPTIMIZE = 2

class ClusteringEnum:
    SINGLE = 0
    MPI = 1
    MPI_RC = 2
    OACC = 3
    OACC_RC = 4
    OACC_MPI = 5
    OACC_MPI_RC = 6

''' Start simulation widgets wrapper class
Programatically alters the dialog to match current instrument.
Supports reuse of widgets from sim to sim, to retain input values.
Works as a dialog - call _exec(), probe for return behavior and
state to proceed.
'''
class McInsertComponentDialog(QtWidgets.QDialog):
    __standard_le_style = None
    def __init__(self, parent=None):
        super(McInsertComponentDialog, self).__init__(parent)
        self.ui = Ui_dlgInsertComponent()
        self.ui.setupUi(self)
        self.ui.btnInsert.clicked.connect(self.accept)
        self.ui.btnCancel.clicked.connect(self.reject)

        self.__standard_le_style = self.ui.edtInstanceName.styleSheet()

    def accept(self):
        # detect missing default values
        dirty = False

        # mark/unmark params dynamic lineedits
        first_params_hit = True
        for w in self._wParams:
            if w[1].text() == '':
                w[1].setStyleSheet("border: 3px solid red;")
                dirty = True
                if first_params_hit:
                    w[1].setFocus()
                    first_params_hit = False
            else:
                w[1].setStyleSheet(self.__standard_le_style)

        # mark/unmark instance name lineedit
        if self.ui.edtInstanceName.text() == '':
            self.ui.edtInstanceName.setStyleSheet("border: 3px solid red;")
            if not dirty:
                self.ui.edtInstanceName.setFocus()
            dirty = True
        else:
            self.ui.edtInstanceName.setStyleSheet(self.__standard_le_style)

        # exit if all lineedit text boxes are filled out
        if not dirty:
            super(McInsertComponentDialog, self).accept()

    _wParams = []
    def initComponentData(self, comp_parser):
        # parse component info
        comp_parser.parse()

        # window title
        self.setWindowTitle("Component: " + comp_parser.name)

        # info & description docstrings - make sure newlines work in case doc includes html
        info_description = comp_parser.info + '\n\n' + comp_parser.description
        info_description_html = str(info_description).replace('\n', '<br>')
        self.ui.lblDescr.setText(info_description_html)

        # clear params grd
        grd = self.ui.gridParameters
        for i in reversed(range(grd.count())):
            grd.itemAt(i).widget().setParent(None)

        # populate and init params grd
        self._wParams = None
        self._wParams = []
        for i in range(len(comp_parser.pars)):
            par = ComponentParInfo(comp_parser.pars[i])
            if par.par_name == "string filename":
                par.par_name = "filename"

            # i'th line/row of the UI
            y = i

            # parameter name label
            x = 0
            lbl = QtWidgets.QLabel()
            lbl.setObjectName("lbl" + par.par_name)
            lbl.setText(par.par_name + ':')
            self.ui.gridParameters.addWidget(lbl, y, x, 1, 1)

            # parameter value line-edit
            x = 1
            edt = QtWidgets.QLineEdit()
            edt.setAlignment(QtCore.Qt.AlignmentFlag.AlignRight|QtCore.Qt.AlignmentFlag.AlignTrailing|QtCore.Qt.AlignmentFlag.AlignVCenter)
            edt.setObjectName("edt" + par.par_name)
            edt.defval = par.default_value
            self._initTbwFocusEvents(edt)
            if par.par_name == "filename":
                edt.setText('"' + par.default_value + '"')
            else:
                edt.setText(par.default_value)
            self.ui.gridParameters.addWidget(edt, y, x, 1, 1)

            # save widget references for use in self.getValues (also save the par default value)
            self._wParams.append([lbl, edt, edt.text()])

            # parameter docstring label
            x = 2
            lbl = QtWidgets.QLabel()
            lbl.setWordWrap(True)
            lbl.setObjectName("lbl" + par.par_name + "_doc")
            lbl.setText(par.doc_and_unit)
            self.ui.gridParameters.addWidget(lbl, y, x, 1, 1)

        # fix tab-order
        q = self.ui.btnInsert
        for i in range(len(self._wParams)):
            w = self._wParams[i][1]
            self.setTabOrder(q, w)
            q = w
        self.setTabOrder(q, self.ui.edtAtX)

        # init instance-name field with an example, mark the text
        tbx = self.ui.edtInstanceName
        tbx.setText(str.lower(comp_parser.name))
        tbx.setFocus()
        tbx.selectAll()
    
    def _initTbwFocusEvents(self, w):
        ''' we assume that w_edt has the member defval, which contains the default value '''
        def _wEventFilter(subject, object, event):
            ''' focus event handler '''
            edt = QtWidgets.QLineEdit()
            edt = subject
            # handle focus on
            if event.type() == QtCore.QEvent.Type.FocusIn:
                if edt.text() == edt.defval:
                    edt.setText('')
                    font = QtGui.QFont()
                    font.setItalic(False)
                    edt.setFont(font)
                    edt.setStyleSheet("color: black;")
                    edt.setCursorPosition(0)
            
            # handle focus off
            elif event.type() == QtCore.QEvent.Type.FocusOut:
                if edt.text() == '': # Empty
                    font = QtGui.QFont()
                    font.setItalic(True)
                    edt.setFont(font)
                    edt.setStyleSheet("color: grey;")
                    edt.setText(edt.defval)
                elif edt.text() == edt.defval: # Default value
                    edt.setText(edt.defval)
                    font = QtGui.QFont()
                    font.setItalic(True)
                    edt.setFont(font)
                    edt.setStyleSheet("color: grey;")
                else: # Something else
                    font = QtGui.QFont()
                    edt.setFont(font)
                    edt.setStyleSheet("color: black;")
            return False

        # init
        font = QtGui.QFont()
        font.setItalic(True)
        w.setStyleSheet("color: grey;")
        w.setFont(font)
        w.setText(w.defval)
        
        # set events
        w.eventFilter = lambda o, e: _wEventFilter(w, o, e)
        w.installEventFilter(w)
    
    def getValues(self):
        '''
        inst_name : contents of instance name field
        params : list of [name, value] pairs matching component parameters
        '''
        if not self.ui.cbxVerbose.isChecked():
            return self.__getValuesReduced()

        # instance name
        inst_name = self.ui.edtInstanceName.text()
        m = re.match("Component: (.*)", self.windowTitle())
        comp_type = m.group(1)

        # get dynamic params
        params = []
        for w in self._wParams:
            p = []
            p.append(str(w[0].text()).rstrip(':'))
            p.append(str(w[1].text()))
            params.append(p)

        # get values for AT(x,y,z), RELATIVE <posrel>, ROTATED(x,y,z), RELATIVE <rotrel>
        atrel = []
        atrel.append(self.ui.edtAtX.text())
        atrel.append(self.ui.edtAtY.text())
        atrel.append(self.ui.edtAtZ.text())
        atrel.append(self.ui.edtAtRel.text())
        atrel.append(self.ui.edtRotX.text())
        atrel.append(self.ui.edtRotY.text())
        atrel.append(self.ui.edtRotZ.text())
        atrel.append(self.ui.edtRotRel.text())

        return comp_type, inst_name, params, atrel

    def __getValuesReduced(self):
        '''
        inst_name : contents of instance name field
        params : list of [name, value] pairs matching component parameters
        '''
        # instance name
        inst_name = self.ui.edtInstanceName.text()
        m = re.match("Component: (.*)", self.windowTitle())
        comp_type = m.group(1)

        # get dynamic params
        params = []
        for w in self._wParams:
            # proceed if typed value differs from the default value (also counting empty default values)
            if w[1].text() != w[2]:
                p = []
                p.append(str(w[0].text()).rstrip(':'))
                p.append(str(w[1].text()))
                params.append(p)

        # get values for AT(x,y,z), RELATIVE <posrel>, ROTATED(x,y,z), RELATIVE <rotrel>
        atrel = []
        atrel.append(self.ui.edtAtX.text())
        atrel.append(self.ui.edtAtY.text())
        atrel.append(self.ui.edtAtZ.text())
        atrel.append(self.ui.edtAtRel.text())
        if self.ui.edtRotX.text() != '0' or self.ui.edtRotY.text() != '0' or self.ui.edtRotZ.text() != '0':
            atrel.append(self.ui.edtRotX.text())
            atrel.append(self.ui.edtRotY.text())
            atrel.append(self.ui.edtRotZ.text())
            atrel.append(self.ui.edtRotRel.text())

        return comp_type, inst_name, params, atrel


''' mcgui config widgets wrapper class
'''
class McConfigDialog(QtWidgets.QDialog):
    __standard_le_style = None
    def __init__(self, parent=None):
        super(McConfigDialog, self).__init__(parent)
        self.ui = Ui_dlgConfig()
        self.ui.setupUi(self)

        self.ui.btnOk.clicked.connect(self.accept)
        self.ui.btnSave.clicked.connect(self.save)
        self.ui.btnSaveEdit.clicked.connect(self.saveedit)
        self.ui.btnCancel.clicked.connect(self.reject)
        
        # set labels mccode-prefix
        prefix = mccode_config.get_mccode_prefix()
        self.ui.lblMcrun.setText("%srun" % prefix)
        self.ui.lblMcplot.setText("%splot" % prefix)
        self.ui.lblMcdisplay.setText("%sdisplay" % prefix)
        
        
    def initConfigData(self, args):
        # comboboxes
        mcrun_lst, mcplot_lst, mcdisplay_lst, format_lst = mccode_config.get_options()

        # mcrun combobox
        selected_val = mccode_config.configuration["MCRUN"]
        i = 0
        for val in mcrun_lst:
            self.ui.cbxMcrun.addItem(val)
            if val == selected_val:
                self.ui.cbxMcrun.setCurrentIndex(i)
            i += 1
        self.ui.cbxMcrun.conf_var = "MCRUN"
        self.ui.cbxMcrun.conf_org_value = mccode_config.configuration["MCRUN"]
        self.ui.cbxMcrun.conf_options_lst = mcrun_lst

        # mcplot combobox
        selected_val = mccode_config.configuration["MCPLOT"]
        i = 0
        for val in mcplot_lst:
            self.ui.cbxMcPlot.addItem(val)
            if val == selected_val:
                self.ui.cbxMcPlot.setCurrentIndex(i)
            i += 1
        self.ui.cbxMcPlot.conf_var = "MCPLOT"
        self.ui.cbxMcPlot.conf_org_value = mccode_config.configuration["MCPLOT"]
        self.ui.cbxMcPlot.conf_options_lst = mcplot_lst

        # mcdisplay combobox
        selected_val = mccode_config.configuration["MCDISPLAY"]
        i = 0
        for val in mcdisplay_lst:
            self.ui.cbxMcdisplay.addItem(val)
            if val == selected_val:
                self.ui.cbxMcdisplay.setCurrentIndex(i)
            i += 1
        self.ui.cbxMcdisplay.conf_var = "MCDISPLAY"
        self.ui.cbxMcdisplay.conf_org_value = mccode_config.configuration["MCDISPLAY"]
        self.ui.cbxMcdisplay.conf_options_lst = mcdisplay_lst
      
        # line edits
        self.ui.edtCC.setText(mccode_config.compilation["CC"])
        self.ui.edtCC.conf_var = "CC"

        self.ui.edtCflags.setText(mccode_config.compilation["CFLAGS"])
        self.ui.edtCflags.conf_var = "CFLAGS"

        self.ui.edtMpicc.setText(mccode_config.compilation["MPICC"])
        self.ui.edtMpicc.conf_var = "MPICC"

        self.ui.edtMPIrun.setText(mccode_config.compilation["MPIRUN"])
        self.ui.edtMPIrun.conf_var = "MPIRUN"

        self.ui.edtNumNodes.setText(mccode_config.compilation["MPINODES"])
        self.ui.edtNumNodes.conf_var = "MPINODES"

        self.ui.edtNumCols.setText(mccode_config.configuration["GUICOLS"])
        self.ui.edtNumCols.conf_var = "GUICOLS"

        self.ui.edtFontSize.setText(mccode_config.configuration["GUIFONTSIZE"])
        self.ui.edtFontSize.conf_var = "GUIFONTSIZE"

        self.ui.edtNDbufsiz.setText(mccode_config.configuration["NDBUFFERSIZE"])
        self.ui.edtNDbufsiz.conf_var = "NDBUFFERSIZE"
        
        self.ui.editor.setText(mccode_config.configuration["EDITOR"])
        self.ui.editor.conf_var = "EDITOR"


    def __pullValuesTo_mccode_config(self):
        # mcrun combobox
        i = self.ui.cbxMcrun.currentIndex()
        mccode_config.configuration["MCRUN"] = self.ui.cbxMcrun.conf_options_lst[i]

        # mcplot combobox
        i = self.ui.cbxMcPlot.currentIndex()
        mccode_config.configuration["MCPLOT"] = self.ui.cbxMcPlot.conf_options_lst[i]

        # mcdisplay combobox
        i = self.ui.cbxMcdisplay.currentIndex()
        mccode_config.configuration["MCDISPLAY"] = self.ui.cbxMcdisplay.conf_options_lst[i]

        # line edits
        mccode_config.compilation[str(self.ui.edtCC.conf_var)] = str(self.ui.edtCC.text())
        mccode_config.compilation[str(self.ui.edtCflags.conf_var)] = str(self.ui.edtCflags.text())
        mccode_config.compilation[str(self.ui.edtMpicc.conf_var)] = str(self.ui.edtMpicc.text())
        mccode_config.compilation[str(self.ui.edtMPIrun.conf_var)] = str(self.ui.edtMPIrun.text())
        mccode_config.compilation[str(self.ui.edtNumNodes.conf_var)] = str(self.ui.edtNumNodes.text())
        mccode_config.configuration[str(self.ui.edtNumCols.conf_var)] = str(self.ui.edtNumCols.text())
        mccode_config.configuration[str(self.ui.edtNDbufsiz.conf_var)] = str(self.ui.edtNDbufsiz.text())
        mccode_config.configuration[str(self.ui.edtFontSize.conf_var)] = str(self.ui.edtFontSize.text())
        mccode_config.configuration[str(self.ui.editor.conf_var)] = str(self.ui.editor.text())
        # Export selected variables to the system / mcrun
        target_mccode=mccode_config.configuration["MCCODE"].upper()
        # CFLAGS and CC:
        os.environ[target_mccode + '_CFLAGS_OVERRIDE']=mccode_config.compilation[str(self.ui.edtCflags.conf_var)]
        os.environ[target_mccode + '_CC_OVERRIDE']=mccode_config.compilation[str(self.ui.edtCC.conf_var)]
        # MPIRUN and MPICC:
        os.environ[target_mccode + '_MPICC_OVERRIDE']=mccode_config.compilation[str(self.ui.edtMpicc.conf_var)]
        os.environ[target_mccode + '_MPIRUN_OVERRIDE']=mccode_config.compilation[str(self.ui.edtMPIrun.conf_var)]
    
    def accept(self):
        self.__pullValuesTo_mccode_config()

        # finally
        super(McConfigDialog, self).accept()

    def save(self):
        self.__pullValuesTo_mccode_config()
        mccode_config.save_user_config()

    def saveedit(self):
        self.__pullValuesTo_mccode_config()
        mccode_config.save_user_config(Edit=True)

        # finally
        super(McConfigDialog, self).accept()
