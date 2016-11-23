
-- put bin/ and lualibs/ first to avoid conflicts with included modules
-- that may have other versions present somewhere else in path/cpath.
local function isproc()
  local file = io.open("/proc")
  if file then file:close() end
  return file ~= nil
end
local iswindows = os.getenv('WINDIR') or (os.getenv('OS') or ''):match('[Ww]indows')
local islinux = not iswindows and isproc()
local arch = "x86" -- use 32bit by default
local unpack = table.unpack or unpack

if islinux then
  local file = io.popen("uname -m")
  if file then
    local machine=file:read("*l")
    local archtype= { x86_64="x64", armv7l="armhf" }
    arch = archtype[machine] or "x86"
    file:close()
  end
end

local clibsdir = "clibs"
if _VERSION == "Lua 5.1" then clibsdir = "clibs51" end
if _VERSION == "Lua 5.2" then clibsdir = "clibs52" end
if _VERSION == "Lua 5.3" then clibsdir = "clibs53" end

package.cpath = (
  iswindows and ('lib/windows/clibs52/?.dll;'):format(clibsdir) or
  islinux and ('lib/linux/%s/%s/lib?.so;'):format(arch, clibsdir) or
  --[[isosx]] ('lib/mac/%s/lib?.dylib;'):format(clibsdir))
    .. package.cpath
--package.path  = 'lualibs/?.lua;lualibs/?/?.lua;lualibs/?/init.lua;lualibs/?/?/?.lua;lualibs/?/?/init.lua;'
--              .. package.path

require("wx")
require("rfsm")
require ("rfsm2uml")


count=0
tmpdir="/tmp/"
filenameTemp="displayer"
filenameTemp=tmpdir .. filenameTemp
loaded=false
listIndexQ=0
listIndexLog=0
scale=1.0
imgSize = {w=0, h=0}
imgPos={x=0,y=0}



-- per ora non la uso ma puo' tornare comodo
listImageList = wx.wxImageList(16, 16)
listImageList:Add(wx.wxArtProvider.GetBitmap(wx.wxART_CROSS_MARK, wx.wxART_TOOLBAR, wx.wxSize(16, 16)))
listImageList:Add(wx.wxArtProvider.GetBitmap(wx.wxART_TICK_MARK, wx.wxART_TOOLBAR, wx.wxSize(16, 16)))


mainFrame = wx.wxFrame()
panelView=wx.wxPanel()
panelCtrl=wx.wxPanel()
choiceSE=wx.wxChoice()
listCtrl_Q=wx.wxListCtrl()
listCtrl_Log=wx.wxListCtrl()


--loading image
Image=wx.wxBitmap()

function populateChoice ()
     local known_events = {
      e_init_fsm=true,
   }

   rfsm.mapfsm(function(t)
		  local events = t.events or {}
		  for _,e in ipairs(events) do
		     known_events[e] = true 
          --print("Event: ",e)
		  end
	       end, fsm, rfsm.is_trans)
     
    local a = {}
    for k, v in pairs(known_events) do
        if string.find(k, "e_done@") == nil then 
            table.insert(a, k)
        end    
    end
 
    table.sort(a)
    for k, v in pairs(a) do
      choiceSE:Append(v)
    end
    choiceSE:SetSelection(0)
end

function getObjectAs(obj_name, obj_type)
  
  obj=mainFrame:FindWindow(xmlResource.GetXRCID(obj_name))
    if not obj then
        wx.wxMessageBox("Unable to find window "..obj_name.." in the dialog",
                        "rFSMSimulator",
                        wx.wxOK + wx.wxICON_EXCLAMATION,
                        wx.NULL)
        return nil
    end
    obj=obj:DynamicCast(obj_type)
    if not obj then
        wx.wxMessageBox("window "..obj_name.." is not a "..obj_type.." or is not derived from it",
                        "rFSMSimulator",
                        wx.wxOK + wx.wxICON_EXCLAMATION,
                        wx.NULL)
        return nil
        
    end
    return obj
end

function load()
    local dlg = wx.wxFileDialog(mainFrame, "Choose a rfsm file",
                                wx.wxGetCwd(), "", "All Files (*)|*|rFSM File (*.lua)|*.lua",
                                wx.wxFD_OPEN + wx.wxFD_FILE_MUST_EXIST + wx.wxFD_CHANGE_DIR )
    if dlg:ShowModal() == wx.wxID_OK then
        local filepath = dlg:GetPath()
        fileName = dlg:GetFilename()
        fsmMod=rfsm.load(filepath)
--TODO gestire che cosa accade se non si carica
        if not rfsm.is_state(fsmMod) then
            wx.wxMessageBox(string.format("Unable to load %s: Unsupported format?", filepath),
                            "rFSMSimulator",
                            wx.wxICON_ERROR + wx.wxOK)
            loaded=false
           listCtrl_Log:InsertItem(listIndexLog,"",0)
           listCtrl_Log:SetItem(listIndexLog, 0, os.date("%X"))
           listCtrl_Log:SetItem(listIndexLog, 1, "Error in "..fileName.." loading")
           listIndexLog=listIndexLog+1 
        else
           fsm=rfsm.init(fsmMod)
           wx.wxMessageBox(string.format("Fsm loaded correctly", filepath),
                            "rFSMSimulator",
                            wx.wxICON_INFORMATION + wx.wxOK)
           mainFrame:SetStatusText("Welcome to rFSMDisplayer. Displaying ".. fileName)
           loaded=true
           populateChoice()
           listCtrl_Log:InsertItem(listIndexLog,"",1)
           listCtrl_Log:SetItem(listIndexLog, 0, os.date("%X"))
           listCtrl_Log:SetItem(listIndexLog, 1, "Loaded "..fileName.." correctly")
           listIndexLog=listIndexLog+1 
           --[[RunButton:Enable(true)
           StepButton:Enable(true)
           sendEventButton:Enable(true)
           showeqButton:Enable(true)]]
           rfsm2uml.rfsm2dot(fsm, filenameTemp.. count .. ".dot")
           --900x1500
           os.execute("dot".." -Gsize=9,9\\! -Gdpi=600 -Tpng -o "..filenameTemp..count..".png "..filenameTemp..count..".dot")
           if Image:LoadFile(filenameTemp..count..".png",wx.wxBITMAP_TYPE_PNG)==false then
              print("Cannot load image!!")
              exit(0)
           else
              panelView:Update()
              panelView:Refresh()
         --[[else                  
              local dc = wx.wxPaintDC(panelView)
              dc:DrawBitmap(Image,0,0,false);
              dc:delete()]]
         end
         
        end
    end
    dlg:Destroy()
end 


function quit()
  local answer=wx.wxMessageBox("Are you sure to exit ?","rFSMSimulator",
                    wx.wxYES_NO+wx.wxNO_DEFAULT+ wx.wxICON_QUESTION,
                    mainFrame)
  if answer==wx.wxYES then
    mainFrame:Destroy()
  end
end
function about()
    wx.wxMessageBox('This is the "About" dialog of the rFSMSimulator.\n Author Nicolo Genesio\n'..
                    wxlua.wxLUA_VERSION_STRING.." built with "..wx.wxVERSION_STRING,
                    "About wxLua rFSMSimulator",
                    wx.wxOK + wx.wxICON_INFORMATION,
                    mainFrame)
end

function showeq()
   rfsm.check_events(fsm)
   local q=table.concat(utils.map(tostring, fsm._intq), ', ')
   return q
end

function updateEventList() 
    listCtrl_Q:DeleteAllItems()
    local listIndexQ = 0
    local strq = showeq()
    for word in string.gmatch(strq, '([^,]+)') do 
        local event = word:gsub("%s+", "")
        local time = os.date("%X")
        listCtrl_Q:InsertItem(listIndexQ,"",1)
        listCtrl_Q:SetItem(listIndexQ, 0, time)
        listCtrl_Q:SetItem(listIndexQ, 1, event)
        listIndexQ=listIndexQ+1
    end 
end


function runAndDisplay()
  if loaded then
    listCtrl_Log:InsertItem(listIndexLog,"",1)
    listCtrl_Log:SetItem(listIndexLog, 0, os.date("%X"))
    listCtrl_Log:SetItem(listIndexLog, 1, "Run button pressed")
    listIndexLog=listIndexLog+1     
    rfsm.run(fsm)
    updateEventList()    
    rfsm2uml.rfsm2dot(fsm, filenameTemp.. count .. ".dot")
    os.execute("dot".." -Gsize=9,9\\! -Gdpi=600 -Tpng -o "..filenameTemp..count..".png "..filenameTemp..count..".dot")
    if Image:LoadFile(filenameTemp..count..".png",wx.wxBITMAP_TYPE_PNG)==false then
      print("Cannot load image!!")
      exit(0)
    else
      local dc = wx.wxPaintDC(panelView)
      dc:SetBrush(wx.wxBrush(wx.wxWHITE,0))
      dc:SetPen(wx.wxPen(wx.wxWHITE,1,0))
      dc:DrawRectangle(0,0,1920,1080)
      dc:DrawBitmap(Image,0,0,false);
      dc:delete()
    end      
    count=count+1
  else
    wx.wxMessageBox(string.format("Please load an rFSM file"),
                            "rFSMSimulator",
                            wx.wxICON_ERROR + wx.wxOK)
    listCtrl_Log:InsertItem(listIndexLog,"",0)
    listCtrl_Log:SetItem(listIndexLog, 0, os.date("%X"))
    listCtrl_Log:SetItem(listIndexLog, 1, "Missing rfsm...")
    listIndexLog=listIndexLog+1 
  end
end
  
function stepAndDisplay()
  if loaded then
    listCtrl_Log:InsertItem(listIndexLog,"",1)
    listCtrl_Log:SetItem(listIndexLog, 0, os.date("%X"))
    listCtrl_Log:SetItem(listIndexLog, 1, "Step button pressed")
    listIndexLog=listIndexLog+1 
    rfsm.step(fsm)
    updateEventList()
    rfsm2uml.rfsm2dot(fsm, filenameTemp.. count .. ".dot")
    os.execute("dot".." -Gsize=9,9\\! -Gdpi=600 -Tpng -o "..filenameTemp..count..".png "..filenameTemp..count..".dot")
    if Image:LoadFile(filenameTemp..count..".png",wx.wxBITMAP_TYPE_PNG)==false then
      print("Cannot load image!!")
      exit(0)
    else
      local dc = wx.wxPaintDC(panelView)
      dc:SetBrush(wx.wxBrush(wx.wxWHITE,0))
      dc:SetPen(wx.wxPen(wx.wxWHITE,1,0))
      dc:DrawRectangle(0,0,1920,1080)
      dc:DrawBitmap(Image,0,0,false);
      dc:delete()
    end  
    count=count+1
  else
    wx.wxMessageBox(string.format("Please load an rFSM file"),
                            "rFSMSimulator",
                            wx.wxICON_ERROR + wx.wxOK)
    listCtrl_Log:InsertItem(listIndexLog,"",0)
    listCtrl_Log:SetItem(listIndexLog, 0, os.date("%X"))
    listCtrl_Log:SetItem(listIndexLog, 1, "Missing rfsm...")
    listIndexLog=listIndexLog+1                          
  end
end

function sendEvent()
  if loaded then
    textToSend=choiceSE:GetString(choiceSE:GetSelection())
    rfsm.send_events(fsm, textToSend)
    --print("Event sent:".. textToSend)
    
    listCtrl_Log:InsertItem(listIndexLog,"",1)
    listCtrl_Log:SetItem(listIndexLog, 0, os.date("%X"))
    listCtrl_Log:SetItem(listIndexLog, 1, "Event sent: "..textToSend)
    listIndexLog=listIndexLog+1 

    -- event queue 
    updateEventList() 

  else
    wx.wxMessageBox(string.format("Please load an rFSM file"),
                            "rFSMSimulator",
                            wx.wxICON_ERROR + wx.wxOK)
    listCtrl_Log:InsertItem(listIndexLog,"",0)
    listCtrl_Log:SetItem(listIndexLog, 0, os.date("%X"))
    listCtrl_Log:SetItem(listIndexLog, 1, "Impossible to send the event, missing rfsm...")
    listIndexLog=listIndexLog+1
                                
  end
end

function onKeyDown(event)
  local code=event:GetKeyCode()
  if code == wx.WXK_RETURN then
      sendEvent()
    end
  end


function OnMouseMiddleWheel(event)
  if loaded then
    if event:GetWheelRotation()>0 then
      scale=scale+0.2
    else
      scale=scale-0.2
    end
  end
  panelView:Refresh()
  panelView:Update()
  
  
end
function OnMouseMiddleDown(event)
  mouse={x=event:GetX(),y=event:GetY()}
  panelView:SetCursor(wx.wxCursor(wx.wxCURSOR_HAND))
end
function OnMouseMiddleUp(event) 
  mouse =nil 
  panelView:SetCursor(wx.wxSTANDARD_CURSOR)
  end
function OnMouseMotion(event)
  if mouse==nil or not loaded then return end
  if event:Dragging() then
    if panelView:GetSize():GetWidth() < Image:GetWidth()*scale or panelView:GetSize():GetHeight() < Image:GetHeight()*scale then
      dx=event:GetX()-mouse.x
      dy=event:GetY()-mouse.y
    else
      dx=0
      dy=0
    end
    
    if imgPos.x < panelView:GetSize():GetWidth()-dx-20 and  imgPos.x > -(Image:GetWidth()+dx-20) then
      imgPos.x=imgPos.x+dx
    end
    
    if imgPos.y < panelView:GetSize():GetHeight()-dy-20 and  imgPos.y > -(Image:GetHeight()+dy-20) then
      imgPos.y=imgPos.y+dy
    end
    
    --Image:GetWidth(),Image:GetHeight()
    panelView:Refresh()
    panelView:Update()
    mouse.x=event:GetX()
    mouse.y=event:GetY()
  end
end
function OnPaint(event)
    -- must always create a wxPaintDC in a wxEVT_PAINT handler
    local dc = wx.wxPaintDC(panelView)
    dc:SetUserScale(scale,scale)
    --[[dc:SetBrush(wx.wxBrush(wx.wxWHITE,0))
    dc:SetPen(wx.wxPen(wx.wxWHITE,0))
    dc:DrawRectangle(0,0,1920,1080)]]
    -- call drawing functions

    if loaded then
      --if imgSize.w<Image:GetWidth()*scale then imgSize.w=Image:GetWidth()*scale end
      --if imgSize.h<Image:GetHeight()*scale then imgSize.h=Image:GetHeight()*scale end      
      dc:SetBrush(wx.wxBrush(wx.wxWHITE,0))
      dc:SetPen(wx.wxPen(wx.wxWHITE,1,0))
      dc:Clear()
      dc:DrawBitmap(Image,imgPos.x,imgPos.y,false)
      dc:SetBrush(wx.wxBrush(wx.wxWHITE,wx.wxTRANSPARENT))
      dc:SetPen(wx.wxBLACK_DASHED_PEN)
      dc:DrawRectangle(imgPos.x,imgPos.y,Image:GetWidth(),Image:GetHeight())

              
      
    end
    -- the paint DC will be automatically destroyed by the garbage collector,
    -- however on Windows 9x/Me this may be too late (DC's are precious resource)
    -- so delete it here
    dc:delete() -- ALWAYS delete() any wxDCs created when done
end




-- ---------------------------------------------------------------------------
-- The main program as a function (makes it easy to exit on error)
function main()
    -- xml style resources (if present)
    xmlResource = wx.wxXmlResource()
    xmlResource:InitAllHandlers()
    local xrcFilename = "src/res/ui/rFSMSimulator.xrc"

    local logNo = wx.wxLogNull() -- silence wxXmlResource error msg since we provide them

    -- try to load the resource and ask for path to it if not found
    while not xmlResource:Load(xrcFilename) do
        -- must unload the file before we try again
        xmlResource:Unload(xrcFilename)

        wx.wxMessageBox("Error loading xrc resources, please choose the path to 'rFSMSimulator.xrc'.",
                        "rFSMSimulator",
                        wx.wxOK + wx.wxICON_EXCLAMATION,
                        wx.NULL)
        local fileDialog = wx.wxFileDialog(wx.NULL,
                                           "Open 'rFSMSimulator.xrc' resource file",
                                           "",
                                           "rFSMSimulator.xrc",
                                           "XRC files (*.xrc)|*.xrc|All files (*)|*",
                                            wx.wxFD_OPEN + wx.wxFD_FILE_MUST_EXIST)

        if fileDialog:ShowModal() == wx.wxID_OK then
            xrcFilename = fileDialog:GetPath()
        else
            return -- quit program
        end
    end

    logNo:delete() -- turn error messages back on
    
    
    if not xmlResource:LoadFrame(mainFrame, wx.NULL, "MainFrame") then
        wx.wxMessageBox("Error loading xrc resources!",
                        "rFSMSimulator",
                        wx.wxOK + wx.wxICON_EXCLAMATION,
                        wx.NULL)
        return -- quit program
    end
    
    panelView=getObjectAs("m_panelView","wxPanel")
    choiceSE=getObjectAs("m_choiceSE","wxChoice")
    listCtrl_Q=getObjectAs("m_listCtrlQueue","wxListCtrl")
    listCtrl_Log=getObjectAs("m_listCtrlLog","wxListCtrl")
    btn_SE=getObjectAs("m_buttonSE","wxButton")
    btn_SE:SetDefault()
    
    listCtrl_Q:InsertColumn(0, "Time(hh:mm:ss)",wx.wxLIST_FORMAT_LEFT,150)
    listCtrl_Q:InsertColumn(1, "Queue",wx.wxLIST_FORMAT_LEFT,350)
    listCtrl_Q:SetImageList(listImageList, wx.wxIMAGE_LIST_SMALL)
    
    listCtrl_Log:InsertColumn(0, "Time(hh:mm:ss)",wx.wxLIST_FORMAT_LEFT,150)
    listCtrl_Log:InsertColumn(1, "Event",wx.wxLIST_FORMAT_LEFT,350)
    listCtrl_Log:SetImageList(listImageList, wx.wxIMAGE_LIST_SMALL)
    
    splitter=getObjectAs("m_splitterMain","wxSplitterWindow")
    splitter:SetSashPosition(mainFrame:GetSize():GetWidth()*70/100)

  
    
         -- init global wxWindow ID values
    ID_MAINFRAME= xmlResource.GetXRCID("MainFrame")
    --menubar ids
    ID_MENU     = xmlResource.GetXRCID("m_menubarMain")
    ID_LOAD     = xmlResource.GetXRCID("m_menuItemOpen")
    ID_EXIT     = xmlResource.GetXRCID("m_menuItemQuit")
    --toolbar ids
    ID_TOOLBAR  = xmlResource.GetXRCID("m_toolBar")
    ID_TOOLOPEN = xmlResource.GetXRCID("m_toolOpen")
    ID_TOOLRUN  = xmlResource.GetXRCID("m_toolRun")
    ID_TOOLSTEP = xmlResource.GetXRCID("m_toolStep")
    ID_TOOLHELP = xmlResource.GetXRCID("m_toolHelp")
    ID_TOOLQUIT = xmlResource.GetXRCID("m_toolQuit")
    --splitter ids
    ID_SPLITTER = xmlResource.GetXRCID("m_splitterMain")
    ID_PANELV   = xmlResource.GetXRCID("m_panelView")
    ID_PANELC   = xmlResource.GetXRCID("m_panelCtrl")
    --sizerCtrl ids
    ID_SEND     = xmlResource.GetXRCID("m_buttonSE")
    ID_LISTQ    = xmlResource.GetXRCID("m_listCtrlQueue")
    --sizerLog id
    ID_LISTL    = xmlResource.GetXRCID("m_listCtrlLog")
    --status bar id
    ID_STATUS    = xmlResource.GetXRCID("m_statusBar")


    --events
    
    panelView:Connect(wx.wxEVT_PAINT, OnPaint)
    panelView:Connect(wx.wxEVT_MOUSEWHEEL, OnMouseMiddleWheel)
    panelView:Connect(wx.wxEVT_MIDDLE_DOWN, OnMouseMiddleDown)
    panelView:Connect(wx.wxEVT_MIDDLE_UP, OnMouseMiddleUp)
    panelView:Connect(wx.wxEVT_MOTION, OnMouseMotion)

    mainFrame:Connect(ID_LOAD, wx.wxEVT_COMMAND_MENU_SELECTED, load)

    mainFrame:Connect(ID_EXIT, wx.wxEVT_COMMAND_MENU_SELECTED,quit)


    mainFrame:Connect(ID_TOOLHELP, wx.wxEVT_COMMAND_TOOL_CLICKED,about)
    mainFrame:Connect(ID_TOOLQUIT, wx.wxEVT_COMMAND_TOOL_CLICKED,quit)
    mainFrame:Connect(ID_TOOLOPEN, wx.wxEVT_COMMAND_TOOL_CLICKED,load)
    mainFrame:Connect(ID_TOOLRUN, wx.wxEVT_COMMAND_TOOL_CLICKED,runAndDisplay)
    mainFrame:Connect(ID_TOOLSTEP,wx.wxEVT_COMMAND_TOOL_CLICKED,stepAndDisplay)
    mainFrame:Connect(ID_SEND,wx.wxEVT_COMMAND_BUTTON_CLICKED,sendEvent)
    mainFrame:Connect(wx.wxEVT_CLOSE_WINDOW,quit)
    panelCtrl:Connect(wx.wxEVT_KEY_DOWN,onKeyDown)

    -- finally, show the frame window
  mainFrame:Show(true)
  --necessary otherwise it will close immediately
  wx.wxGetApp():MainLoop()
end

main()
