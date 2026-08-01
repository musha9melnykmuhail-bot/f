-- Защита от повторного запуска
if getgenv().ThoriumLib_Loaded or game:GetService("CoreGui"):FindFirstChild("ThoriumLibGui") then
    warn("[ThoriumLib] Скрипт уже запущен!")
    return
end
getgenv().ThoriumLib_Loaded = true

-- Проверка Place ID (Brookhaven RP: 4924922222)
local TARGET_PLACE_ID = 4924922222
if game.PlaceId ~= TARGET_PLACE_ID then
    warn("[ThoriumLib] Неверный Place ID (" .. tostring(game.PlaceId) .. ")! Ожидался Brookhaven RP (" .. tostring(TARGET_PLACE_ID) .. ")")
    return
end

local Players = game:GetService("Players")
local TweenService = game:GetService("TweenService")
local UserInputService = game:GetService("UserInputService")
local Lighting = game:GetService("Lighting")
local Workspace = game:GetService("Workspace")
local RunService = game:GetService("RunService")
local TeleportService = game:GetService("TeleportService")
local HttpService = game:GetService("HttpService")
local VirtualUser = game:GetService("VirtualUser")

local LocalPlayer = Players.LocalPlayer
local Mouse = LocalPlayer:GetMouse()

-- Таблицы для управления соединениями и циклами (для Unload)
local activeConnections = {}
local activeLoops = {}

local function AddConnection(conn)
    table.insert(activeConnections, conn)
    return conn
end

local ScreenGui = Instance.new("ScreenGui")
ScreenGui.Name = "ThoriumLibGui"
ScreenGui.ResetOnSpawn = false

local success, _ = pcall(function()
    ScreenGui.Parent = game:GetService("CoreGui")
end)
if not success then
    ScreenGui.Parent = LocalPlayer:WaitForChild("PlayerGui")
end

local UIBlur = Lighting:FindFirstChild("ThoriumUIBlur") or Instance.new("BlurEffect")
UIBlur.Name = "ThoriumUIBlur"
UIBlur.Size = 14
UIBlur.Enabled = true
UIBlur.Parent = Lighting

local MainFrame = Instance.new("Frame")
MainFrame.Name = "MainFrame"
MainFrame.Size = UDim2.new(0, 740, 0, 460)
MainFrame.Position = UDim2.new(0.5, -370, 0.5, -230)
MainFrame.BackgroundColor3 = Color3.fromRGB(16, 16, 22)
MainFrame.BackgroundTransparency = 0.25
MainFrame.BorderSizePixel = 0
MainFrame.Active = true
MainFrame.Visible = false
MainFrame.Parent = ScreenGui

local MainCorner = Instance.new("UICorner")
MainCorner.CornerRadius = UDim.new(0, 12)
MainCorner.Parent = MainFrame

local MainStroke = Instance.new("UIStroke")
MainStroke.Color = Color3.fromRGB(70, 70, 95)
MainStroke.Thickness = 1
MainStroke.Transparency = 0.4
MainStroke.Parent = MainFrame

local MobileToggleBtn = Instance.new("TextButton")
MobileToggleBtn.Name = "MobileToggleBtn"
MobileToggleBtn.Size = UDim2.new(0, 46, 0, 46)
MobileToggleBtn.Position = UDim2.new(0, 18, 0.35, 0)
MobileToggleBtn.BackgroundColor3 = Color3.fromRGB(108, 92, 231)
MobileToggleBtn.BackgroundTransparency = 0.2
MobileToggleBtn.Text = "TH"
MobileToggleBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
MobileToggleBtn.TextSize = 15
MobileToggleBtn.Font = Enum.Font.GothamBold
MobileToggleBtn.Active = true
MobileToggleBtn.Visible = false
MobileToggleBtn.Parent = ScreenGui

local MobileBtnCorner = Instance.new("UICorner")
MobileBtnCorner.CornerRadius = UDim.new(1, 0)
MobileBtnCorner.Parent = MobileToggleBtn

local MobileBtnStroke = Instance.new("UIStroke")
MobileBtnStroke.Color = Color3.fromRGB(180, 170, 255)
MobileBtnStroke.Thickness = 1.5
MobileBtnStroke.Parent = MobileToggleBtn

local SplashFrame = Instance.new("Frame")
SplashFrame.Name = "ThoriumSplash"
SplashFrame.Size = UDim2.new(1, 0, 1, 0)
SplashFrame.Position = UDim2.new(0, 0, 0, 0)
SplashFrame.BackgroundColor3 = Color3.fromRGB(12, 12, 18)
SplashFrame.BackgroundTransparency = 0
SplashFrame.BorderSizePixel = 0
SplashFrame.ZIndex = 100
SplashFrame.Parent = ScreenGui

local SplashTitle = Instance.new("TextLabel")
SplashTitle.Name = "SplashTitle"
SplashTitle.Size = UDim2.new(0, 500, 0, 80)
SplashTitle.Position = UDim2.new(0.5, -250, 0.5, -40)
SplashTitle.BackgroundTransparency = 1
SplashTitle.Text = "THORIUM<font color='#6C5CE7'>LIB</font>"
SplashTitle.RichText = true
SplashTitle.TextColor3 = Color3.fromRGB(255, 255, 255)
SplashTitle.TextTransparency = 1
SplashTitle.TextSize = 42
SplashTitle.Font = Enum.Font.GothamBold
SplashTitle.ZIndex = 101
SplashTitle.Parent = SplashFrame

local SplashSub = Instance.new("TextLabel")
SplashSub.Name = "SplashSub"
SplashSub.Size = UDim2.new(0, 500, 0, 30)
SplashSub.Position = UDim2.new(0.5, -250, 0.5, 35)
SplashSub.BackgroundTransparency = 1
SplashSub.Text = "Brookhaven RP Loaded"
SplashSub.TextColor3 = Color3.fromRGB(160, 160, 180)
SplashSub.TextTransparency = 1
SplashSub.TextSize = 13
SplashSub.Font = Enum.Font.GothamMedium
SplashSub.ZIndex = 101
SplashSub.Parent = SplashFrame

task.spawn(function()
    TweenService:Create(SplashTitle, TweenInfo.new(1, Enum.EasingStyle.Quart, Enum.EasingDirection.Out), {TextTransparency = 0}):Play()
    TweenService:Create(SplashSub, TweenInfo.new(1, Enum.EasingStyle.Quart, Enum.EasingDirection.Out), {TextTransparency = 0.3}):Play()
    task.wait(1.8)
    TweenService:Create(SplashTitle, TweenInfo.new(0.8, Enum.EasingStyle.Quart, Enum.EasingDirection.In), {TextTransparency = 1}):Play()
    TweenService:Create(SplashSub, TweenInfo.new(0.8, Enum.EasingStyle.Quart, Enum.EasingDirection.In), {TextTransparency = 1}):Play()
    local splashFade = TweenService:Create(SplashFrame, TweenInfo.new(0.8, Enum.EasingStyle.Quart, Enum.EasingDirection.In), {BackgroundTransparency = 1})
    splashFade:Play()
    splashFade.Completed:Wait()
    SplashFrame:Destroy()
    MainFrame.Visible = true
    MobileToggleBtn.Visible = true
end)

local function ToggleMenu()
    MainFrame.Visible = not MainFrame.Visible
    UIBlur.Enabled = MainFrame.Visible
end

MobileToggleBtn.MouseButton1Click:Connect(ToggleMenu)

local mDragging, mDragStart, mStartPos
AddConnection(MobileToggleBtn.InputBegan:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
        mDragging = true
        mDragStart = input.Position
        mStartPos = MobileToggleBtn.Position
        input.Changed:Connect(function()
            if input.UserInputState == Enum.UserInputState.End then
                mDragging = false
            end
        end)
    end
end))

AddConnection(UserInputService.InputChanged:Connect(function(input)
    if mDragging and (input.UserInputType == Enum.UserInputType.MouseMovement or input.UserInputType == Enum.UserInputType.Touch) then
        local delta = input.Position - mDragStart
        MobileToggleBtn.Position = UDim2.new(mStartPos.X.Scale, mStartPos.X.Offset + delta.X, mStartPos.Y.Scale, mStartPos.Y.Offset + delta.Y)
    end
end))

local dragging, dragInput, dragStart, startPos
local function updateDrag(input)
    local delta = input.Position - dragStart
    MainFrame.Position = UDim2.new(startPos.X.Scale, startPos.X.Offset + delta.X, startPos.Y.Scale, startPos.Y.Offset + delta.Y)
end

AddConnection(MainFrame.InputBegan:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
        dragging = true
        dragStart = input.Position
        startPos = MainFrame.Position
        input.Changed:Connect(function()
            if input.UserInputState == Enum.UserInputState.End then
                dragging = false
            end
        end)
    end
end))

AddConnection(MainFrame.InputChanged:Connect(function(input)
    if input.UserInputType == Enum.UserInputType.MouseMovement or input.UserInputType == Enum.UserInputType.Touch then
        dragInput = input
    end
end))

AddConnection(UserInputService.InputChanged:Connect(function(input)
    if input == dragInput and dragging then
        updateDrag(input)
    end
end))

local toggleKey = Enum.KeyCode.K
local isListeningForKey = false

AddConnection(UserInputService.InputBegan:Connect(function(input, gameProcessed)
    if isListeningForKey and input.UserInputType == Enum.UserInputType.Keyboard then
        toggleKey = input.KeyCode
        isListeningForKey = false
        return
    end

    if not gameProcessed and input.UserInputType == Enum.UserInputType.Keyboard and input.KeyCode == toggleKey then
        ToggleMenu()
    end
end))

local Sidebar = Instance.new("Frame")
Sidebar.Name = "Sidebar"
Sidebar.Size = UDim2.new(0, 190, 1, 0)
Sidebar.BackgroundColor3 = Color3.fromRGB(12, 12, 18)
Sidebar.BackgroundTransparency = 0.45
Sidebar.BorderSizePixel = 0
Sidebar.Parent = MainFrame

local SidebarCorner = Instance.new("UICorner")
SidebarCorner.CornerRadius = UDim.new(0, 12)
SidebarCorner.Parent = Sidebar

local SidebarRightLine = Instance.new("Frame")
SidebarRightLine.Size = UDim2.new(0, 1, 1, -20)
SidebarRightLine.Position = UDim2.new(1, -1, 0, 10)
SidebarRightLine.BackgroundColor3 = Color3.fromRGB(50, 50, 68)
SidebarRightLine.BackgroundTransparency = 0.5
SidebarRightLine.BorderSizePixel = 0
SidebarRightLine.Parent = Sidebar

local Title = Instance.new("TextLabel")
Title.Name = "Title"
Title.Size = UDim2.new(1, 0, 0, 50)
Title.BackgroundTransparency = 1
Title.Text = "Thoriumlib <font color='#6C5CE7'>v1.8</font>"
Title.RichText = true
Title.TextColor3 = Color3.fromRGB(255, 255, 255)
Title.TextSize = 18
Title.Font = Enum.Font.GothamBold
Title.Parent = Sidebar

local TabContainer = Instance.new("Frame")
TabContainer.Name = "TabContainer"
TabContainer.Size = UDim2.new(1, -16, 1, -130)
TabContainer.Position = UDim2.new(0, 8, 0, 55)
TabContainer.BackgroundTransparency = 1
TabContainer.Parent = Sidebar

local TabListLayout = Instance.new("UIListLayout")
TabListLayout.SortOrder = Enum.SortOrder.LayoutOrder
TabListLayout.Padding = UDim.new(0, 6)
TabListLayout.Parent = TabContainer

local ContentArea = Instance.new("Frame")
ContentArea.Name = "ContentArea"
ContentArea.Size = UDim2.new(1, -215, 1, -20)
ContentArea.Position = UDim2.new(0, 205, 0, 10)
ContentArea.BackgroundTransparency = 1
ContentArea.Parent = MainFrame

local ProfileBox = Instance.new("Frame")
ProfileBox.Name = "ProfileBox"
ProfileBox.Size = UDim2.new(1, -16, 0, 50)
ProfileBox.Position = UDim2.new(0, 8, 1, -60)
ProfileBox.BackgroundColor3 = Color3.fromRGB(20, 20, 28)
ProfileBox.BackgroundTransparency = 0.4
ProfileBox.BorderSizePixel = 0
ProfileBox.Parent = Sidebar

local ProfileCorner = Instance.new("UICorner")
ProfileCorner.CornerRadius = UDim.new(0, 8)
ProfileCorner.Parent = ProfileBox

local ProfileStroke = Instance.new("UIStroke")
ProfileStroke.Color = Color3.fromRGB(55, 55, 75)
ProfileStroke.Transparency = 0.6
ProfileStroke.Parent = ProfileBox

local ProfileIcon = Instance.new("ImageLabel")
ProfileIcon.Name = "Avatar"
ProfileIcon.Size = UDim2.new(0, 34, 0, 34)
ProfileIcon.Position = UDim2.new(0, 8, 0.5, -17)
ProfileIcon.BackgroundTransparency = 1
ProfileIcon.Image = Players:GetUserThumbnailAsync(LocalPlayer.UserId, Enum.ThumbnailType.HeadShot, Enum.ThumbnailSize.Size420x420)
ProfileIcon.Parent = ProfileBox

local ProfileIconCorner = Instance.new("UICorner")
ProfileIconCorner.CornerRadius = UDim.new(1, 0)
ProfileIconCorner.Parent = ProfileIcon

local NicknameLabel = Instance.new("TextLabel")
NicknameLabel.Name = "Nickname"
NicknameLabel.Size = UDim2.new(1, -50, 0, 16)
NicknameLabel.Position = UDim2.new(0, 48, 0, 8)
NicknameLabel.BackgroundTransparency = 1
NicknameLabel.Text = LocalPlayer.Name
NicknameLabel.TextColor3 = Color3.fromRGB(240, 240, 240)
NicknameLabel.TextSize = 12
NicknameLabel.Font = Enum.Font.GothamSemibold
NicknameLabel.TextXAlignment = Enum.TextXAlignment.Left
NicknameLabel.Parent = ProfileBox

-- Система контроля Trusted Mode
local trustedMode = false
local dangerousElements = {}

local ModeLabel = Instance.new("TextLabel")
ModeLabel.Name = "ModeLabel"
ModeLabel.Size = UDim2.new(1, -50, 0, 14)
ModeLabel.Position = UDim2.new(0, 48, 0, 24)
ModeLabel.BackgroundTransparency = 1
ModeLabel.RichText = true
ModeLabel.Text = "<font color='#FF4D4D'>Unsafe Mode</font>"
ModeLabel.TextColor3 = Color3.fromRGB(255, 255, 255)
ModeLabel.TextSize = 10
ModeLabel.Font = Enum.Font.GothamSemibold
ModeLabel.TextXAlignment = Enum.TextXAlignment.Left
ModeLabel.Parent = ProfileBox

local function UpdateModeStatus(state)
    trustedMode = state
    if state then
        ModeLabel.Text = "<font color='#4EFE8C'>Safe Mode</font>"
        for _, elem in ipairs(dangerousElements) do
            if elem.getState() then
                elem.setState(false)
            end
            elem.setYellow(true)
        end
    else
        ModeLabel.Text = "<font color='#FF4D4D'>Unsafe Mode</font>"
        for _, elem in ipairs(dangerousElements) do
            elem.setYellow(false)
        end
    end
end

local tabs = {}

local function createTab(name, icon)
    local page = Instance.new("Frame")
    page.Name = name .. "Page"
    page.Size = UDim2.new(1, 0, 1, 0)
    page.BackgroundTransparency = 1
    page.Visible = false
    page.Parent = ContentArea

    local centerDivider = Instance.new("Frame")
    centerDivider.Name = "CenterDivider"
    centerDivider.Size = UDim2.new(0, 1, 1, -10)
    centerDivider.Position = UDim2.new(0.5, -0.5, 0, 5)
    centerDivider.BackgroundColor3 = Color3.fromRGB(55, 55, 75)
    centerDivider.BackgroundTransparency = 0.5
    centerDivider.BorderSizePixel = 0
    centerDivider.Parent = page

    local leftCol = Instance.new("ScrollingFrame")
    leftCol.Name = "LeftColumn"
    leftCol.Size = UDim2.new(0.5, -12, 1, 0)
    leftCol.Position = UDim2.new(0, 0, 0, 0)
    leftCol.BackgroundTransparency = 1
    leftCol.BorderSizePixel = 0
    leftCol.ScrollBarThickness = 3
    leftCol.ScrollBarImageColor3 = Color3.fromRGB(108, 92, 231)
    leftCol.Parent = page

    local leftLayout = Instance.new("UIListLayout")
    leftLayout.SortOrder = Enum.SortOrder.LayoutOrder
    leftLayout.Padding = UDim.new(0, 6)
    leftLayout.Parent = leftCol

    leftLayout:GetPropertyChangedSignal("AbsoluteContentSize"):Connect(function()
        leftCol.CanvasSize = UDim2.new(0, 0, 0, leftLayout.AbsoluteContentSize.Y + 10)
    end)

    local rightCol = Instance.new("ScrollingFrame")
    rightCol.Name = "RightColumn"
    rightCol.Size = UDim2.new(0.5, -12, 1, 0)
    rightCol.Position = UDim2.new(0.5, 12, 0, 0)
    rightCol.BackgroundTransparency = 1
    rightCol.BorderSizePixel = 0
    rightCol.ScrollBarThickness = 3
    rightCol.ScrollBarImageColor3 = Color3.fromRGB(108, 92, 231)
    rightCol.Parent = page

    local rightLayout = Instance.new("UIListLayout")
    rightLayout.SortOrder = Enum.SortOrder.LayoutOrder
    rightLayout.Padding = UDim.new(0, 6)
    rightLayout.Parent = rightCol

    rightLayout:GetPropertyChangedSignal("AbsoluteContentSize"):Connect(function()
        rightCol.CanvasSize = UDim2.new(0, 0, 0, rightLayout.AbsoluteContentSize.Y + 10)
    end)

    local tabBtn = Instance.new("TextButton")
    tabBtn.Name = name .. "Tab"
    tabBtn.Size = UDim2.new(1, 0, 0, 34)
    tabBtn.BackgroundColor3 = Color3.fromRGB(24, 24, 32)
    tabBtn.BackgroundTransparency = 0.6
    tabBtn.Text = "  " .. (icon or "•") .. "  " .. name
    tabBtn.TextColor3 = Color3.fromRGB(160, 160, 180)
    tabBtn.TextSize = 12
    tabBtn.Font = Enum.Font.GothamMedium
    tabBtn.TextXAlignment = Enum.TextXAlignment.Left
    tabBtn.Parent = TabContainer

    local btnCorner = Instance.new("UICorner")
    btnCorner.CornerRadius = UDim.new(0, 6)
    btnCorner.Parent = tabBtn

    local activeIndicator = Instance.new("Frame")
    activeIndicator.Name = "Indicator"
    activeIndicator.Size = UDim2.new(0, 3, 0, 18)
    activeIndicator.Position = UDim2.new(0, 2, 0.5, -9)
    activeIndicator.BackgroundColor3 = Color3.fromRGB(108, 92, 231)
    activeIndicator.BorderSizePixel = 0
    activeIndicator.Visible = false
    activeIndicator.Parent = tabBtn

    local indCorner = Instance.new("UICorner")
    indCorner.CornerRadius = UDim.new(1, 0)
    indCorner.Parent = activeIndicator

    local tabObj = {
        Button = tabBtn,
        Page = page,
        LeftCol = leftCol,
        RightCol = rightCol,
        Indicator = activeIndicator,
        ItemCount = 0
    }

    local function selectTab()
        for _, tabData in pairs(tabs) do
            tabData.Page.Visible = false
            tabData.Indicator.Visible = false
            TweenService:Create(tabData.Button, TweenInfo.new(0.2), {
                BackgroundColor3 = Color3.fromRGB(24, 24, 32),
                BackgroundTransparency = 0.6,
                TextColor3 = Color3.fromRGB(160, 160, 180)
            }):Play()
        end
        page.Visible = true
        activeIndicator.Visible = true
        TweenService:Create(tabBtn, TweenInfo.new(0.2), {
            BackgroundColor3 = Color3.fromRGB(108, 92, 231),
            BackgroundTransparency = 0.2,
            TextColor3 = Color3.fromRGB(255, 255, 255)
        }):Play()
    end

    tabBtn.MouseButton1Click:Connect(selectTab)

    function tabObj:GetNextParent()
        self.ItemCount = self.ItemCount + 1
        return (self.ItemCount % 2 == 1) and self.LeftCol or self.RightCol
    end

    function tabObj:AddToggle(text, default, isDangerous, callback)
        if type(isDangerous) == "function" then
            callback = isDangerous
            isDangerous = false
        end

        local targetParent = self:GetNextParent()
        local state = default or false
        
        local toggleFrame = Instance.new("Frame")
        toggleFrame.Size = UDim2.new(1, 0, 0, 38)
        toggleFrame.BackgroundColor3 = Color3.fromRGB(22, 22, 30)
        toggleFrame.BackgroundTransparency = 0.35
        toggleFrame.Parent = targetParent

        local corner = Instance.new("UICorner")
        corner.CornerRadius = UDim.new(0, 6)
        corner.Parent = toggleFrame

        local stroke = Instance.new("UIStroke")
        stroke.Color = Color3.fromRGB(50, 50, 65)
        stroke.Transparency = 0.6
        stroke.Parent = toggleFrame

        local label = Instance.new("TextLabel")
        label.Size = UDim2.new(1, -50, 1, 0)
        label.Position = UDim2.new(0, 10, 0, 0)
        label.BackgroundTransparency = 1
        label.Text = text
        label.TextColor3 = isDangerous and (trustedMode and Color3.fromRGB(255, 215, 0) or Color3.fromRGB(220, 220, 230)) or Color3.fromRGB(220, 220, 230)
        label.TextSize = 11
        label.Font = Enum.Font.GothamMedium
        label.TextXAlignment = Enum.TextXAlignment.Left
        label.TextWrapped = true
        label.Parent = toggleFrame

        local btn = Instance.new("TextButton")
        btn.Size = UDim2.new(0, 34, 0, 18)
        btn.Position = UDim2.new(1, -40, 0.5, -9)
        btn.BackgroundColor3 = state and Color3.fromRGB(108, 92, 231) or Color3.fromRGB(40, 40, 52)
        btn.Text = ""
        btn.Parent = toggleFrame

        local btnCorner = Instance.new("UICorner")
        btnCorner.CornerRadius = UDim.new(1, 0)
        btnCorner.Parent = btn

        local circle = Instance.new("Frame")
        circle.Size = UDim2.new(0, 14, 0, 14)
        circle.Position = state and UDim2.new(1, -16, 0.5, -7) or UDim2.new(0, 2, 0.5, -7)
        circle.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
        circle.Parent = btn

        local circleCorner = Instance.new("UICorner")
        circleCorner.CornerRadius = UDim.new(1, 0)
        circleCorner.Parent = circle

        local function applyToggle(newState)
            if trustedMode and isDangerous and newState == true then
                warn("[ThoriumLib]: Функция '" .. text .. "' заблокирована в Trusted Mode!")
                return
            end

            state = newState
            TweenService:Create(btn, TweenInfo.new(0.2), {
                BackgroundColor3 = state and Color3.fromRGB(108, 92, 231) or Color3.fromRGB(40, 40, 52)
            }):Play()
            TweenService:Create(circle, TweenInfo.new(0.2), {
                Position = state and UDim2.new(1, -16, 0.5, -7) or UDim2.new(0, 2, 0.5, -7)
            }):Play()
            task.spawn(callback, state)
        end

        btn.MouseButton1Click:Connect(function()
            applyToggle(not state)
        end)

        if isDangerous then
            table.insert(dangerousElements, {
                getState = function() return state end,
                setState = applyToggle,
                setYellow = function(yellow)
                    label.TextColor3 = yellow and Color3.fromRGB(255, 215, 0) or Color3.fromRGB(220, 220, 230)
                end
            })
            if trustedMode then
                label.TextColor3 = Color3.fromRGB(255, 215, 0)
            end
        end
    end

    function tabObj:AddSlider(text, min, max, default, callback)
        local targetParent = self:GetNextParent()
        local val = math.clamp(default or min, min, max)

        local sliderFrame = Instance.new("Frame")
        sliderFrame.Size = UDim2.new(1, 0, 0, 44)
        sliderFrame.BackgroundColor3 = Color3.fromRGB(22, 22, 30)
        sliderFrame.BackgroundTransparency = 0.35
        sliderFrame.Parent = targetParent

        local corner = Instance.new("UICorner")
        corner.CornerRadius = UDim.new(0, 6)
        corner.Parent = sliderFrame

        local stroke = Instance.new("UIStroke")
        stroke.Color = Color3.fromRGB(50, 50, 65)
        stroke.Transparency = 0.6
        stroke.Parent = sliderFrame

        local label = Instance.new("TextLabel")
        label.Size = UDim2.new(0.7, 0, 0, 18)
        label.Position = UDim2.new(0, 10, 0, 4)
        label.BackgroundTransparency = 1
        label.Text = text
        label.TextColor3 = Color3.fromRGB(220, 220, 230)
        label.TextSize = 11
        label.Font = Enum.Font.GothamMedium
        label.TextXAlignment = Enum.TextXAlignment.Left
        label.Parent = sliderFrame

        local valLabel = Instance.new("TextLabel")
        valLabel.Size = UDim2.new(0.3, -10, 0, 18)
        valLabel.Position = UDim2.new(0.7, 0, 0, 4)
        valLabel.BackgroundTransparency = 1
        valLabel.Text = tostring(val)
        valLabel.TextColor3 = Color3.fromRGB(160, 160, 175)
        valLabel.TextSize = 11
        valLabel.Font = Enum.Font.Gotham
        valLabel.TextXAlignment = Enum.TextXAlignment.Right
        valLabel.Parent = sliderFrame

        local track = Instance.new("Frame")
        track.Size = UDim2.new(1, -20, 0, 6)
        track.Position = UDim2.new(0, 10, 1, -12)
        track.BackgroundColor3 = Color3.fromRGB(40, 40, 52)
        track.Parent = sliderFrame

        local trackCorner = Instance.new("UICorner")
        trackCorner.CornerRadius = UDim.new(1, 0)
        trackCorner.Parent = track

        local fill = Instance.new("Frame")
        fill.Size = UDim2.new((val - min)/(max - min), 0, 1, 0)
        fill.BackgroundColor3 = Color3.fromRGB(108, 92, 231)
        fill.Parent = track

        local fillCorner = Instance.new("UICorner")
        fillCorner.CornerRadius = UDim.new(1, 0)
        fillCorner.Parent = fill

        local knob = Instance.new("Frame")
        knob.Name = "Knob"
        knob.Size = UDim2.new(0, 14, 0, 14)
        knob.Position = UDim2.new(1, -7, 0.5, -7)
        knob.BackgroundColor3 = Color3.fromRGB(255, 255, 255)
        knob.BorderSizePixel = 0
        knob.Parent = fill

        local knobCorner = Instance.new("UICorner")
        knobCorner.CornerRadius = UDim.new(1, 0)
        knobCorner.Parent = knob

        local knobStroke = Instance.new("UIStroke")
        knobStroke.Color = Color3.fromRGB(108, 92, 231)
        knobStroke.Thickness = 1.5
        knobStroke.Parent = knob

        local trigger = Instance.new("TextButton")
        trigger.Size = UDim2.new(1, 0, 1, 0)
        trigger.BackgroundTransparency = 1
        trigger.Text = ""
        trigger.Parent = sliderFrame

        local draggingSlider = false
        local function updateInput(input)
            local posX = math.clamp((input.Position.X - track.AbsolutePosition.X) / track.AbsoluteSize.X, 0, 1)
            val = math.floor(min + (max - min) * posX)
            valLabel.Text = tostring(val)
            fill.Size = UDim2.new(posX, 0, 1, 0)
            task.spawn(callback, val)
        end

        trigger.InputBegan:Connect(function(input)
            if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
                draggingSlider = true
                updateInput(input)
            end
        end)

        AddConnection(UserInputService.InputChanged:Connect(function(input)
            if draggingSlider and (input.UserInputType == Enum.UserInputType.MouseMovement or input.UserInputType == Enum.UserInputType.Touch) then
                updateInput(input)
            end
        end))

        AddConnection(UserInputService.InputEnded:Connect(function(input)
            if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
                draggingSlider = false
            end
        end))
    end

    function tabObj:AddButton(text, callback)
        local targetParent = self:GetNextParent()
        local btnFrame = Instance.new("Frame")
        btnFrame.Size = UDim2.new(1, 0, 0, 36)
        btnFrame.BackgroundColor3 = Color3.fromRGB(22, 22, 30)
        btnFrame.BackgroundTransparency = 0.35
        btnFrame.Parent = targetParent

        local corner = Instance.new("UICorner")
        corner.CornerRadius = UDim.new(0, 6)
        corner.Parent = btnFrame

        local stroke = Instance.new("UIStroke")
        stroke.Color = Color3.fromRGB(50, 50, 65)
        stroke.Transparency = 0.6
        stroke.Parent = btnFrame

        local btn = Instance.new("TextButton")
        btn.Size = UDim2.new(1, 0, 1, 0)
        btn.BackgroundTransparency = 1
        btn.Text = text
        btn.TextColor3 = Color3.fromRGB(220, 220, 230)
        btn.TextSize = 11
        btn.Font = Enum.Font.GothamSemibold
        btn.Parent = btnFrame

        btn.MouseButton1Click:Connect(function()
            TweenService:Create(btnFrame, TweenInfo.new(0.1), {BackgroundColor3 = Color3.fromRGB(108, 92, 231)}):Play()
            task.wait(0.1)
            TweenService:Create(btnFrame, TweenInfo.new(0.2), {BackgroundColor3 = Color3.fromRGB(22, 22, 30)}):Play()
            task.spawn(callback, btn)
        end)
        return btn
    end

    table.insert(tabs, tabObj)
    if #tabs == 1 then selectTab() end
    return tabObj
end

-- Вкладки под Brookhaven RP
local brookhavenPage = createTab("Brookhaven", "🏡")
local visualsPage    = createTab("Visuals", "👁")
local movementPage   = createTab("Movement", "🏃")
local teleportsPage  = createTab("Teleports", "📍")
local automationPage = createTab("Automation", "⚙")
local miscPage       = createTab("Misc", "🔧")

-- ==================== BROOKHAVEN FUNCTIONS ==================== --

brookhavenPage:AddButton("Auto Rob Bank Vault", function()
    local char = LocalPlayer.Character
    if char and char:FindFirstChild("HumanoidRootPart") then
        -- Телепорт к сейфу
        char.HumanoidRootPart.CFrame = CFrame.new(-25, 4, -460)
        task.wait(0.5)
        -- Попытка взорвать или забрать деньги при взаимодействии
        firetouchinterest(char.HumanoidRootPart, Workspace:FindFirstChild("VaultDoor", true) or char.HumanoidRootPart, 0)
    end
end)

local carSpeedBoost = 1
brookhavenPage:AddSlider("Vehicle Speed Multiplier", 1, 10, 1, function(val)
    carSpeedBoost = val
end)

local vehicleFlyLoop = nil
brookhavenPage:AddToggle("Fly Car / Vehicle", false, true, function(state)
    if state then
        vehicleFlyLoop = task.spawn(function()
            while true do
                local char = LocalPlayer.Character
                if char and char:FindFirstChild("Humanoid") and char.Humanoid.SeatPart then
                    local seat = char.Humanoid.SeatPart
                    local vel = Vector3.zero
                    local cam = Workspace.CurrentCamera

                    if UserInputService:IsKeyDown(Enum.KeyCode.W) then vel += cam.CFrame.LookVector end
                    if UserInputService:IsKeyDown(Enum.KeyCode.S) then vel -= cam.CFrame.LookVector end
                    if UserInputService:IsKeyDown(Enum.KeyCode.A) then vel -= cam.CFrame.RightVector end
                    if UserInputService:IsKeyDown(Enum.KeyCode.D) then vel += cam.CFrame.RightVector end
                    if UserInputService:IsKeyDown(Enum.KeyCode.Space) then vel += Vector3.new(0, 1, 0) end
                    if UserInputService:IsKeyDown(Enum.KeyCode.LeftShift) then vel -= Vector3.new(0, 1, 0) end

                    seat.AssemblyLinearVelocity = vel * (60 * carSpeedBoost)
                end
                task.wait()
            end
        end)
        table.insert(activeLoops, vehicleFlyLoop)
    else
        if vehicleFlyLoop then task.cancel(vehicleFlyLoop) end
    end
end)

brookhavenPage:AddSlider("Avatar Scale Height", 1, 5, 1, function(val)
    if LocalPlayer.Character and LocalPlayer.Character:FindFirstChild("Humanoid") then
        local hum = LocalPlayer.Character.Humanoid
        if hum:FindFirstChild("BodyHeightScale") then
            hum.BodyHeightScale.Value = val
        end
    end
end)

brookhavenPage:AddButton("TP inside Nearest House Safe", function()
    local char = LocalPlayer.Character
    if char and char:FindFirstChild("HumanoidRootPart") then
        for _, obj in ipairs(Workspace:GetDescendants()) do
            if obj.Name == "Safe" and obj:IsA("BasePart") then
                char.HumanoidRootPart.CFrame = obj.CFrame * CFrame.new(0, 0, 3)
                break
            end
        end
    end
end)

-- ==================== VISUALS ==================== --

local playerHighlights = {}
visualsPage:AddToggle("Player ESP", false, function(state)
    if state then
        for _, plr in ipairs(Players:GetPlayers()) do
            if plr ~= LocalPlayer and plr.Character then
                local hl = Instance.new("Highlight")
                hl.Name = "ThoriumPlayerESP"
                hl.FillColor = Color3.fromRGB(108, 92, 231)
                hl.OutlineColor = Color3.fromRGB(255, 255, 255)
                hl.Parent = plr.Character
                table.insert(playerHighlights, hl)
            end
        end
    else
        for _, hl in ipairs(playerHighlights) do if hl then hl:Destroy() end end
        playerHighlights = {}
    end
end)

local nameTags = {}
visualsPage:AddToggle("Name Tags", false, function(state)
    if state then
        for _, plr in ipairs(Players:GetPlayers()) do
            if plr ~= LocalPlayer and plr.Character and plr.Character:FindFirstChild("Head") then
                local bb = Instance.new("BillboardGui")
                bb.Name = "ThoriumNameTag"
                bb.Adornee = plr.Character.Head
                bb.Size = UDim2.new(0, 100, 0, 30)
                bb.StudsOffset = Vector3.new(0, 2, 0)
                bb.AlwaysOnTop = true
                bb.Parent = plr.Character

                local lbl = Instance.new("TextLabel")
                lbl.Size = UDim2.new(1, 0, 1, 0)
                lbl.BackgroundTransparency = 1
                lbl.Text = plr.Name
                lbl.TextColor3 = Color3.fromRGB(255, 255, 255)
                lbl.TextSize = 11
                lbl.Font = Enum.Font.GothamBold
                lbl.Parent = bb
                table.insert(nameTags, bb)
            end
        end
    else
        for _, tag in ipairs(nameTags) do if tag then tag:Destroy() end end
        nameTags = {}
    end
end)

local originalAmbient = Lighting.Ambient
visualsPage:AddToggle("Fullbright", false, function(state)
    Lighting.Ambient = state and Color3.fromRGB(255, 255, 255) or originalAmbient
end)

local origFogEnd = Lighting.FogEnd
visualsPage:AddToggle("No Fog", false, function(state)
    Lighting.FogEnd = state and 9e9 or origFogEnd
end)

local crosshairFrame = nil
visualsPage:AddToggle("Screen Crosshair", false, function(state)
    if state then
        if not crosshairFrame then
            crosshairFrame = Instance.new("Frame")
            crosshairFrame.Size = UDim2.new(0, 6, 0, 6)
            crosshairFrame.Position = UDim2.new(0.5, -3, 0.5, -3)
            crosshairFrame.BackgroundColor3 = Color3.fromRGB(108, 92, 231)
            crosshairFrame.BorderSizePixel = 0
            crosshairFrame.Parent = ScreenGui
            local crCorner = Instance.new("UICorner")
            crCorner.CornerRadius = UDim.new(1, 0)
            crCorner.Parent = crosshairFrame
        end
        crosshairFrame.Visible = true
    else
        if crosshairFrame then crosshairFrame.Visible = false end
    end
end)

-- ==================== MOVEMENT ==================== --

local currentWalkSpeed = 16
local currentJumpPower = 50

AddConnection(LocalPlayer.CharacterAdded:Connect(function(char)
    local hum = char:WaitForChild("Humanoid")
    hum.WalkSpeed = currentWalkSpeed
    hum.UseJumpPower = true
    hum.JumpPower = currentJumpPower
end))

AddConnection(RunService.RenderStepped:Connect(function()
    if LocalPlayer.Character and LocalPlayer.Character:FindFirstChildOfClass("Humanoid") then
        local hum = LocalPlayer.Character:FindFirstChildOfClass("Humanoid")
        if hum.WalkSpeed ~= currentWalkSpeed then hum.WalkSpeed = currentWalkSpeed end
        if hum.JumpPower ~= currentJumpPower then hum.UseJumpPower = true hum.JumpPower = currentJumpPower end
    end
end))

movementPage:AddSlider("Walk Speed", 16, 120, 16, function(val)
    currentWalkSpeed = val
end)

movementPage:AddSlider("Jump Power", 50, 250, 50, function(val)
    currentJumpPower = val
end)

local flying = false
local flySpeed = 60
movementPage:AddToggle("Fly Mode", false, true, function(state)
    flying = state
    if flying then
        local flyLoop = task.spawn(function()
            while flying do
                if LocalPlayer.Character and LocalPlayer.Character:FindFirstChild("HumanoidRootPart") and LocalPlayer.Character:FindFirstChildOfClass("Humanoid") then
                    local hrp = LocalPlayer.Character.HumanoidRootPart
                    local hum = LocalPlayer.Character:FindFirstChildOfClass("Humanoid")
                    hum.PlatformStand = true

                    local cam = Workspace.CurrentCamera
                    local moveDir = Vector3.zero

                    if UserInputService:IsKeyDown(Enum.KeyCode.W) then moveDir += cam.CFrame.LookVector end
                    if UserInputService:IsKeyDown(Enum.KeyCode.S) then moveDir -= cam.CFrame.LookVector end
                    if UserInputService:IsKeyDown(Enum.KeyCode.A) then moveDir -= cam.CFrame.RightVector end
                    if UserInputService:IsKeyDown(Enum.KeyCode.D) then moveDir += cam.CFrame.RightVector end
                    if UserInputService:IsKeyDown(Enum.KeyCode.Space) then moveDir += Vector3.new(0, 1, 0) end
                    if UserInputService:IsKeyDown(Enum.KeyCode.LeftShift) then moveDir -= Vector3.new(0, 1, 0) end

                    if moveDir.Magnitude > 0 then
                        hrp.AssemblyLinearVelocity = moveDir.Unit * flySpeed
                    else
                        hrp.AssemblyLinearVelocity = Vector3.zero
                    end
                end
                task.wait()
            end
            if LocalPlayer.Character and LocalPlayer.Character:FindFirstChildOfClass("Humanoid") then
                LocalPlayer.Character:FindFirstChildOfClass("Humanoid").PlatformStand = false
            end
        end)
        table.insert(activeLoops, flyLoop)
    else
        if LocalPlayer.Character and LocalPlayer.Character:FindFirstChildOfClass("Humanoid") then
            LocalPlayer.Character:FindFirstChildOfClass("Humanoid").PlatformStand = false
        end
    end
end)

local noclip = false
local noclipConn = nil
movementPage:AddToggle("Noclip", false, true, function(state)
    noclip = state
    if noclip then
        noclipConn = AddConnection(RunService.Stepped:Connect(function()
            if noclip and LocalPlayer.Character then
                for _, part in ipairs(LocalPlayer.Character:GetDescendants()) do
                    if part:IsA("BasePart") then part.CanCollide = false end
                end
            end
        end))
    else
        if noclipConn then noclipConn:Disconnect() end
    end
end)

local bhopConnection = nil
movementPage:AddToggle("Bunny Hop", false, function(state)
    if state then
        bhopConnection = AddConnection(RunService.RenderStepped:Connect(function()
            if UserInputService:IsKeyDown(Enum.KeyCode.Space) and LocalPlayer.Character and LocalPlayer.Character:FindFirstChild("Humanoid") then
                if LocalPlayer.Character.Humanoid.FloorMaterial ~= Enum.Material.Air then
                    LocalPlayer.Character.Humanoid:ChangeState("Jumping")
                end
            end
        end))
    else
        if bhopConnection then bhopConnection:Disconnect() end
    end
end)

local infJumpConnection = nil
movementPage:AddToggle("Infinite Jump", false, function(state)
    if state then
        infJumpConnection = AddConnection(UserInputService.JumpRequest:Connect(function()
            if LocalPlayer.Character and LocalPlayer.Character:FindFirstChildOfClass("Humanoid") then
                LocalPlayer.Character:FindFirstChildOfClass("Humanoid"):ChangeState("Jumping")
            end
        end))
    else
        if infJumpConnection then infJumpConnection:Disconnect() end
    end
end)

local spinConnection = nil
movementPage:AddToggle("Spinbot", false, true, function(state)
    if state then
        spinConnection = AddConnection(RunService.RenderStepped:Connect(function()
            if LocalPlayer.Character and LocalPlayer.Character:FindFirstChild("HumanoidRootPart") then
                LocalPlayer.Character.HumanoidRootPart.CFrame *= CFrame.Angles(0, math.rad(30), 0)
            end
        end))
    else
        if spinConnection then spinConnection:Disconnect() end
    end
end)

local originalGravity = Workspace.Gravity
movementPage:AddToggle("Low Gravity", false, function(state)
    Workspace.Gravity = state and 35 or originalGravity
end)

-- ==================== TELEPORTS ==================== --

local locations = {
    ["Bank"] = Vector3.new(-20, 4, -280),
    ["Police Station"] = Vector3.new(-60, 4, -70),
    ["Hospital"] = Vector3.new(-290, 4, 170),
    ["Daycare"] = Vector3.new(220, 4, -130),
    ["Airport"] = Vector3.new(260, 4, -750),
    ["Arcade / Cinema"] = Vector3.new(-120, 4, 180),
    ["Spawn / Center"] = Vector3.new(0, 5, 0)
}

for name, pos in pairs(locations) do
    teleportsPage:AddButton("TP to " .. name, function()
        if LocalPlayer.Character and LocalPlayer.Character:FindFirstChild("HumanoidRootPart") then
            LocalPlayer.Character.HumanoidRootPart.CFrame = CFrame.new(pos)
        end
    end)
end

local clickTpConnection = nil
teleportsPage:AddToggle("Click TP (Ctrl + Click)", false, true, function(state)
    if state then
        clickTpConnection = AddConnection(UserInputService.InputBegan:Connect(function(input, gameProcessed)
            if not gameProcessed and (input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch) then
                if UserInputService:IsKeyDown(Enum.KeyCode.LeftControl) or UserInputService:IsKeyDown(Enum.KeyCode.RightControl) then
                    if Mouse and Mouse.Hit and LocalPlayer.Character and LocalPlayer.Character:FindFirstChild("HumanoidRootPart") then
                        LocalPlayer.Character.HumanoidRootPart.CFrame = CFrame.new(Mouse.Hit.Position + Vector3.new(0, 3, 0))
                    end
                end
            end
        end))
    else
        if clickTpConnection then clickTpConnection:Disconnect() end
    end
end)

-- ==================== AUTOMATION ==================== --

automationPage:AddButton("FPS Booster", function()
    for _, v in ipairs(Workspace:GetDescendants()) do
        if v:IsA("BasePart") then
            v.Material = Enum.Material.SmoothPlastic
        elseif v:IsA("Decal") or v:IsA("Texture") then
            v:Destroy()
        elseif v:IsA("ParticleEmitter") or v:IsA("Smoke") or v:IsA("Fire") then
            v.Enabled = false
        end
    end
    Lighting.GlobalShadows = false
end)

local xrayEnabled = false
automationPage:AddToggle("Wall X-Ray", false, function(state)
    xrayEnabled = state
    for _, part in ipairs(Workspace:GetDescendants()) do
        if part:IsA("BasePart") and not part.Parent:FindFirstChild("Humanoid") then
            part.LocalTransparencyModifier = xrayEnabled and 0.5 or 0
        end
    end
end)

local autoRejoinConn = nil
automationPage:AddToggle("Auto Rejoin on Error", false, function(state)
    if state then
        autoRejoinConn = AddConnection(game:GetService("CoreGui").RobloxPromptGui.promptOverlay.ChildAdded:Connect(function(child)
            if child.Name == "ErrorPrompt" then
                TeleportService:Teleport(game.PlaceId, LocalPlayer)
            end
        end))
    else
        if autoRejoinConn then autoRejoinConn:Disconnect() end
    end
end)

-- ==================== MISC ==================== --

miscPage:AddToggle("Trusted Mode", false, function(state)
    UpdateModeStatus(state)
end)

-- ФУНКЦИЯ UNLOAD (ПОЛНАЯ ВЫГРУЗКА СКРИПТА)
miscPage:AddButton("Unload ThoriumLib", function()
    -- Отмена гетов
    getgenv().ThoriumLib_Loaded = nil

    -- Отключение всех activeConnections
    for _, conn in ipairs(activeConnections) do
        if conn then pcall(function() conn:Disconnect() end) end
    end
    activeConnections = {}

    -- Отмена activeLoops
    for _, loop in ipairs(activeLoops) do
        if loop then pcall(function() task.cancel(loop) end) end
    end
    activeLoops = {}

    -- Сброс настроек
    pcall(function()
        if LocalPlayer.Character and LocalPlayer.Character:FindFirstChildOfClass("Humanoid") then
            local hum = LocalPlayer.Character:FindFirstChildOfClass("Humanoid")
            hum.WalkSpeed = 16
            hum.JumpPower = 50
            hum.PlatformStand = false
        end
        Workspace.Gravity = 196.2
        Lighting.Ambient = originalAmbient or Lighting.Ambient
        Lighting.FogEnd = origFogEnd or Lighting.FogEnd
        Workspace.CurrentCamera.FieldOfView = 70
    end)

    -- Удаление интерфейса
    if UIBlur then UIBlur:Destroy() end
    if ScreenGui then ScreenGui:Destroy() end
    warn("[ThoriumLib] Успешно выгружен из игры.")
end)

local autoClickerLoop = nil
miscPage:AddToggle("Auto Clicker", false, function(state)
    if state then
        autoClickerLoop = task.spawn(function()
            while true do
                mouse1click()
                task.wait(0.02)
            end
        end)
        table.insert(activeLoops, autoClickerLoop)
    else
        if autoClickerLoop then task.cancel(autoClickerLoop) end
    end
end)

local antiAfkConnection = nil
miscPage:AddToggle("Anti-AFK Disconnect", false, function(state)
    if state then
        antiAfkConnection = AddConnection(LocalPlayer.Idled:Connect(function()
            VirtualUser:Button2Down(Vector2.new(0,0), Workspace.CurrentCamera.CFrame)
            task.wait(1)
            VirtualUser:Button2Up(Vector2.new(0,0), Workspace.CurrentCamera.CFrame)
        end))
    else
        if antiAfkConnection then antiAfkConnection:Disconnect() end
    end
end)

miscPage:AddSlider("Camera FOV", 70, 120, 70, function(val)
    Workspace.CurrentCamera.FieldOfView = val
end)

miscPage:AddButton("Copy Job ID to Clipboard", function()
    if setclipboard then
        setclipboard(tostring(game.JobId))
    end
end)

miscPage:AddButton("Server Hop", function()
    local api = "https://games.roblox.com/v1/games/" .. game.PlaceId .. "/servers/Public?sortOrder=Asc&limit=100"
    local success, result = pcall(function() return HttpService:JSONDecode(game:HttpGet(api)) end)
    if success and result and result.data then
        for _, server in ipairs(result.data) do
            if server.playing < server.maxPlayers and server.id ~= game.JobId then
                TeleportService:TeleportToPlaceInstance(game.PlaceId, server.id, LocalPlayer)
                break
            end
        end
    end
end)

miscPage:AddSlider("UI Transparency", 20, 80, 25, function(val)
    local alpha = val / 100
    MainFrame.BackgroundTransparency = alpha
    Sidebar.BackgroundTransparency = alpha + 0.15
end)

miscPage:AddButton("Reset Window Position", function()
    MainFrame.Position = UDim2.new(0.5, -370, 0.5, -230)
end)

local keybindBtnObj
keybindBtnObj = miscPage:AddButton("Keybind: [" .. toggleKey.Name .. "] (Click to change)", function(btnRef)
    isListeningForKey = true
    btnRef.Text = "Press ANY key on keyboard..."
    
    task.spawn(function()
        while isListeningForKey do
            task.wait(0.1)
        end
        btnRef.Text = "Keybind: [" .. toggleKey.Name .. "] (Click to change)"
    end)
end)
